/*
 * MeshAppearanceTemplate.cpp
 *
 *  Created on: 07/12/2010
 *      Author: victor
 */

#include "MeshAppearanceTemplate.h"

void MeshAppearanceTemplate::parse(IffStream* iffStream) {
	if (iffStream->openForm('MESH') == nullptr) {
		throw Exception(String("Missing MESH form in ") + iffStream->getFileName());
	}

	uint32 version = iffStream->getNextFormType();
	if (iffStream->openForm(version) == nullptr) {
		throw Exception(String("Missing mesh version form in ") + iffStream->getFileName());
	}

	const bool legacy = version == '0003';
	if (!legacy) {
		AppearanceTemplate::readObject(iffStream);
	}

	parseSPS(iffStream, legacy);

	if (legacy) {
		Chunk* center = iffStream->openChunk('CNTR');
		if (center == nullptr || center->getChunkSize() != 12) {
			throw Exception(String("Invalid legacy mesh center in ") + iffStream->getFileName());
		}
		iffStream->closeChunk('CNTR');

		Chunk* radius = iffStream->openChunk('RADI');
		if (radius == nullptr || radius->getChunkSize() != 4) {
			throw Exception(String("Invalid legacy mesh radius in ") + iffStream->getFileName());
		}
		iffStream->closeChunk('RADI');
	}

	iffStream->closeForm(version);
	if (legacy) {
		readLegacyMeshBounds(iffStream);
	}
	iffStream->closeForm('MESH');

	if (meshes.size() != 0) {
		createAABB();
	}
}

void MeshAppearanceTemplate::createAABB() {
	Vector<Triangle*> triangles;

	for (int k = 0; k < meshes.size(); ++k) {
		MeshData* meshData = meshes.get(k);

		for (int i = 0; i < meshData->triangles.size(); ++i) {
			MeshTriangle* tri = &meshData->triangles.getUnsafe(i);

			int pointA = tri->verts[0];
			int pointB = tri->verts[1];
			int pointC = tri->verts[2];

			Vector3* vert1 = &meshData->vertices.get(pointA);
			Vector3* vert2 = &meshData->vertices.get(pointB);
			Vector3* vert3 = &meshData->vertices.get(pointC);

			Vector3 trian[3];
			trian[0] = Vector3(vert1->getX(), vert1->getY(), vert1->getZ());
			trian[1] = Vector3(vert2->getX(), vert2->getY(), vert2->getZ());
			trian[2] = Vector3(vert3->getX(), vert3->getY(), vert3->getZ());

			triangles.add(new Triangle(trian));
		}
	}

	if (triangles.size() == 0) {
		return;
	}

	//Logger::console.info("creating mesh aabb for triangles " + String::valueOf(triangles.size()), true);

	AABBTreeHeuristic heurData;
	heurData.maxdepth = 4; // maximum depth
	heurData.mintricnt = 5; // minimum triangle count
	heurData.tartricnt = 10; // target triangle count
	heurData.minerror = 0.5f; // minimum error required
	heurData.storePrimitives = true;

	aabbTree = new AABBTree(triangles, 0, heurData);

	AABB& box = aabbTree->getBoundingBox();

	Vector3 center = box.center();
	Vector3 extents = box.extents();
	float radius = extents.length();

	boundingSphere = new Sphere(center, radius);
}

/*AABNode* MeshAppearanceTemplate::createNewAABB(float rotationRadians = 0) {

}*/

bool MeshAppearanceTemplate::testCollide(float x, float z, float y, float radius) const {
	Vector3 point(x, z, y);

	Sphere sphere(point, radius);

	//Logger::console.info("checking collide in mesh", true);

	return aabbTree != nullptr && aabbTree->testCollide(sphere);
}

void MeshAppearanceTemplate::parseSPS(IffStream* iffStream, bool legacy) {
	if (iffStream->openForm('SPS ') == nullptr) {
		throw Exception(String("Missing mesh shader groups in ") + iffStream->getFileName());
	}

	const uint32 version = legacy ? '0000' : '0001';
	if (iffStream->getNextFormType() != version || iffStream->openForm(version) == nullptr) {
		throw Exception(String("Unsupported mesh shader group version in ") + iffStream->getFileName());
	}

	Chunk* countChunk = iffStream->openChunk('CNT ');
	if (countChunk == nullptr || countChunk->getChunkSize() != 4) {
		throw Exception(String("Invalid mesh shader group count in ") + iffStream->getFileName());
	}

	int count = iffStream->getInt();

	iffStream->closeChunk();
	if (count < 0 || count > iffStream->getRemainingSubChunksNumber()) {
		throw Exception(String("Invalid mesh shader group count in ") + iffStream->getFileName());
	}

	for (int i = 1; i <= count; ++i) {
		parseVertexData(iffStream, i, legacy);
	}

	iffStream->closeForm(version);
	iffStream->closeForm('SPS ');
}

void MeshAppearanceTemplate::parseVertexData(IffStream* iffStream, int idx, bool legacy) {
	int formVersion = 0;

	String idxText = String::valueOf(idx);
	int lengthOfText = idxText.length();

	for (int i = 0; i < 4 - lengthOfText; ++i)
		idxText = "0" + idxText;

	for (int i = 0; i < 4; ++i) {
		char a = idxText.charAt(i);

		formVersion = formVersion << 8;
		formVersion += (int)a;
	}

	if (iffStream->openForm(formVersion) == nullptr) {
		throw Exception(String("Missing mesh shader group in ") + iffStream->getFileName());
	}

	if (iffStream->openChunk('NAME') == nullptr) {
		throw Exception(String("Missing mesh shader name in ") + iffStream->getFileName());
	}

	String shaderName;
	iffStream->getString(shaderName);

	iffStream->closeChunk();

	if (iffStream->openChunk('INFO') == nullptr) {
		throw Exception(String("Missing mesh shader information in ") + iffStream->getFileName());
	}
	iffStream->closeChunk();

	uint32 nextVersion = iffStream->getNextFormType();
	if (iffStream->openForm(nextVersion) == nullptr) {
		throw Exception(String("Missing mesh shader data in ") + iffStream->getFileName());
	}

	if (iffStream->openChunk('INFO') == nullptr) {
		throw Exception(String("Missing mesh primitive information in ") + iffStream->getFileName());
	}
	iffStream->closeChunk();

	Reference<MeshData*> meshData = new MeshData;
	meshData->readObject(iffStream, legacy);
	meshes.emplace(std::move(meshData));

	iffStream->closeForm(nextVersion);

	iffStream->closeForm(formVersion);
}
