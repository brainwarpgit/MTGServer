/*
 * ZoneTest.cpp
 *
 *  Created on: 09/09/2013
 *      Author: victor
 */

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "server/db/MySqlDatabase.h"
#include "server/db/ServerDatabase.h"
#include "server/zone/GroundZone.h"
#include "server/zone/ZoneProcessServer.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/scene/DatabaseZoneInsertion.h"
#include "server/zone/objects/area/ActiveArea.h"
#include "conf/ConfigManager.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/login/objects/GalaxyList.h"

using ::testing::_;
using ::testing::Return;
using ::testing::AnyNumber;
using ::testing::TypedEq;
using ::testing::An;

class ZoneTest : public ::testing::Test {
protected:
#ifndef WITH_SWGREALMS_API
	ServerDatabase* database = nullptr;
#endif // !WITH_SWGREALMS_API
	Reference<ZoneServer*> zoneServer;
	Reference<GroundZone*> groundZone;
	Reference<ZoneProcessServer*> processServer;
	AtomicLong nextObjectId;
	Reference<PlayerManager*> playerManager;
public:
	ZoneTest() {
		// Perform creation setup here.
		nextObjectId = 1;
	}

	~ZoneTest() {
		// Clean up.
	}

	void setDefaultComponents(SceneObject* object) {
		object->setContainerComponent("ContainerComponent");
		object->setGroundZoneComponent("GroundZoneComponent");
	}

	Reference<SceneObject*> createSceneObject() {
		Reference<SceneObject*> object = new SceneObject();
		setDefaultComponents(object);
		object->_setObjectID(nextObjectId.increment());
		object->initializeContainerObjectsMap();

		return object;
	}

	Reference<TangibleObject*> createTangibleObject() {
		Reference<TangibleObject*> object = new TangibleObject();
		setDefaultComponents(object);
		object->_setObjectID(nextObjectId.increment());
		object->initializeContainerObjectsMap();

		return object;
	}

	Reference<ActiveArea*> createActiveArea(bool mock = false) {
		Reference<ActiveArea*> activeArea;

		if (mock) {
			activeArea = new MockActiveArea();
		} else {
			activeArea = new ActiveArea();
		}
		setDefaultComponents(activeArea);
		activeArea->_setObjectID(nextObjectId.increment());
		activeArea->initializeContainerObjectsMap();

		return activeArea;
	}

	void SetUp() {
		// Perform setup of common constructs here.
		ConfigManager::instance()->loadConfigData();
		ConfigManager::instance()->setProgressMonitors(false);
		auto configManager = ConfigManager::instance();

#ifndef WITH_SWGREALMS_API
		database = new ServerDatabase(configManager);
#endif // !WITH_SWGREALMS_API
		zoneServer = new ZoneServer(configManager);
		processServer = new ZoneProcessServer(zoneServer);
		groundZone = new GroundZone(processServer, "test_zone");
		groundZone->createContainerComponent();
		groundZone->_setObjectID(1);
	}

	void TearDown() {
		// Perform clean up of common constructs here.
#ifndef WITH_SWGREALMS_API
		if (database != nullptr) {
			delete database;
			database = nullptr;
		}
#endif // !WITH_SWGREALMS_API

		if (playerManager != nullptr) {
			playerManager->finalize();
			playerManager = nullptr;
		}

		groundZone = nullptr;
		processServer = nullptr;
		zoneServer = nullptr;
	}
};

TEST_F(ZoneTest, GalaxyList) {
	auto galaxies = GalaxyList(1);

	while(galaxies.next()) {
		std::cerr << "[>>>>>>>>>>] " << galaxies.toString().toCharArray() << std::endl;

#ifdef USE_RANDOM_EXTRA_PORTS
		// Make a couple calls to getRandomPort()
		std::cerr << "[>>>>>>>>>>] getRandomPort " << galaxies.getRandomPort();
		for (int i = 0; i < 3;i++) {
			std::cerr << " " << galaxies.getRandomPort();
		}
		std::cerr<< std::endl;
#endif // USE_RANDOM_EXTRA_PORTS
	}
}

