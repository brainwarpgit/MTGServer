#ifndef ORPHANSTRUCTUREREPORT_H_
#define ORPHANSTRUCTUREREPORT_H_

#include "server/zone/objects/creature/CreatureObject.h"

class OrphanStructureReport {
public:
	static bool parseArguments(const String& arguments, String& planet, int& page);
	static int execute(CreatureObject* player, const String& arguments);
};

#endif /* ORPHANSTRUCTUREREPORT_H_ */
