#ifndef ORPHANSTRUCTURESCOMMAND_H_
#define ORPHANSTRUCTURESCOMMAND_H_

#include "server/zone/managers/structure/OrphanStructureReport.h"

class OrphanStructuresCommand {
public:
	static int executeCommand(CreatureObject* creature, uint64 target, const UnicodeString& arguments) {
		return OrphanStructureReport::execute(creature, arguments.toString());
	}
};

#endif /* ORPHANSTRUCTURESCOMMAND_H_ */