TEST_F(ZoneTest, PlayerManager) {
	playerManager = new PlayerManager( zoneServer, processServer, false);
}

TEST_F(ZoneTest, TreLoad) {
	TemplateManager::instance();

	ASSERT_EQ(TemplateManager::ERROR_CODE, 0);

	if (TemplateManager::instance()->loadedTemplatesCount == 0) {
		TemplateManager::instance()->loadLuaTemplates();
		ASSERT_EQ(TemplateManager::ERROR_CODE, 0);
	}
}

TEST_F(ZoneTest, ActiveAreaTest) {
	Reference<MockActiveArea*> activeArea = createActiveArea(true).castTo<MockActiveArea*>();
	ON_CALL(*activeArea, getZone()).WillByDefault(Return(groundZone));
	ON_CALL(*activeArea, getZoneUnsafe()).WillByDefault(Return(groundZone));
	ON_CALL(*activeArea, getParent()).WillByDefault(Return(ManagedWeakReference<SceneObject*>(NULL)));
	EXPECT_CALL(*activeArea, getZone()).Times(AnyNumber());
	EXPECT_CALL(*activeArea, getZoneUnsafe()).Times(AnyNumber());
	EXPECT_CALL(*activeArea, getParent()).Times(AnyNumber());
	EXPECT_CALL(*activeArea, enqueueEnterEvent(_)).Times(AnyNumber());
	EXPECT_CALL(*activeArea, enqueueExitEvent(_)).Times(AnyNumber());

	Locker alocker(activeArea);

	activeArea->setRadius(128);
	activeArea->initializePosition(0, 0, 0);

	groundZone->transferObject(activeArea, -1);

	alocker.release();

	Reference<TangibleObject*> tano = createTangibleObject();

	Locker slocker(tano);

	tano->initializePosition(0, 0, 0);

	ASSERT_EQ(tano->getActiveAreasSize(), 0);

	groundZone->transferObject(tano, -1);

	ASSERT_EQ(tano->getActiveAreasSize(), 1);

	tano->teleport(200, 0, 0);

	ASSERT_EQ(tano->getActiveAreasSize(), 0);

	tano->teleport(120, 0, 0);

	ASSERT_EQ(tano->getActiveAreasSize(), 1);

	slocker.release();

	Locker blocker(activeArea);

	activeArea->destroyObjectFromWorld(false);

	blocker.release();

	Locker s2locker(tano);

	ASSERT_EQ(tano->getActiveAreasSize(), 0);

	tano->destroyObjectFromWorld(false);
}

TEST_F(ZoneTest, InRangeTest) {
	ASSERT_EQ(groundZone->getZoneObjectCount(), 0);

	Reference<SceneObject*> scene = createSceneObject();

	Locker slocker(scene);

	groundZone->transferObject(scene, -1);

	ASSERT_EQ(groundZone->getZoneObjectCount(), 1);

	ASSERT_TRUE(scene->getZone() != nullptr);

	SortedVector<ManagedReference<TreeEntry*> > objects;

	groundZone->getInRangeObjects(0, 0, 0, 128, &objects, true);

	ASSERT_EQ(objects.size(), 1);

	scene->teleport(1000, 1000, 1000);

	objects.removeAll();

	groundZone->getInRangeObjects(0, 0, 0, 128, &objects, true);

	ASSERT_EQ(objects.size(), 0);

	objects.removeAll();

	groundZone->getInRangeObjects(1000, 0, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 1);

	slocker.release();

	Reference<SceneObject*> scene2 = createSceneObject();

	Locker s2locker(scene2);

	scene2->initializePosition(1000, 1000, 1000);

	groundZone->transferObject(scene2, -1);

	ASSERT_EQ(groundZone->getZoneObjectCount(), 2);

	objects.removeAll();

	groundZone->getInRangeObjects(1000, 0, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 2);

	scene2->destroyObjectFromWorld(false);

	ASSERT_EQ(groundZone->getZoneObjectCount(), 1);

	s2locker.release();

	Locker s3locker(scene);

	scene->destroyObjectFromWorld(false);

	ASSERT_EQ(groundZone->getZoneObjectCount(), 0);

	objects.removeAll();

	groundZone->getInRangeObjects(1000, 0, 1000, 128, &objects, true);

	ASSERT_EQ(objects.size(), 0);
}

