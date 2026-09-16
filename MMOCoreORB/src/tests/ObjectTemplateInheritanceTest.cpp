#include "gtest/gtest.h"

#include "engine/lua/Lua.h"
#include "server/zone/objects/scene/SceneObjectType.h"
#include "templates/SharedTangibleObjectTemplate.h"
#include "templates/resource/ResourceSpawnTemplate.h"

#include <cstdint>
#include <initializer_list>
#include <string>

namespace {

using Bytes = std::string;

// Independent IFF fixtures: lengths are big endian, numeric payloads are
// little endian, and strings are null terminated. No TRE files are needed.
Bytes little32(uint32_t value) {
	Bytes result;
	for (unsigned shift = 0; shift < 32; shift += 8)
		result.push_back(static_cast<char>(value >> shift));
	return result;
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

Bytes stringValue(const char* value) {
	return Bytes(1, '\1') + terminated(value);
}

Bytes integerValue(uint32_t value) {
	return Bytes("\1\x20", 2) + little32(value);
}

Bytes booleanValue(bool value) {
	return Bytes(1, '\1') + static_cast<char>(value);
}

Bytes stringIdValue(const char* table, const char* entry) {
	return Bytes(1, '\1') + stringValue(table) + stringValue(entry);
}

Bytes field(const char* name, const Bytes& value) {
	return chunk("XXXX", terminated(name) + value);
}

Bytes version(std::initializer_list<Bytes> fields) {
	Bytes contents = chunk("PCNT", little32(fields.size()));
	for (const Bytes& value : fields)
		contents += value;
	return form("0000", contents);
}

Bytes objectBase(uint32_t gameObjectType = SceneObjectType::RESOURCECONTAINER) {
	return form("SHOT", version({
		field("appearanceFilename", stringValue("appearance/ancestor-test.apt")),
		field("objectName", stringIdValue("ancestor_test", "inherited_name")),
		field("containerVolumeLimit", integerValue(27)),
		field("collisionActionFlags", integerValue(51)),
		field("gameObjectType", integerValue(gameObjectType))
	}));
}

Bytes tangibleBase(uint32_t gameObjectType = SceneObjectType::RESOURCECONTAINER) {
	return form("STOT", version({field("targetable", booleanValue(true))}) + objectBase(gameObjectType));
}

Bytes ancestor(const char* tag, const Bytes& base, const Bytes& derv = Bytes()) {
	return form(tag, derv + version({
		// These fields deliberately resemble SHOT data. They belong to a
		// specialized version block, so must never become common metadata.
		field("appearanceFilename", stringValue("appearance/wrong-wrapper.apt")),
		field("containerVolumeLimit", integerValue(999)),
		field("gameObjectType", integerValue(999)),
		field("targetable", booleanValue(false))
	}) + base);
}

Bytes derivedFrom(const char* path) {
	return form("DERV", chunk("XXXX", terminated(path)));
}

class ResourceSpawnProbe : public ResourceSpawnTemplate {
public:
	ResourceSpawnProbe() {
		templateType = RESOURCESPAWN;
	}

	int templateKind() const {
		return templateType;
	}
};

class TangibleProbe : public SharedTangibleObjectTemplate {
public:
	TangibleProbe() {
		templateType = STOT;
	}

	int templateKind() const {
		return templateType;
	}
};

// Attach to the local template instance only. No global logger or callback
// settings are changed, and the callback is removed before its target dies.
class TemplateLog {
	SharedObjectTemplate& object;
	std::string messages;

public:
	explicit TemplateLog(SharedObjectTemplate& value) : object(value) {
		object.setLoggerCallback([this](Logger::LogLevel, const char* message) -> int {
			messages += message;
			return Logger::SUCCESS;
		});
	}

	~TemplateLog() {
		object.clearLoggerCallback();
	}

	bool contains(const char* message) const {
		return messages.find(message) != std::string::npos;
	}
};

void readTemplate(SharedObjectTemplate& object, const Bytes& contents) {
	Bytes bytes = form("TEST", contents + form("NEXT", ""));
	IffStream stream;
	ASSERT_TRUE(stream.parseChunks(reinterpret_cast<byte*>(&bytes[0]), bytes.size(), "ancestor-test.iff"));
	stream.openForm('TEST');
	ASSERT_NO_THROW(object.readObject(&stream));
	// Reading one template must leave its following sibling untouched.
	ASSERT_EQ(static_cast<uint32>('NEXT'), stream.getNextFormType());
	stream.openForm('NEXT');
	stream.closeForm('NEXT');
	EXPECT_EQ(0, stream.getRemainingSubChunksNumber());
	stream.closeForm('TEST');
}

void expectObjectMetadata(const SharedObjectTemplate& object, uint32_t gameObjectType) {
	EXPECT_EQ(String("appearance/ancestor-test.apt"), object.getAppearanceFilename());
	EXPECT_EQ(String("@ancestor_test:inherited_name"), object.getObjectName());
	EXPECT_EQ(27, object.getContainerVolumeLimit());
	EXPECT_EQ(51, object.getCollisionActionFlags());
	EXPECT_EQ(static_cast<int>(gameObjectType), object.getGameObjectType());
	EXPECT_EQ(static_cast<int>(gameObjectType), object.getClientGameObjectType());
}

} // namespace

TEST(ObjectTemplateInheritanceTest, ResourceSpawnReadsContainerAndTangibleAncestors) {
	ResourceSpawnProbe object;
	TemplateLog log(object);
	readTemplate(object, ancestor("RCCT", tangibleBase()));
	expectObjectMetadata(object, SceneObjectType::RESOURCECONTAINER);
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::RESOURCESPAWN), object.templateKind());
	EXPECT_FALSE(log.contains("expecting SHOT"));
}

