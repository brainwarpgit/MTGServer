#include "SkillSurrenderDialog.h"

#include <algorithm>
#include <vector>

#include "SkillManager.h"
#include "SkillSurrenderDisplay.h"
#include "SkillSurrenderPlan.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/jedi/JediManager.h"
#include "server/zone/managers/objectcontroller/ObjectController.h"
#include "server/zone/managers/stringid/StringIdManager.h"
#include "server/zone/objects/creature/commands/QueueCommand.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/player/sui/SuiWindowType.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"

namespace {

bool isAvailable(CreatureObject* player) {
	if (player == nullptr || !player->isPlayerCreature() || !player->isOnline())
		return false;

	auto server = player->getZoneServer();
	return server != nullptr && !server->isServerLoading() && !server->isServerShuttingDown();
}

bool canRevoke(CreatureObject* actor) {
	auto ghost = actor->getPlayerObject();
	if (ghost == nullptr || !ghost->hasGodMode() || !ghost->hasAbility("revokeskill"))
		return false;

	auto controller = actor->getZoneServer()->getObjectController();
	if (controller == nullptr)
		return false;
	const auto command = controller->getQueueCommand("revokeskill");
	if (command == nullptr)
		return false;
	const String& ability = command->getCharacterAbility();
	return ability.length() <= 1 || ghost->hasAbility(ability);
}

bool validateContext(CreatureObject* actor, CreatureObject* subject, bool adminMode) {
	if (!isAvailable(actor))
		return false;
	if (adminMode && !canRevoke(actor)) {
		actor->sendSystemMessage("@error_message:insufficient_permissions");
		return false;
	}
	if (!isAvailable(subject) || subject->getZoneServer() != actor->getZoneServer()) {
		actor->sendSystemMessage("The selected player is no longer available. Run the command again to choose a player.");
		return false;
	}
	return adminMode || actor == subject;
}

String commandName(bool adminMode) {
	return adminMode ? "/revokeSkill" : "/surrenderSkill";
}

std::vector<Skill*> getOwnedSkills(CreatureObject* player) {
	std::vector<Skill*> owned;
	const auto skills = player->getSkillList();
	for (int i = 0; i < skills->size(); ++i) {
		if (skills->get(i) != nullptr)
			owned.push_back(skills->get(i));
	}
	std::sort(owned.begin(), owned.end(), [](const Skill* a, const Skill* b) {
		return a->getSkillName().compareTo(b->getSkillName()) < 0;
	});
	return owned;
}

std::vector<String> getNames(const std::vector<Skill*>& skills) {
	std::vector<String> names;
	for (const auto skill : skills)
		names.push_back(skill->getSkillName());
	return names;
}

std::vector<SkillSurrenderDisplay::Entry> getDisplayRows(const std::vector<Skill*>& skills) {
	std::vector<SkillSurrenderDisplay::Entry> rows;
	for (const auto skill : skills) {
		const String& name = skill->getSkillName();
		String label = StringIdManager::instance()->getStringId("@skl_n:" + name).toString();
		if (label.isEmpty())
			label = name;
		rows.push_back({name, label});
	}
	SkillSurrenderDisplay::sort(rows);
	return rows;
}

bool makePlan(CreatureObject* player, const std::vector<Skill*>& owned, Skill* selected, std::vector<Skill*>& plan, bool adminMode) {
	if (!SkillSurrenderPlan::build(owned, selected, plan, adminMode))
		return false;

	Vector<String> names;
	bool hasForceSkill = false;
	for (const auto skill : plan) {
		names.add(skill->getSkillName());
		hasForceSkill = hasForceSkill || skill->getSkillName().beginsWith("force_");
	}

	// Validate the whole batch without removing skills or sending denial dialogs.
	return !hasForceSkill || JediManager::instance()->canSurrenderSkills(player, names);
}

void closeDialogs(CreatureObject* player) {
	auto ghost = player->getPlayerObject();
	ghost->removeSuiBoxType(SuiWindowType::SURRENDER_SKILL_SELECT);
	ghost->removeSuiBoxType(SuiWindowType::SURRENDER_SKILL_CONFIRM);
}

bool isAccepted(CreatureObject* player, SuiBox* box, uint32 eventIndex, int windowType) {
	return eventIndex == 0 && box != nullptr && box->isListBox() && box->getWindowType() == windowType &&
		box->getPlayer().get() == player && isAvailable(player);
}

bool showConfirmation(CreatureObject* actor, CreatureObject* subject, bool adminMode, const String& selectedName);

class ConfirmSkillSurrenderCallback : public SuiCallback {
	ManagedWeakReference<CreatureObject*> target;
	bool adminMode;
	String selectedName;
	std::vector<String> confirmedNames;
	std::vector<String> ownedAtConfirmation;

public:
	ConfirmSkillSurrenderCallback(ZoneServer* server, CreatureObject* subject, bool revoke, const String& selected,
		const std::vector<Skill*>& plan, const std::vector<Skill*>& owned)
		: SuiCallback(server), target(subject), adminMode(revoke), selectedName(selected),
		  confirmedNames(getNames(plan)), ownedAtConfirmation(getNames(owned)) {
	}

