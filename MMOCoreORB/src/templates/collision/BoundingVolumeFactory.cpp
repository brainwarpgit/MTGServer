#include "BoundingVolumeFactory.h"
#include "BaseBoundingVolume.h"
#include "templates/collision/BoundingVolumes.h"
#include <memory>

BaseBoundingVolume* BoundingVolumeFactory::getVolume(IffStream *iff) {
	static Logger logger("BoundingVolumeFactory");

	std::unique_ptr<BaseBoundingVolume> volume;
	uint32 type = iff->getNextFormType();
	switch(type) {
		case 'NULL':
			iff->openForm('NULL');
			iff->closeForm('NULL');
			return nullptr;
		case 'EXBX':
			volume.reset(new BoxVolume());
			volume->read(iff);
			return volume.release();
		case 'CPST':
			volume.reset(new CompositeVolume());
			break;
		case 'CMSH':
			volume.reset(new CollisionMeshVolume());
			break;
		case 'CMPT':
			volume.reset(new ComponentVolume());
			break;
		case 'DTAL':
			volume.reset(new DetailVolume());
			break;
		case 'XCYL':
			volume.reset(new CylinderVolume());
			break;
		case 'EXSP':
			volume.reset(new SphereVolume());
			volume->read(iff);
			return volume.release();
		default: {
			logger.error(iff->getFileName() + " - INVALID VOLUME TYPE " + String::hexvalueOf((int64)type));
			iff->openForm(type);
			iff->closeForm(type);
			return nullptr;
		}
	}
	iff->openForm(type);
	volume->read(iff);
	iff->closeForm(type);

	return volume.release();
}