TEST(ObjectTemplateInheritanceTest, ResourceSpawnRetainsFinalLuaRuntimeOverride) {
	ResourceSpawnProbe object;
	readTemplate(object, ancestor("RCCT", tangibleBase()));
	expectObjectMetadata(object, SceneObjectType::RESOURCECONTAINER);

	// Mirror TemplateManager's IFF-then-Lua order with an isolated table.
	// Do not initialize the global template registry or load server scripts.
	Lua lua;
	lua.init();
	lua_State* state = lua.getLuaState();
	lua_newtable(state);
	lua_pushinteger(state, SharedObjectTemplate::RESOURCESPAWN);
	lua_setfield(state, -2, "templateType");
	lua_pushinteger(state, SceneObjectType::RESOURCESPAWN);
	lua_setfield(state, -2, "gameObjectType");
	lua_pushstring(state, "object/resource_container/shared_simple.iff");
	lua_setfield(state, -2, "clientTemplateFileName");
	LuaObject data(state);
	ASSERT_NO_THROW(object.readObject(&data));
	data.pop();

	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::RESOURCESPAWN), object.templateKind());
	EXPECT_EQ(static_cast<int>(SceneObjectType::RESOURCESPAWN), object.getGameObjectType());
	EXPECT_EQ(static_cast<int>(SceneObjectType::RESOURCECONTAINER), object.getClientGameObjectType());
	EXPECT_EQ(String("appearance/ancestor-test.apt"), object.getAppearanceFilename());
	EXPECT_EQ(String("@ancestor_test:inherited_name"), object.getObjectName());
}

TEST(ObjectTemplateInheritanceTest, BattlefieldStationReadsShipAncestorAsTangible) {
	TangibleProbe object;
	TemplateLog log(object);
	readTemplate(object, ancestor("SSHP", tangibleBase(SceneObjectType::SPACEOBJECT)));
	expectObjectMetadata(object, SceneObjectType::SPACEOBJECT);
	EXPECT_TRUE(object.getTargetable());
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::STOT), object.templateKind());
	EXPECT_FALSE(log.contains("expecting SHOT"));
}