	void run(CreatureObject* player, SuiBox* box, uint32 eventIndex, Vector<UnicodeString>* args) override {
		if (!isAccepted(player, box, eventIndex, SuiWindowType::SURRENDER_SKILL_CONFIRM))
			return;

		// Keep the original subject even if the admin changes their current target.
		ManagedReference<CreatureObject*> subject = target.get();
		if (subject == nullptr) {
			player->sendSystemMessage("The selected player is no longer available.");
			return;
		}
		// SuiManager locks the actor; take the subject lock too, then revalidate
		// permission, availability and the complete plan before the first removal.
		Locker subjectLocker(subject, player);
		if (!validateContext(player, subject, adminMode))
			return;

		auto manager = SkillManager::instance();
		const auto owned = getOwnedSkills(subject);
		std::vector<Skill*> plan;
		if (!makePlan(subject, owned, manager->getSkill(selectedName), plan, adminMode)) {
			player->sendSystemMessage("These skills can no longer be removed together. Run " + commandName(adminMode) + " to refresh the list.");
			return;
		}

		if (getNames(owned) != ownedAtConfirmation || getNames(plan) != confirmedNames) {
			player->sendSystemMessage("The player's skills have changed. Review the updated list and confirm again.");
			showConfirmation(player, subject, adminMode, selectedName);
			return;
		}

		// The selected row is deliberately ignored: OK confirms ALL displayed skills.
		int removed = 0;
		for (const auto& name : confirmedNames) {
			if (!subject->hasSkill(name) || !manager->surrenderSkill(name, subject, true, true, adminMode)) {
				player->sendSystemMessage("Skill removal stopped after " + String::valueOf(removed) +
					" skill(s). A skill could not be removed; run " + commandName(adminMode) + " to review the remaining skills.");
				return;
			}
			++removed;
		}
		if (adminMode) {
			player->sendSystemMessage("Revoked " + String::valueOf(removed) + " skill(s) from " + subject->getFirstName() + ".");
			if (subject != player)
				subject->sendSystemMessage("An administrator revoked " + String::valueOf(removed) + " of your skills.");
		} else {
			player->sendSystemMessage("Surrendered " + String::valueOf(removed) + " skill(s).");
		}
	}
};

class SelectSkillSurrenderCallback : public SuiCallback {
	ManagedWeakReference<CreatureObject*> target;
	bool adminMode;
	std::vector<String> choices;

public:
	SelectSkillSurrenderCallback(ZoneServer* server, CreatureObject* subject, bool revoke, const std::vector<String>& skillNames)
		: SuiCallback(server), target(subject), adminMode(revoke), choices(skillNames) {
	}

