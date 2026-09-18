#include "gtest/gtest.h"

#include "terrain/layer/boundaries/BoundaryPolygon.h"
#include "terrain/layer/boundaries/BoundaryRectangle.h"

#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <string>

namespace {

using Bytes = std::string;
constexpr const char* shaderName = "shader/boundary_test.sht";

// Independent IFF fixtures: big-endian chunk lengths, little-endian payloads,
// and null-terminated strings. These tests do not load configuration or TREs.
Bytes little32(uint32_t value) {
	Bytes result;
	for (unsigned shift = 0; shift < 32; shift += 8)
		result.push_back(static_cast<char>(value >> shift));
	return result;
}

Bytes real32(float value) {
	uint32_t bits;
	static_assert(sizeof(bits) == sizeof(value), "IFF floats must be 32 bits");
	std::memcpy(&bits, &value, sizeof(bits));
	return little32(bits);
}

Bytes terminated(const char* value) {
	return Bytes(value) + '\0';
}

Bytes chunk(const char* tag, const Bytes& payload) {
	Bytes result(tag, 4);
	for (int shift = 24; shift >= 0; shift -= 8)
		result.push_back(static_cast<char>(payload.size() >> shift));
	return result + payload;
}

Bytes form(const char* tag, const Bytes& contents) {
	return chunk("FORM", Bytes(tag, 4) + contents);
}

Bytes boundaryForm(const char* tag, const char* version, const Bytes& payload, bool enabled = true) {
	Bytes header = form("IHDR", form("0001", chunk("DATA", little32(enabled) + terminated("Boundary version fixture"))));
	return form(tag, form(version, header + chunk("DATA", payload)));
}

Bytes polygonData(const char* version, bool waterEnabled = true) {
	Bytes data = little32(4);
	for (float coordinate : {-10.0f, -20.0f, 30.0f, -20.0f, 30.0f, 40.0f, -10.0f, 40.0f})
		data += real32(coordinate);
	data += little32(3) + real32(4.0f); // Feathering type and distance.
	data += little32(waterEnabled) + real32(123.25f) + real32(16.5f);
	if (std::strcmp(version, "0005") != 0)
		data += little32(1); // Reserved in 0006, water type in 0007.
	data += terminated(shaderName);
	return data;
}

Bytes rectangleData(const char* version, bool waterEnabled = true, float feathering = 0.2f) {
	// Deliberately reversed corners exercise normalization after parsing.
	Bytes data = real32(30.0f) + real32(40.0f) + real32(-10.0f) + real32(-20.0f);
	data += little32(3) + real32(feathering);
	if (std::strcmp(version, "0002") != 0) {
		data += little32(waterEnabled) + little32(1); // Local/global rendering metadata.
		data += real32(123.25f) + real32(16.5f) + terminated(shaderName);
	}
	if (std::strcmp(version, "0004") == 0)
		data += little32(1); // Water type follows the string for rectangles.
	return data;
}

void readBoundary(Boundary& boundary, const Bytes& contents) {
	Bytes bytes = form("TEST", contents + form("NEXT", ""));
	IffStream stream;
	ASSERT_TRUE(stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "boundary-version-test.iff"));
	stream.openForm('TEST');
	ASSERT_NO_THROW(boundary.readObject(&stream));
	// A boundary must consume exactly its own form, preserving the next rule.
	ASSERT_EQ(static_cast<uint32>('NEXT'), stream.getNextFormType());
	stream.openForm('NEXT');
	stream.closeForm('NEXT');
	EXPECT_EQ(0, stream.getRemainingSubChunksNumber());
	stream.closeForm('TEST');
}

void expectGeometry(const Boundary& boundary) {
	EXPECT_TRUE(boundary.isEnabled());
	EXPECT_EQ(3, boundary.getFeatheringType());
	EXPECT_FLOAT_EQ(-10.0f, boundary.getMinX());
	EXPECT_FLOAT_EQ(-20.0f, boundary.getMinY());
	EXPECT_FLOAT_EQ(30.0f, boundary.getMaxX());
	EXPECT_FLOAT_EQ(40.0f, boundary.getMaxY());
	EXPECT_TRUE(boundary.containsPoint(10.0f, 10.0f));
	EXPECT_FALSE(boundary.containsPoint(-11.0f, 10.0f));
	EXPECT_FALSE(boundary.containsPoint(10.0f, 41.0f));
	EXPECT_FLOAT_EQ(1.0f, boundary.process(10.0f, 10.0f));
	EXPECT_FLOAT_EQ(0.5f, boundary.process(-8.0f, 10.0f));
	EXPECT_FLOAT_EQ(0.0f, boundary.process(-11.0f, 10.0f));
	EXPECT_FLOAT_EQ(0.0f, boundary.process(10.0f, 41.0f));
}

void expectWaterRegistration(Boundary& boundary, bool enabled) {
	ProceduralTerrainAppearance terrain;
	boundary.executeRule(&terrain);
	float height = -999.0f;
	EXPECT_EQ(enabled, terrain.getWater(10.0f, 10.0f, height));
	EXPECT_FLOAT_EQ(enabled ? 123.25f : -999.0f, height);
	EXPECT_FALSE(terrain.getWater(-11.0f, 10.0f, height));
	EXPECT_FALSE(terrain.getWater(10.0f, 41.0f, height));
}

} // namespace

