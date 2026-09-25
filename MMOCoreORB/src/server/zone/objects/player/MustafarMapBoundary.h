/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions. */

#ifndef MUSTAFARMAPBOUNDARY_H_
#define MUSTAFARMAPBOUNDARY_H_

#include "engine/engine.h"
#include <cmath>

namespace server {
namespace zone {
class Zone;
namespace objects {
namespace creature {
class CreatureObject;
}
}
}
}

class MustafarMapBoundary {
public:
	// Authored Mustafar map/heightmap rectangle, in world X/Y coordinates.
	static bool contains(float x, float y) {
		return std::isfinite(x) && std::isfinite(y) && x >= -6880.f && x <= 1120.f && y >= -1024.f && y <= 6976.f;
	}

	static bool enforceMovement(server::zone::objects::creature::CreatureObject* player, const Vector3& position, uint64 parentID);

	// Returns false when an invalid destination has no configured safe recovery.
	// Valid cell positions remain local to their original parent.
	static bool resolveDestination(server::zone::objects::creature::CreatureObject* player, server::zone::Zone* zone, Vector3& position, uint64& parentID);
};

#endif /* MUSTAFARMAPBOUNDARY_H_ */