	void run(CreatureObject* player, SuiBox* box, uint32 eventIndex, Vector<UnicodeString>* args) override {
		if (!isAccepted(player, box, eventIndex, SuiWindowType::SURRENDER_SKILL_SELECT) || args == nullptr || args->size() < 1)
			return;

		const String row = args->get(0).toString();
		if (row.isEmpty() || row.length() > 6)
			return;
		unsigned int index = 0;
		for (int i = 0; i < row.length(); ++i) {
			if (row[i] < '0' || row[i] > '9')
				return;
			index = index * 10 + (row[i] - '0');
		}
		if (index >= choices.size())
			return;

		ManagedReference<CreatureObject*> subject = target.get();
		if (subject == nullptr) {
			player->sendSystemMessage("The selected player is no longer available.");
			return;
		}
		Locker subjectLocker(subject, player);
		if (validateContext(player, subject, adminMode))
			showConfirmation(player, subject, adminMode, choices[index]);
	}
};

bool showConfirmation(CreatureObject* player, CreatureObject* subject, bool adminMode, const String& selectedName) {
	const auto owned = getOwnedSkills(subject);
	std::vector<Skill*> plan;
	if (!makePlan(subject, owned, SkillManager::instance()->getSkill(selectedName), plan, adminMode)) {
		player->sendSystemMessage("That player does not have the skill, or it cannot be removed here with its dependent skills. Run " + commandName(adminMode) + " to see the available choices.");
		return false;
	}

	closeDialogs(player);
	ManagedReference<SuiListBox*> list = new SuiListBox(player, SuiWindowType::SURRENDER_SKILL_CONFIRM);
	list->setCallback(new ConfirmSkillSurrenderCallback(player->getZoneServer(), subject, adminMode, selectedName, plan, owned));
	list->setUsingObject(player);
	list->setPromptTitle(adminMode ? "Confirm skill revocation" : "Confirm skill surrender");
	int points = 0;
	for (const auto skill : plan) {
		points += skill->getSkillPointsRequired();
	}
	// Only the presentation is alphabetical. The saved plan still removes
	// dependent skills before their prerequisites.
	for (const auto& row : getDisplayRows(plan))
		list->addMenuItem(row.displayName);
	StringBuffer prompt;
	prompt << "Player: " << subject->getFirstName() << "\n"
		<< (adminMode ? "Revoke ALL " : "Surrender ALL ") << static_cast<int>(plan.size()) << " skills listed below?"
		<< "\nThis includes the chosen skill and every learned skill that requires it, including skills in other professions."
		<< "\nSkill points returned: " << points
		<< "\n\nReview the entire list. Press OK to drop every listed skill, or Cancel to keep them.";
	list->setPromptText(prompt.toString());
	list->setOkButton(true, "@ok");
	list->setCancelButton(true, "@cancel");
	list->setOtherButton(false, "");
	player->getPlayerObject()->addSuiBox(list);
	player->sendMessage(list->generateMessage());
	return true;
}

bool openDialog(CreatureObject* player, CreatureObject* subject, bool adminMode, const String& arguments) {
	if (player == nullptr || subject == nullptr)
		return false;
	Locker locker(player);
	Locker subjectLocker(subject, player);
	if (!validateContext(player, subject, adminMode))
		return false;

	closeDialogs(player);
	StringTokenizer tokens(arguments);
	if (tokens.hasMoreTokens()) {
		String selectedName;
		tokens.getStringToken(selectedName);
		if (tokens.hasMoreTokens()) {
			player->sendSystemMessage("Syntax: " + commandName(adminMode) + " [skill_name]. Omit the name to choose from the player's skills.");
			return false;
		}
		return showConfirmation(player, subject, adminMode, selectedName);
	}

	const auto owned = getOwnedSkills(subject);
	std::vector<Skill*> eligible;
	for (const auto skill : owned) {
		std::vector<Skill*> plan;
		if (makePlan(subject, owned, skill, plan, adminMode))
			eligible.push_back(skill);
	}
	if (eligible.empty()) {
		player->sendSystemMessage("That player has no skills that can currently be removed here.");
		return true;
	}
	const auto rows = getDisplayRows(eligible);
	std::vector<String> choices;
	for (const auto& row : rows)
		choices.push_back(row.skillName);

	ManagedReference<SuiListBox*> list = new SuiListBox(player, SuiWindowType::SURRENDER_SKILL_SELECT);
	list->setCallback(new SelectSkillSurrenderCallback(player->getZoneServer(), subject, adminMode, choices));
	list->setUsingObject(player);
	list->setPromptTitle(adminMode ? "Revoke a skill" : "Surrender a skill");
	StringBuffer prompt;
	prompt << "Player: " << subject->getFirstName()
		<< "\nChoose a skill to remove. Skills that depend on it will also be removed."
		<< "\nThe next window lists every affected skill and asks for confirmation. Internal and protected progression skills are excluded.";
	if (!adminMode)
		prompt << " Pilot retirement uses its existing system.";
	list->setPromptText(prompt.toString());
	list->setOkButton(true, "@ok");
	list->setCancelButton(true, "@cancel");
	list->setOtherButton(false, "");
	for (const auto& row : rows)
		list->addMenuItem(row.displayName);
	player->getPlayerObject()->addSuiBox(list);
	player->sendMessage(list->generateMessage());
	return true;
}

} // namespace

bool SkillSurrenderDialog::open(CreatureObject* player, const String& arguments) {
	return openDialog(player, player, false, arguments);
}

bool SkillSurrenderDialog::openForAdmin(CreatureObject* admin, CreatureObject* target, const String& arguments) {
	return openDialog(admin, target, true, arguments);
}
