#include "gtest/gtest.h"

#include "templates/appearance/MeshAppearanceTemplate.h"
#include "templates/appearance/DetailAppearanceTemplate.h"
#include "templates/collision/BoxVolume.h"

#include <cmath>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <iterator>
#include <string>
#include <vector>

namespace {

// Construct IFFs independently of the reader. Chunk lengths are big endian;
// numeric payloads are little endian. No extracted game assets are required.
using Bytes = std::string;

Bytes little32(uint32_t value) {
	Bytes bytes;
	for (unsigned shift = 0; shift < 32; shift += 8)
		bytes.push_back(static_cast<char>(value >> shift));
	return bytes;
}

Bytes real(float value) {
	uint32_t bits;
	std::memcpy(&bits, &value, sizeof(bits));
	return little32(bits);
}

Bytes vector(float x, float y, float z) {
	return real(x) + real(y) + real(z);
}

Bytes chunk(const char* tag, const Bytes& payload) {
	Bytes bytes(tag, 4);
	for (int shift = 24; shift >= 0; shift -= 8)
		bytes.push_back(static_cast<char>(payload.size() >> shift));
	return bytes + payload;
}

Bytes form(const char* tag, const Bytes& contents) {
	return chunk("FORM", Bytes(tag, 4) + contents);
}

struct Geometry {
	int32_t vertexCount = 24;
	Bytes vertices;
	std::vector<uint32_t> indices;
	int32_t indexCount = 36;
	int modernIndexBytes = 2;
	int indexBytesToDrop = 0;

	Geometry() {
		// Six independently shaded cube faces: 24 vertices and 12 triangles,
		// matching the original default appearance's layout and extent.
		const float faces[6][4][3] = {
			{{-.5f, -.5f, .5f}, {.5f, -.5f, .5f}, {.5f, .5f, .5f}, {-.5f, .5f, .5f}},
			{{.5f, -.5f, -.5f}, {-.5f, -.5f, -.5f}, {-.5f, .5f, -.5f}, {.5f, .5f, -.5f}},
			{{.5f, -.5f, .5f}, {.5f, -.5f, -.5f}, {.5f, .5f, -.5f}, {.5f, .5f, .5f}},
			{{-.5f, -.5f, -.5f}, {-.5f, -.5f, .5f}, {-.5f, .5f, .5f}, {-.5f, .5f, -.5f}},
			{{-.5f, .5f, .5f}, {.5f, .5f, .5f}, {.5f, .5f, -.5f}, {-.5f, .5f, -.5f}},
			{{-.5f, -.5f, -.5f}, {.5f, -.5f, -.5f}, {.5f, -.5f, .5f}, {-.5f, -.5f, .5f}}
		};
		for (unsigned face = 0; face < 6; ++face) {
			for (const auto& position : faces[face]) {
				vertices += vector(position[0], position[1], position[2]);
				// Normal, color and texture fields exercise the 36-byte stride.
				vertices += Bytes(24, '\0');
			}
			for (unsigned corner : {0u, 1u, 2u, 0u, 2u, 3u})
				indices.push_back(face * 4 + corner);
		}
	}
};

Bytes geometry(const Geometry& data, bool legacy) {
	Bytes indices = legacy ? Bytes() : little32(data.indexCount);
	for (uint32_t index : data.indices)
		indices += little32(index).substr(0, legacy ? 4 : data.modernIndexBytes);
	indices.resize(indices.size() - data.indexBytesToDrop);

	return form("VTXA", form(legacy ? "0002" : "0003",
		chunk("INFO", little32(0x110d) + little32(data.vertexCount)) +
		chunk("DATA", data.vertices))) + chunk("INDX", indices);
}

Bytes legacyBounds() {
	return form("EXBX", form("0000",
		chunk("BOX ", vector(.5f, .5f, .5f) + vector(-.5f, -.5f, -.5f)) +
		chunk("CNTR", vector(0, 0, 0)) + chunk("RADI", real(std::sqrt(.75f)))));
}

Bytes modernBounds() {
	return form("EXBX", form("0001",
		form("EXSP", form("0001", chunk("SPHR", vector(0, 0, 0) + real(std::sqrt(.75f))))) +
		chunk("BOX ", vector(.5f, .5f, .5f) + vector(-.5f, -.5f, -.5f))));
}

Bytes appearance(bool legacy, const Geometry& data = Geometry()) {
	Bytes shader = form("0001", chunk("NAME", Bytes("shader/test.sht\0", 16)) +
		chunk("INFO", little32(1)) + form("0001", chunk("INFO", little32(9)) + geometry(data, legacy)));
	Bytes shaders = form("SPS ", form(legacy ? "0000" : "0001", chunk("CNT ", little32(1)) + shader));
	if (legacy) {
		return form("MESH", form("0003", shaders + chunk("CNTR", vector(0, 0, 0)) +
			chunk("RADI", real(std::sqrt(.75f)))) + legacyBounds());
	}
	Bytes base = form("APPR", form("0003", modernBounds() + form("NULL", "") +
		form("HPTS", "") + form("FLOR", chunk("DATA", Bytes(1, '\0')))));
	return form("MESH", form("0005", base + shaders));
}

bool parse(IffStream& stream, Bytes& bytes) {
	return stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "mesh-regression.msh");
}

