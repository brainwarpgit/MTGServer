/*
 * MeshData.h
 *
 *  Created on: 04/22/2016
 *      Author: gslomin
 */

#include "MeshData.h"

AABB MeshData::buildAABB() const {
	float minx = 100000;
	float miny = 100000;
	float minz = 100000;

	float maxx = -1000000;
	float maxy = -1000000;
	float maxz = -1000000;

	for (int i = 0; i < vertices.size(); i++) {
		const Vector3& vert = vertices.getUnsafe(i);
		const float x = vert.getX();
		const float y = vert.getY();
		const float z = vert.getZ();

		if (x < minx)
			minx = x;

		if (y < miny)
			miny = y;

		if (z < minz)
			minz = z;

		if (x > maxx)
			maxx = x;

		if (y > maxy)
			maxy = y;

		if (z > maxz)
			maxz = z;
	}

	return AABB(Vector3(minx, miny, minz), Vector3(maxx, maxy, maxz));
}

Vector <MeshTriangle>* MeshData::getMeshWithinBounds(AABB& bounds) const {
	float bminx = bounds.getXMin();
	float bminy = bounds.getYMin();
	float bminz = bounds.getZMin();

	float bmaxx = bounds.getXMax();
	float bmaxy = bounds.getYMax();
	float bmaxz = bounds.getZMax();

	Vector <MeshTriangle>* tris = new Vector<MeshTriangle>();

	for (int i = 0; i < triangles.size(); i++) {
		const MeshTriangle& tri = triangles.getUnsafe(i);
		const int* v = tri.getVerts();
		float minx = 30000;
		float miny = 30000;
		float minz = 30000;

		float maxx = -30000;
		float maxy = -30000;
		float maxz = -30000;

		for (int i = 0; i < 3; i++) {
			const Vector3& vert = vertices.get(v[i]);
			const float x = vert.getX();
			const float y = vert.getY();
			const float z = vert.getZ();

			if (x > maxx)
				maxx = x;

			if (y > maxy)
				maxy = y;

			if (z > maxz)
				maxz = z;

			if (x < minx)
				minx = x;

			if (y < miny)
				miny = y;

			if (z < minz)
				minz = z;
		}
		if (maxx >= bminx && minx <= bmaxx && maxy >= bminy && miny <= bmaxy && maxz >= bminz && minz <= bmaxz)
			tris->add(tri);
	}

	return tris;
}

void MeshData::readObject(IffStream* iffStream, bool legacy) {
	if (iffStream->openForm('VTXA') == nullptr) {
		throw Exception(String("Missing mesh vertex array in ") + iffStream->getFileName());
	}

	const uint32 version = legacy ? '0002' : '0003';
	if (iffStream->getNextFormType() != version || iffStream->openForm(version) == nullptr) {
		throw Exception(String("Unsupported mesh vertex array version in ") + iffStream->getFileName());
	}

	Chunk* vertexInfo = iffStream->openChunk('INFO');
	if (vertexInfo == nullptr || vertexInfo->getChunkSize() < 8) {
		throw Exception(String("Invalid mesh vertex information in ") + iffStream->getFileName());
	}

	iffStream->getInt(); //unk

	int numVertices = iffStream->getInt();

	iffStream->closeChunk();

	Chunk* vertexDataChunk = iffStream->openChunk('DATA');
	if (vertexDataChunk == nullptr) {
		throw Exception(String("Missing mesh vertex data in ") + iffStream->getFileName());
	}

	int vertexDataChunkSize = vertexDataChunk->getChunkSize();
	if (numVertices <= 0 || vertexDataChunkSize % numVertices != 0 || vertexDataChunkSize / numVertices < 12) {
		throw Exception(String("Invalid mesh vertex count or stride in ") + iffStream->getFileName());
	}

	int intBytesPerVertex = vertexDataChunkSize / numVertices;

	vertices.removeAll(numVertices);

	for (int i = 0; i < numVertices; ++i) {
		vertices.emplace(iffStream->getVector3());

		vertexDataChunk->shiftOffset(intBytesPerVertex - 12);
	}

	iffStream->closeChunk('DATA');
	iffStream->closeForm(version);
	iffStream->closeForm('VTXA');

	Chunk* indexData = iffStream->openChunk('INDX');
	if (indexData == nullptr) {
		throw Exception(String("Missing mesh indices in ") + iffStream->getFileName());
	}

	const int indexBytes = indexData->getChunkSize();
	int indexCount = 0;
	bool wideIndices = legacy;

	if (legacy) {
		if (indexBytes % 4 != 0) {
			throw Exception(String("Invalid legacy mesh index size in ") + iffStream->getFileName());
		}
		indexCount = indexBytes / 4;
	} else {
		if (indexBytes < 4 || (indexBytes - 4) % 2 != 0) {
			throw Exception(String("Invalid mesh index size in ") + iffStream->getFileName());
		}

		indexCount = iffStream->getInt();
		const int payloadBytes = indexBytes - 4;

		if (indexCount < 0) {
			throw Exception(String("Invalid mesh index count in ") + iffStream->getFileName());
		} else if (indexCount == payloadBytes / 2) {
			wideIndices = false;
		} else if (payloadBytes % 4 == 0 && indexCount == payloadBytes / 4) {
			wideIndices = true;
		} else {
			throw Exception(String("Invalid mesh index count in ") + iffStream->getFileName());
		}
	}

	if (indexCount % 3 != 0) {
		throw Exception(String("Incomplete mesh triangle in ") + iffStream->getFileName());
	}

	triangles.removeAll(indexCount / 3);

	for (int i = 0; i < indexCount; i += 3) {
		uint32 a = wideIndices ? iffStream->getUnsignedInt() : static_cast<uint16>(indexData->readShort());
		uint32 b = wideIndices ? iffStream->getUnsignedInt() : static_cast<uint16>(indexData->readShort());
		uint32 c = wideIndices ? iffStream->getUnsignedInt() : static_cast<uint16>(indexData->readShort());

		if (a >= static_cast<uint32>(numVertices) || b >= static_cast<uint32>(numVertices) || c >= static_cast<uint32>(numVertices)) {
			throw Exception(String("Mesh triangle index out of range in ") + iffStream->getFileName());
		}

		MeshTriangle triangle;
		triangle.verts[0] = a;
		triangle.verts[1] = b;
		triangle.verts[2] = c;

		triangles.add(triangle);
	}

	iffStream->closeChunk();
}
