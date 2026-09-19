/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#ifndef REVOKESKILLCOMMAND_H_
#define REVOKESKILLCOMMAND_H_

#include "server/zone/managers/skill/SkillSurrenderDialog.h"

class RevokeSkillCommand : public QueueCommand {
public:
	RevokeSkillCommand(const String& name, ZoneProcessServer* server) : QueueCommand(name, server) {
	}

	int doQueueCommand(CreatureObject* creature, const uint64& target, const UnicodeString& arguments) const {
		if (!checkStateMask(creature))
			return INVALIDSTATE;

		if (!checkInvalidLocomotions(creature))
			return INVALIDLOCOMOTION;

		ManagedReference<SceneObject*> object = creature;
		if (target != 0)
			object = server->getZoneServer()->getObject(target);

		if (object == nullptr || !object->isPlayerCreature()) {
			creature->sendSystemMessage("/revokeSkill requires a player target. Clear your target to revoke your own skills.");
			return INVALIDTARGET;
		}

		CreatureObject* targetCreature = object->asCreatureObject();

		if (targetCreature == nullptr)
			return GENERALERROR;

		return SkillSurrenderDialog::openForAdmin(creature, targetCreature, arguments.toString()) ? SUCCESS : GENERALERROR;
	}
};

#endif // REVOKESKILLCOMMAND_H_