void expectCubeBounds(const AABB& bounds) {
	EXPECT_FLOAT_EQ(-.5f, bounds.getXMin());
	EXPECT_FLOAT_EQ(-.5f, bounds.getYMin());
	EXPECT_FLOAT_EQ(-.5f, bounds.getZMin());
	EXPECT_FLOAT_EQ(.5f, bounds.getXMax());
	EXPECT_FLOAT_EQ(.5f, bounds.getYMax());
	EXPECT_FLOAT_EQ(.5f, bounds.getZMax());
}

void expectCube(Bytes bytes) {
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	MeshAppearanceTemplate mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream));
	ASSERT_EQ(1, mesh.getMeshes().size());
	const MeshData* data = mesh.getMeshes().get(0);
	ASSERT_EQ(24, data->getVerts()->size());
	ASSERT_EQ(12, data->getTriangles()->size());
	EXPECT_EQ(0, data->getTriangles()->get(0).getVerts()[0]);
	EXPECT_EQ(1, data->getTriangles()->get(0).getVerts()[1]);
	EXPECT_EQ(2, data->getTriangles()->get(0).getVerts()[2]);
	EXPECT_EQ(23, data->getTriangles()->get(11).getVerts()[2]);
	expectCubeBounds(data->buildAABB());
	ASSERT_NE(nullptr, mesh.getBoundingVolume());
	EXPECT_TRUE(mesh.getBoundingVolume()->isBoundingBox());
	expectCubeBounds(mesh.getBoundingVolume()->getBoundingBox());
	EXPECT_NEAR(std::sqrt(.75f), mesh.getBoundingVolume()->getBoundingSphere().getRadius(), 1e-6);
	ASSERT_NE(nullptr, mesh.getAABBTree());
	ASSERT_NE(nullptr, mesh.getBoundingSphere());
	EXPECT_NEAR(std::sqrt(.75f), mesh.getBoundingSphere()->getRadius(), 1e-6);
	EXPECT_TRUE(mesh.testCollide(Sphere(Vector3(.5f, 0, 0), .1f)));
	EXPECT_FALSE(mesh.testCollide(Sphere(Vector3(2, 2, 2), .1f)));
}

void expectRejected(const Geometry& data, bool legacy) {
	Bytes bytes = appearance(legacy, data);
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	MeshAppearanceTemplate mesh;
	EXPECT_THROW(mesh.readObject(&stream), Exception);
	EXPECT_EQ(nullptr, mesh.getAABBTree());
}

} // namespace

TEST(MeshAppearanceTest, LegacyCubeHasGeometryBoundsAndCollision) {
	expectCube(appearance(true));
}

TEST(MeshAppearanceTest, ModernCubeHasGeometryBoundsAndCollision) {
	expectCube(appearance(false));
}

TEST(MeshAppearanceTest, Modern32BitCubeHasGeometryBoundsAndCollision) {
	Geometry data;
	data.modernIndexBytes = 4;
	expectCube(appearance(false, data));
}

TEST(MeshAppearanceTest, OriginalMediaLegacyCubeWhenAvailable) {
	std::ifstream file("appearance/defaultappearance.msh", std::ios::binary);
	if (!file.is_open()) {
		GTEST_SKIP() << "Original-media defaultappearance.msh is not present in the bin folder";
	}
	Bytes bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	ASSERT_GE(bytes.size(), 24u);
	ASSERT_EQ("FORM", bytes.substr(0, 4));
	ASSERT_EQ("MESH", bytes.substr(8, 4));
	ASSERT_EQ("FORM", bytes.substr(12, 4));
	ASSERT_EQ("0003", bytes.substr(20, 4));
	expectCube(bytes);
}

