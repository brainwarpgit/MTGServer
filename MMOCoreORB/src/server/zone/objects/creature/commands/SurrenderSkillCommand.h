/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef SURRENDERSKILLCOMMAND_H_
#define SURRENDERSKILLCOMMAND_H_

#include "server/zone/managers/skill/SkillSurrenderDialog.h"

class SurrenderSkillCommand : public QueueCommand {
public:
	SurrenderSkillCommand(const String& name, ZoneProcessServer* server) : QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		return SkillSurrenderDialog::open(creature, arguments.toString()) ? SUCCESS : GENERALERROR;
	}
};

#endif // SURRENDERSKILLCOMMAND_H_
