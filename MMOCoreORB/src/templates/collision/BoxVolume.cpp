#include "BoxVolume.h"

void BoxVolume::read(IffStream *iff) {
	if (iff->openForm('EXBX') == nullptr) {
		throw Exception(String("Missing EXBX form in ") + iff->getFileName());
	}

	uint32 version = iff->getNextFormType();
	if (version != '0000' && version != '0001') {
		throw Exception(String("Unsupported EXBX version ") + String::hexvalueOf((int64)version) + " in " + iff->getFileName());
	}

	if (iff->openForm(version) == nullptr) {
		throw Exception(String("Missing EXBX version form in ") + iff->getFileName());
	}

	if (version == '0001') {
		BaseBoundingVolume::read(iff);
	}

	Chunk* bounds = iff->openChunk('BOX ');
	if (bounds == nullptr || bounds->getChunkSize() != 24) {
		throw Exception(String("Invalid BOX chunk in ") + iff->getFileName());
	}

	float maxx = iff->getFloat();
	float maxy = iff->getFloat();
	float maxz = iff->getFloat();

	float minx = iff->getFloat();
	float miny = iff->getFloat();
	float minz = iff->getFloat();

	bbox = AABB(Vector3(minx, miny, minz), Vector3(maxx, maxy, maxz));

	iff->closeChunk('BOX ');

	if (version == '0000') {
		Chunk* center = iff->openChunk('CNTR');
		if (center == nullptr || center->getChunkSize() != 12) {
			throw Exception(String("Invalid legacy CNTR chunk in ") + iff->getFileName());
		}

		float x = iff->getFloat();
		float y = iff->getFloat();
		float z = iff->getFloat();
		iff->closeChunk('CNTR');

		Chunk* radiusChunk = iff->openChunk('RADI');
		if (radiusChunk == nullptr || radiusChunk->getChunkSize() != 4) {
			throw Exception(String("Invalid legacy RADI chunk in ") + iff->getFileName());
		}

		float radius = iff->getFloat();
		iff->closeChunk('RADI');

		bsphere = Sphere(Vector3(x, y, z), radius);
	}

	iff->closeForm(version);
	iff->closeForm('EXBX');
}
#ifdef OSG_RENDERER
osg::ref_ptr<osg::Node> BoxVolume::draw() const {
	Vector3 boxCenter = bbox.center();
	Vector3 boxExtents = bbox.extents();
	osg::Geode* geode = new osg::Geode();
	geode->addDrawable( new osg::ShapeDrawable( new osg::Box(osg::Vec3(boxCenter.getX(), boxCenter.getY(), boxCenter.getZ()), boxExtents.getX()*2, boxExtents.getY()*2, boxExtents.getZ()*2)) );

	geode->getOrCreateStateSet()->setAttribute( new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK,osg::PolygonMode::LINE) );

	return geode;
}
#endif