TEST(MeshAppearanceTest, CapturedModern32BitMeshWhenAvailable) {
	std::ifstream file("log/codex-modern-indices32.msh", std::ios::binary);
	if (!file.is_open()) {
		GTEST_SKIP() << "Captured modern 32-bit mesh is not present in the bin/log folder";
	}
	Bytes bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	MeshAppearanceTemplate mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream));
	ASSERT_EQ(13, mesh.getMeshes().size());
	const MeshData* first = mesh.getMeshes().get(0);
	ASSERT_EQ(254, first->getVerts()->size());
	ASSERT_EQ(198, first->getTriangles()->size());
	const int* indices = first->getTriangles()->get(0).getVerts();
	EXPECT_EQ(38, indices[0]);
	EXPECT_EQ(39, indices[1]);
	EXPECT_EQ(40, indices[2]);
	int triangles = 0;
	for (int i = 0; i < mesh.getMeshes().size(); ++i)
		triangles += mesh.getMeshes().get(i)->getTriangles()->size();
	EXPECT_EQ(758, triangles);
	EXPECT_NE(nullptr, mesh.getAABBTree());
}

TEST(MeshAppearanceTest, EmptyMeshHasNoCollision) {
	MeshAppearanceTemplate mesh;
	Sphere sphere(Vector3(0, 0, 0), 1);
	Ray ray(Vector3(0, 0, 0), Vector3(1, 0, 0));
	float distance = -1;
	Triangle* triangle = nullptr;
	SortedVector<IntersectionResult> intersections;
	EXPECT_FALSE(mesh.testCollide(sphere));
	EXPECT_FALSE(mesh.testCollide(0, 0, 0, 1));
	EXPECT_FALSE(mesh.intersects(ray, 10, distance, triangle));
	EXPECT_EQ(0, mesh.intersects(ray, 10, intersections));
	EXPECT_EQ(0, intersections.size());
}

TEST(MeshAppearanceTest, EmptyDetailHasNoCollision) {
	DetailAppearanceTemplate detail;
	Sphere sphere(Vector3(0, 0, 0), 1);
	Ray ray(Vector3(0, 0, 0), Vector3(1, 0, 0));
	float distance = -1;
	Triangle* triangle = nullptr;
	SortedVector<IntersectionResult> intersections;
	EXPECT_FALSE(detail.testCollide(sphere));
	EXPECT_FALSE(detail.intersects(ray, 10, distance, triangle));
	EXPECT_EQ(0, detail.intersects(ray, 10, intersections));
	EXPECT_EQ(0, intersections.size());
}

TEST(MeshAppearanceTest, RejectsZeroOrNegativeVertexCount) {
	for (bool legacy : {false, true}) {
		Geometry data;
		data.vertexCount = 0;
		expectRejected(data, legacy);
		data.vertexCount = -1;
		expectRejected(data, legacy);
	}
}

TEST(MeshAppearanceTest, RejectsTruncatedOrUndersizedVertexRecords) {
	for (bool legacy : {false, true}) {
		Geometry data;
		data.vertices.pop_back();
		expectRejected(data, legacy);
		data.vertices.resize(data.vertexCount * 8);
		expectRejected(data, legacy);
	}
}

TEST(MeshAppearanceTest, RejectsOutOfRangeVertexIndices) {
	for (bool legacy : {false, true}) {
		for (int indexBytes : {2, 4}) {
			Geometry data;
			data.modernIndexBytes = indexBytes;
			data.indices[0] = data.vertexCount;
			expectRejected(data, legacy);
			data.indices[0] = UINT32_MAX;
			expectRejected(data, legacy);
		}
	}
}

TEST(MeshAppearanceTest, RejectsIncompleteIndexRecords) {
	for (bool legacy : {false, true}) {
		for (int indexBytes : {2, 4}) {
			Geometry data;
			data.modernIndexBytes = indexBytes;
			data.indexBytesToDrop = 1;
			expectRejected(data, legacy);
		}
	}
	Geometry data;
	data.indices.pop_back();
	expectRejected(data, true);
}

TEST(MeshAppearanceTest, RejectsInvalidModernIndexCounts) {
	for (int indexBytes : {2, 4}) {
		Geometry data;
		data.modernIndexBytes = indexBytes;
		for (int count : {-1, 0, 35, 39}) {
			data.indexCount = count;
			expectRejected(data, false);
		}
	}
}