TEST(TerrainBoundaryVersionTest, PolygonVersionsPreserveVerticesAndFeathering) {
	for (const char* version : {"0005", "0006", "0007"}) {
		SCOPED_TRACE(version);
		BoundaryPolygon boundary;
		ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BPOL", version, polygonData(version))));
		expectGeometry(boundary);
		const auto& vertices = boundary.getVertices();
		ASSERT_EQ(4, vertices.size());
		EXPECT_FLOAT_EQ(-10.0f, vertices.get(0)->getX());
		EXPECT_FLOAT_EQ(-20.0f, vertices.get(0)->getY());
		EXPECT_FLOAT_EQ(30.0f, vertices.get(2)->getX());
		EXPECT_FLOAT_EQ(40.0f, vertices.get(2)->getY());
	}
}

TEST(TerrainBoundaryVersionTest, PolygonVersionsPreserveDisabledInformationHeader) {
	for (const char* version : {"0005", "0006", "0007"}) {
		SCOPED_TRACE(version);
		BoundaryPolygon boundary;
		ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BPOL", version, polygonData(version), false)));
		EXPECT_FALSE(boundary.isEnabled());
		EXPECT_FLOAT_EQ(-10.0f, boundary.getMinX());
		EXPECT_FLOAT_EQ(40.0f, boundary.getMaxY());
	}
}

TEST(TerrainBoundaryVersionTest, PolygonVersionsRegisterOnlyEnabledLocalWater) {
	for (const char* version : {"0005", "0006", "0007"}) {
		SCOPED_TRACE(version);
		for (bool enabled : {false, true}) {
			SCOPED_TRACE(enabled);
			BoundaryPolygon boundary;
			ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BPOL", version, polygonData(version, enabled))));
			EXPECT_FLOAT_EQ(123.25f, boundary.getLocalWaterTableHeight());
			expectWaterRegistration(boundary, enabled);
		}
	}
}

TEST(TerrainBoundaryVersionTest, RectangleVersionsNormalizeBoundsAndPreserveFeathering) {
	for (const char* version : {"0002", "0003", "0004"}) {
		SCOPED_TRACE(version);
		BoundaryRectangle boundary;
		ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BREC", version, rectangleData(version))));
		expectGeometry(boundary);
	}
}

TEST(TerrainBoundaryVersionTest, RectangleVersionsPreserveDisabledInformationHeader) {
	for (const char* version : {"0002", "0003", "0004"}) {
		SCOPED_TRACE(version);
		BoundaryRectangle boundary;
		ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BREC", version, rectangleData(version), false)));
		EXPECT_FALSE(boundary.isEnabled());
		EXPECT_FLOAT_EQ(-10.0f, boundary.getMinX());
		EXPECT_FLOAT_EQ(40.0f, boundary.getMaxY());
	}
}

TEST(TerrainBoundaryVersionTest, RectangleVersionsClampFeatheringToUnitInterval) {
	for (const char* version : {"0002", "0003", "0004"}) {
		SCOPED_TRACE(version);
		BoundaryRectangle negative;
		ASSERT_NO_FATAL_FAILURE(readBoundary(negative, boundaryForm("BREC", version, rectangleData(version, false, -0.5f))));
		EXPECT_FLOAT_EQ(1.0f, negative.process(-8.0f, 10.0f));
		BoundaryRectangle excessive;
		ASSERT_NO_FATAL_FAILURE(readBoundary(excessive, boundaryForm("BREC", version, rectangleData(version, false, 2.0f))));
		EXPECT_FLOAT_EQ(0.1f, excessive.process(-8.0f, 10.0f));
		EXPECT_FLOAT_EQ(1.0f, excessive.process(10.0f, 10.0f));
	}
}

TEST(TerrainBoundaryVersionTest, RectangleWaterVersionsRegisterOnlyEnabledLocalWater) {
	for (const char* version : {"0003", "0004"}) {
		SCOPED_TRACE(version);
		for (bool enabled : {false, true}) {
			SCOPED_TRACE(enabled);
			BoundaryRectangle boundary;
			ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BREC", version, rectangleData(version, enabled))));
			EXPECT_FLOAT_EQ(123.25f, boundary.getLocalWaterTableHeight());
			expectWaterRegistration(boundary, enabled);
		}
	}
}

TEST(TerrainBoundaryVersionTest, RectangleVersionTwoHasNoLocalWater) {
	BoundaryRectangle boundary;
	ASSERT_NO_FATAL_FAILURE(readBoundary(boundary, boundaryForm("BREC", "0002", rectangleData("0002"))));
	EXPECT_FLOAT_EQ(0.0f, boundary.getLocalWaterTableHeight());
	expectWaterRegistration(boundary, false);
}

TEST(TerrainBoundaryVersionTest, RectangleVersionFourRejectsTruncatedWaterType) {
	Bytes payload = rectangleData("0004");
	payload.pop_back();
	Bytes bytes = boundaryForm("BREC", "0004", payload);
	IffStream stream;
	ASSERT_TRUE(stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "truncated-boundary-version-test.iff"));
	BoundaryRectangle boundary;
	EXPECT_ANY_THROW(boundary.readObject(&stream));
}

TEST(TerrainBoundaryVersionTest, NewPolygonVersionsRejectTruncatedMetadataBeforeShader) {
	for (const char* version : {"0006", "0007"}) {
		SCOPED_TRACE(version);
		Bytes payload = polygonData(version);
		// Remove the shader and the final byte of the preceding integer. A
		// legacy reader would mistake its remaining bytes for a short string.
		payload.resize(payload.size() - terminated(shaderName).size() - 1);
		Bytes bytes = boundaryForm("BPOL", version, payload);
		IffStream stream;
		ASSERT_TRUE(stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "truncated-boundary-version-test.iff"));
		BoundaryPolygon boundary;
		EXPECT_ANY_THROW(boundary.readObject(&stream));
	}
}
