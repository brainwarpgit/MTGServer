#ifndef TRAVELSTATUSCOMMAND_H_
#define TRAVELSTATUSCOMMAND_H_

#include "server/zone/TravelStartupTask.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/sui/messagebox/SuiMessageBox.h"

class TravelStatusCommand {
public:
	static bool parseArguments(const String& arguments, String& planet, int& page, bool& includeReady) {
		planet = "all";
		page = 1;
		includeReady = false;
		StringTokenizer tokens(arguments);

		if (!tokens.hasMoreTokens())
			return true;

		String mode;
		tokens.getStringToken(mode);
		mode = mode.toLowerCase();

		if (mode != "issues" && mode != "all")
			return false;

		includeReady = mode == "all";

		if (tokens.hasMoreTokens()) {
			tokens.getStringToken(planet);
			planet = planet.toLowerCase();
		}

		if (tokens.hasMoreTokens()) {
			String value;
			tokens.getStringToken(value);

			if (value.isEmpty() || value.length() > 6)
				return false;

			page = 0;

			for (int i = 0; i < value.length(); ++i) {
				if (value[i] < '0' || value[i] > '9')
					return false;

				page = page * 10 + value[i] - '0';
			}

			if (page < 1)
				return false;
		}

		return !tokens.hasMoreTokens();
	}

	static int executeCommand(CreatureObject* creature, uint64 target, const UnicodeString& arguments) {
		if (creature == nullptr || !creature->isPlayerCreature())
			return 1;

		auto ghost = creature->getPlayerObject();

		if (ghost == nullptr || ghost->getAdminLevel() < 15)
			return 1;

		String planet;
		int page;
		bool includeReady;

		if (!parseArguments(arguments.toString(), planet, page, includeReady)) {
			creature->sendSystemMessage("Syntax: /server travelstatus [issues|all] [planet|all] [page]");
			return 1;
		}

		auto zoneServer = creature->getZoneServer();

		if (zoneServer == nullptr)
			return 1;

		if (planet != "all") {
			auto zone = zoneServer->getZone(planet);

			if (zone == nullptr || !zone->isGroundZone()) {
				creature->sendSystemMessage("That is not an enabled ground planet. Use its scene name, such as naboo or kashyyyk_main.");
				return 1;
			}
		}

		Reference<TravelStartupTask*> startup = zoneServer->getTravelStartupTask();

		if (startup == nullptr) {
			creature->sendSystemMessage("No shuttle startup report is available for this server session.");
			return 1;
		}

		ManagedReference<SuiMessageBox*> box = new SuiMessageBox(creature, 0);
		box->setPromptTitle("Shuttle and Starport Startup Report");
		box->setPromptText(startup->getStartupReport(planet, page, includeReady));
		box->setOkButton(true, "Close");
		ghost->addSuiBox(box);
		creature->sendMessage(box->generateMessage());
		return 0;
	}
};

#endif /* TRAVELSTATUSCOMMAND_H_ */
