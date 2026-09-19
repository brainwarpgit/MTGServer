#ifndef SKILLSURRENDERDIALOG_H_
#define SKILLSURRENDERDIALOG_H_

#include "server/zone/objects/creature/CreatureObject.h"

class SkillSurrenderDialog {
public:
	// Empty arguments open the skill list; a skill name opens its confirmation.
	static bool open(CreatureObject* player, const String& arguments);
	static bool openForAdmin(CreatureObject* admin, CreatureObject* target, const String& arguments);
};

#endif /* SKILLSURRENDERDIALOG_H_ */
