#ifndef STRUCTUREOWNERSHIP_H_
#define STRUCTUREOWNERSHIP_H_

#include <cstdint>

namespace StructureOwnership {

inline bool requiresPlayerOwner(std::uint64_t objectID, bool persistent, bool clientObject, bool civic) {
	// Database namespace zero contains world snapshot objects. Screenplay
	// structures can also be temporary, and civic buildings belong to a city.
	return (objectID >> 48) != 0 && persistent && !clientObject && !civic;
}

template<typename Structure>
bool requiresPlayerMaintenance(Structure* structure) {
	// Keep existing maintenance for assigned owners, including orphan cleanup
	// when a nonzero owner ID no longer identifies a player.
	return structure->getOwnerObjectID() != 0 || requiresPlayerOwner(structure->getObjectID(),
		structure->isPersistent(), structure->isClientObject(), structure->isCivicStructure());
}

} // namespace StructureOwnership

#endif /* STRUCTUREOWNERSHIP_H_ */