TEST(MeshAppearanceTest, RejectsUnsupportedModernIndexWidths) {
	for (int indexBytes : {1, 3}) {
		Geometry data;
		data.modernIndexBytes = indexBytes;
		expectRejected(data, false);
	}
}

TEST(MeshAppearanceTest, RejectsModernIndexCountsWithIncompleteTriangles) {
	for (int indexBytes : {2, 4}) {
		Geometry data;
		data.modernIndexBytes = indexBytes;
		data.indices.pop_back();
		data.indexCount = data.indices.size();
		expectRejected(data, false);
	}
}

TEST(MeshAppearanceTest, ModernEmptyIndexDataHasNoCollision) {
	Geometry data;
	data.indices.clear();
	data.indexCount = 0;
	Bytes bytes = appearance(false, data);
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	MeshAppearanceTemplate mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream));
	ASSERT_EQ(1, mesh.getMeshes().size());
	EXPECT_EQ(0, mesh.getMeshes().get(0)->getTriangles()->size());
	EXPECT_EQ(nullptr, mesh.getAABBTree());
	EXPECT_FALSE(mesh.testCollide(Sphere(Vector3(0, 0, 0), 1)));
}

TEST(MeshAppearanceTest, ModernIndicesAreUnsigned16Bit) {
	Geometry data;
	data.vertexCount = 32771;
	data.vertices = Bytes(data.vertexCount * 12, '\0');
	data.indices = {32768, 32769, 32770};
	data.indexCount = 3;
	Bytes bytes = form("TEST", geometry(data, false));
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	stream.openForm('TEST');
	MeshData mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream));
	ASSERT_EQ(1, mesh.getTriangles()->size());
	const int* indices = mesh.getTriangles()->get(0).getVerts();
	EXPECT_EQ(32768, indices[0]);
	EXPECT_EQ(32769, indices[1]);
	EXPECT_EQ(32770, indices[2]);
	stream.closeForm('TEST');
}

TEST(MeshAppearanceTest, LegacyIndicesRetainAll32Bits) {
	Geometry data;
	data.vertexCount = 65539;
	data.vertices = Bytes(data.vertexCount * 12, '\0');
	data.indices = {65536, 65537, 65538};
	Bytes bytes = form("TEST", geometry(data, true));
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	stream.openForm('TEST');
	MeshData mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream, true));
	ASSERT_EQ(1, mesh.getTriangles()->size());
	const int* indices = mesh.getTriangles()->get(0).getVerts();
	EXPECT_EQ(65536, indices[0]);
	EXPECT_EQ(65537, indices[1]);
	EXPECT_EQ(65538, indices[2]);
	stream.closeForm('TEST');
}

TEST(MeshAppearanceTest, ModernIndicesRetainAll32Bits) {
	Geometry data;
	data.vertexCount = 65539;
	data.vertices = Bytes(data.vertexCount * 12, '\0');
	data.indices = {65536, 65537, 65538};
	data.indexCount = 3;
	data.modernIndexBytes = 4;
	Bytes bytes = form("TEST", geometry(data, false));
	IffStream stream;
	ASSERT_TRUE(parse(stream, bytes));
	stream.openForm('TEST');
	MeshData mesh;
	ASSERT_NO_THROW(mesh.readObject(&stream));
	ASSERT_EQ(1, mesh.getTriangles()->size());
	const int* indices = mesh.getTriangles()->get(0).getVerts();
	EXPECT_EQ(65536, indices[0]);
	EXPECT_EQ(65537, indices[1]);
	EXPECT_EQ(65538, indices[2]);
	stream.closeForm('TEST');
}

TEST(MeshAppearanceTest, RejectsTruncatedLegacyBoundingData) {
	const Bytes box = chunk("BOX ", vector(.5f, .5f, .5f) + vector(-.5f, -.5f, -.5f));
	const Bytes center = chunk("CNTR", vector(0, 0, 0));
	const Bytes radius = chunk("RADI", real(std::sqrt(.75f)));
	const Bytes malformed[] = {
		chunk("BOX ", Bytes(20, '\0')) + center + radius,
		box + chunk("CNTR", Bytes(8, '\0')) + radius,
		box + center + chunk("RADI", ""),
		box + center,
		box + radius
	};
	for (const auto& contents : malformed) {
		Bytes bytes = form("EXBX", form("0000", contents));
		IffStream stream;
		ASSERT_TRUE(parse(stream, bytes));
		BoxVolume bounds;
		EXPECT_THROW(bounds.read(&stream), Exception);
	}
}
