#pragma once

#include "server/zone/Zone.h"
#include "server/zone/objects/scene/SceneObject.h"

namespace DatabaseZoneInsertion {

// The saved zone is only an insertion request. Storing or moving the object
// before its deferred task runs invalidates that request.
inline bool insertIfUnchanged(SceneObject* object, Zone* savedZone) {
	if (object == nullptr || savedZone == nullptr) {
		return false;
	}

	Locker locker(object);

	if (object->getLocalZone() != savedZone) {
		return false;
	}

	// Active areas use separate spatial trees and retain their normal insertion
	// path. Do not reject an object just because it has a saved parent.
	if (object->isInQuadTree() || object->isInOctree()) {
		return false;
	}

	return savedZone->transferObject(object, -1, true);
}

} // namespace DatabaseZoneInsertion