TEST_F(ZoneTest, DatabaseInsertionRestoresUnchangedSavedZone) {
	auto scene = createSceneObject();
	scene->setZone(groundZone);

	EXPECT_TRUE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_TRUE(scene->isInQuadTree());
	EXPECT_EQ(1, groundZone->getZoneObjectCount());

	Locker locker(scene);
	scene->destroyObjectFromWorld(false);
}

TEST_F(ZoneTest, DatabaseInsertionDoesNotRespawnStoredObject) {
	auto scene = createSceneObject();
	scene->setZone(groundZone);

	// Model the world removal performed by vehicle storage after its database
	// load queued an insertion. Use the real removal path to clear the zone.
	{
		Locker locker(scene);
		scene->destroyObjectFromWorld(false);
	}

	EXPECT_TRUE(scene->getLocalZone() == nullptr);
	EXPECT_FALSE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_FALSE(scene->isInQuadTree());
	EXPECT_EQ(0, groundZone->getZoneObjectCount());

	// Cancelling the stale request must not prevent a later normal call.
	{
		Locker locker(scene);
		EXPECT_TRUE(groundZone->transferObject(scene, -1, false));
		EXPECT_TRUE(scene->isInQuadTree());
		scene->destroyObjectFromWorld(false);
	}
}

TEST_F(ZoneTest, DatabaseInsertionDoesNotRemoveObjectFromNewContainer) {
	auto scene = createSceneObject();
	auto container = createSceneObject();
	container->setContainerVolumeLimit(1);
	scene->setZone(groundZone);

	{
		Locker locker(scene);
		EXPECT_TRUE(container->transferObject(scene, -1, false));
	}

	EXPECT_FALSE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_TRUE(scene->getParent().get().get() == container.get());
	EXPECT_EQ(0, groundZone->getZoneObjectCount());

	Locker locker(scene);
	container->removeObject(scene, nullptr, false);
}

TEST_F(ZoneTest, DatabaseInsertionDoesNotInsertTwice) {
	auto scene = createSceneObject();
	scene->setZone(groundZone);

	EXPECT_TRUE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_FALSE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_EQ(1, groundZone->getZoneObjectCount());

	Locker locker(scene);
	scene->destroyObjectFromWorld(false);
}

TEST_F(ZoneTest, DatabaseInsertionDoesNotMoveObjectBackToSavedZone) {
	Reference<GroundZone*> otherZone = new GroundZone(processServer, "test_other_zone");
	otherZone->createContainerComponent();
	otherZone->_setObjectID(nextObjectId.increment());
	auto scene = createSceneObject();
	scene->setZone(groundZone);

	{
		Locker locker(scene);
		EXPECT_TRUE(otherZone->transferObject(scene, -1, false));
	}

	EXPECT_FALSE(DatabaseZoneInsertion::insertIfUnchanged(scene, groundZone));
	EXPECT_TRUE(scene->getLocalZone() == otherZone);
	EXPECT_EQ(0, groundZone->getZoneObjectCount());
	EXPECT_EQ(1, otherZone->getZoneObjectCount());

	Locker locker(scene);
	scene->destroyObjectFromWorld(false);
}

TEST_F(ZoneTest, DatabaseInsertionPreservesActiveAreaInsertion) {
	auto area = createActiveArea();
	area->setRadius(128);
	area->setZone(groundZone);

	EXPECT_TRUE(DatabaseZoneInsertion::insertIfUnchanged(area, groundZone));
	EXPECT_EQ(1, groundZone->getZoneObjectCount());

	SortedVector<ManagedReference<ActiveArea*>> areas;
	groundZone->getInRangeActiveAreas(0, 0, 0, &areas, true);
	EXPECT_EQ(1, areas.size());
	EXPECT_TRUE(areas.contains(area.get()));

	Locker locker(area);
	area->destroyObjectFromWorld(false);
}