TEST(ObjectTemplateInheritanceTest, WrapperSpecificPropertiesAreIgnoredWithoutAnObjectBase) {
	ResourceSpawnProbe resource;
	resource.setObjectName("@ancestor_test:resource_sentinel");
	readTemplate(resource, ancestor("RCCT", ancestor("STOT", "")));
	EXPECT_EQ(String("@ancestor_test:resource_sentinel"), resource.getObjectName());
	EXPECT_TRUE(resource.getAppearanceFilename().isEmpty());
	EXPECT_EQ(0, resource.getContainerVolumeLimit());
	EXPECT_EQ(0, resource.getGameObjectType());
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::RESOURCESPAWN), resource.templateKind());

	TangibleProbe station;
	readTemplate(station, ancestor("SSHP", ""));
	EXPECT_TRUE(station.getAppearanceFilename().isEmpty());
	EXPECT_EQ(0, station.getContainerVolumeLimit());
	EXPECT_EQ(0, station.getGameObjectType());
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::STOT), station.templateKind());
}

TEST(ObjectTemplateInheritanceTest, AlreadyLoadedDervStillAdvancesToResourceAncestors) {
	const char* path = "object/test/shared_already_loaded_resource.iff";
	ResourceSpawnProbe object;
	object.addDerivedFile(path);
	readTemplate(object, ancestor("RCCT", tangibleBase(), derivedFrom(path)));
	expectObjectMetadata(object, SceneObjectType::RESOURCECONTAINER);
}

TEST(ObjectTemplateInheritanceTest, AlreadyLoadedDervStillAdvancesToShipAncestors) {
	const char* path = "object/test/shared_already_loaded_ship.iff";
	TangibleProbe object;
	object.addDerivedFile(path);
	readTemplate(object, ancestor("SSHP", tangibleBase(SceneObjectType::SPACEOBJECT), derivedFrom(path)));
	expectObjectMetadata(object, SceneObjectType::SPACEOBJECT);
	EXPECT_TRUE(object.getTargetable());
}

TEST(ObjectTemplateInheritanceTest, UnknownWrapperStillWarnsAndSkipsItsContents) {
	ResourceSpawnProbe object;
	TemplateLog log(object);
	readTemplate(object, ancestor("UNKN", objectBase()));
	EXPECT_TRUE(log.contains("expecting SHOT got UNKN"));
	EXPECT_TRUE(object.getAppearanceFilename().isEmpty());
	EXPECT_EQ(0, object.getContainerVolumeLimit());
}

TEST(ObjectTemplateInheritanceTest, UnknownNestedWrapperWarnsAndPreservesFollowingBase) {
	TangibleProbe object;
	TemplateLog log(object);
	const Bytes unknown = ancestor("UNKN", objectBase(999));
	readTemplate(object, ancestor("SSHP", unknown + tangibleBase(SceneObjectType::SPACEOBJECT)));
	EXPECT_TRUE(log.contains("expecting SHOT got UNKN"));
	expectObjectMetadata(object, SceneObjectType::SPACEOBJECT);
	EXPECT_TRUE(object.getTargetable());
}

TEST(ObjectTemplateInheritanceTest, ExistingDirectObjectAndTangibleParsingIsUnchanged) {
	ResourceSpawnProbe resource;
	readTemplate(resource, objectBase());
	expectObjectMetadata(resource, SceneObjectType::RESOURCECONTAINER);
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::RESOURCESPAWN), resource.templateKind());

	TangibleProbe tangible;
	readTemplate(tangible, tangibleBase(SceneObjectType::SPACEOBJECT));
	expectObjectMetadata(tangible, SceneObjectType::SPACEOBJECT);
	EXPECT_TRUE(tangible.getTargetable());
	EXPECT_EQ(static_cast<int>(SharedObjectTemplate::STOT), tangible.templateKind());
}
