#ifndef SKILLSURRENDERPLAN_H_
#define SKILLSURRENDERPLAN_H_

#include "server/zone/objects/creature/variables/Skill.h"

#include <algorithm>
#include <cstring>
#include <vector>

class SkillSurrenderPlan {
	static bool visit(std::size_t index, const std::vector<Skill*>& owned, std::vector<unsigned char>& state,
		std::vector<Skill*>& plan, bool allowPilot) {
		if (state[index] == 1)
			return false;
		if (state[index] == 2)
			return true;

		Skill* skill = owned[index];
		if (!isSelectable(skill, allowPilot))
			return false;

		state[index] = 1;
		for (std::size_t i = 0; i < owned.size(); ++i) {
			// This accessor means that owned[i] requires skill, not the reverse.
			if (owned[i]->isRequiredSkillOf(skill) && !visit(i, owned, state, plan, allowPilot))
				return false;
		}

		state[index] = 2;
		plan.push_back(skill);
		return true;
	}

public:
	static bool isSelectable(const Skill* skill, bool allowPilot = false) {
		if (skill == nullptr || skill->isHidden() || skill->isGodOnly() || skill->isSkill())
			return false;

		const String& name = skill->getSkillName();
		if (name.beginsWith("force_title_") || name.beginsWith("force_rank_"))
			return false;

		// Administrative revocation already permits pilot boxes, which cost no
		// ground skill points. Normal surrender still uses pilot retirement.
		if (name.beginsWith("pilot_"))
			return allowPilot && skill->getSkillPointsRequired() >= 0;

		// Politician boxes and force-sensitive novice/master boxes cost zero
		// points. Other zero-point entries include species and progression markers.
		return skill->getSkillPointsRequired() > 0 ||
			(skill->getSkillPointsRequired() == 0 && (name.beginsWith("social_politician_") ||
				(name.beginsWith("force_sensitive_") && (name.endsWith("_novice") || name.endsWith("_master")))));
	}

	static bool build(const std::vector<Skill*>& owned, Skill* selected, std::vector<Skill*>& plan, bool allowPilot = false) {
		plan.clear();
		if (!isSelectable(selected, allowPilot) || std::find(owned.begin(), owned.end(), selected) == owned.end())
			return false;

		std::vector<Skill*> uniqueOwned;
		for (Skill* skill : owned) {
			if (skill != nullptr && std::find(uniqueOwned.begin(), uniqueOwned.end(), skill) == uniqueOwned.end())
				uniqueOwned.push_back(skill);
		}
		std::sort(uniqueOwned.begin(), uniqueOwned.end(), [](const Skill* left, const Skill* right) {
			return std::strcmp(left->getSkillName().toCharArray(), right->getSkillName().toCharArray()) < 0;
		});

		std::vector<unsigned char> state(uniqueOwned.size(), 0);
		const auto selectedIndex = std::find(uniqueOwned.begin(), uniqueOwned.end(), selected) - uniqueOwned.begin();
		if (!visit(selectedIndex, uniqueOwned, state, plan, allowPilot)) {
			plan.clear();
			return false;
		}
		return true;
	}
};

#endif /* SKILLSURRENDERPLAN_H_ */
