#include "gtest/gtest.h"

#include "server/zone/CloseObjectsVector.h"
#include "server/zone/managers/components/ComponentManager.h"
#include "server/zone/objects/scene/SceneObject.h"
#include "server/zone/objects/scene/components/GroundZoneComponent.h"
#include "server/zone/objects/scene/components/SpaceZoneComponent.h"

#include <functional>
#include <memory>
#include <vector>

namespace {

// SceneObjectImplementation dispatches native disappearance notifications to
// this component. No override of a generated implementation/stub is needed.
class CleanupRecordingComponent : public GroundZoneComponent {
public:
	std::function<void(SceneObject*, TreeEntry*)> onDisappear;

	void notifyDissapear(SceneObject* object, TreeEntry* departing) const override {
		onDisappear(object, departing);
	}
};

struct CleanupObject {
	Reference<SceneObject*> object;
	int disappearances = 0;
	std::function<void(TreeEntry*)> onDisappear;

	CleanupObject(uint64 id, bool ownVector) {
		object = new SceneObject();
		object->_setObjectID(id);
		object->setCloseObjects(ownVector ? new CloseObjectsVector() : nullptr);
		object->setGroundZoneComponent("GroundZoneComponent");
	}

	CloseObjectsVector* cov() const {
		return object->getCloseObjects();
	}

	TreeEntry* entry() const {
		return object.get();
	}
};

class CloseObjectsCleanupTest : public ::testing::TestWithParam<bool> {
protected:
	std::vector<std::unique_ptr<CleanupObject>> objects;
	SortedVector<ManagedReference<TreeEntry*>> scratch;
	Reference<GroundZoneComponent*> originalComponent;
	Reference<CleanupRecordingComponent*> recordingComponent;

	void SetUp() override {
		// Use the normal component setter and restore its registry entry after
		// every test. Fixtures have no zone, database, config, or TRE dependency.
		auto* manager = ComponentManager::instance();
		originalComponent = manager->getComponent<GroundZoneComponent*>("GroundZoneComponent");
		ASSERT_TRUE(originalComponent != nullptr);
		recordingComponent = new CleanupRecordingComponent();
		recordingComponent->onDisappear = [this](SceneObject* object, TreeEntry* departing) {
			for (auto& node : objects) {
				if (node->object.get() == object) {
					++node->disappearances;
					if (node->onDisappear) {
						node->onDisappear(departing);
					}
					return;
				}
			}
		};
		manager->putComponent("GroundZoneComponent", recordingComponent);
	}

	CleanupObject& makeObject(bool ownVector = true) {
		objects.emplace_back(new CleanupObject(objects.size() + 1, ownVector));
		return *objects.back();
	}

	void cleanup(CleanupObject& departing, CleanupObject& owner) {
		if (GetParam()) {
			SpaceZoneComponent::removeAllObjectsFromCOV(owner.cov(), scratch, departing.object, owner.object);
		} else {
			GroundZoneComponent::removeAllObjectsFromCOV(owner.cov(), scratch, departing.object, owner.object);
		}
	}

	void TearDown() override {
		// Break reciprocal references before destroying objects and callbacks.
		scratch.removeAll();
		for (auto& node : objects) {
			node->onDisappear = nullptr;
			if (node->cov() != nullptr) {
				node->cov()->removeAll();
			}
		}
		objects.clear();
		if (originalComponent != nullptr) {
			ComponentManager::instance()->putComponent("GroundZoneComponent", originalComponent);
		}
		if (recordingComponent != nullptr) {
			recordingComponent->onDisappear = nullptr;
		}
	}
};

TEST_P(CloseObjectsCleanupTest, BorrowedVectorPreservesParentAndUnrelatedObjects) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& recipient = makeObject();
	auto& unrelated = makeObject(false);

	owner.cov()->put(recipient.entry());
	owner.cov()->put(unrelated.entry());
	recipient.cov()->put(departing.entry());
	recipient.cov()->put(unrelated.entry());

	cleanup(departing, owner);

	EXPECT_EQ(2, owner.cov()->size());
	EXPECT_TRUE(owner.cov()->contains(recipient.entry()));
	EXPECT_TRUE(owner.cov()->contains(unrelated.entry()));
	EXPECT_FALSE(recipient.cov()->contains(departing.entry()));
	EXPECT_TRUE(recipient.cov()->contains(unrelated.entry()));
	EXPECT_EQ(1, recipient.disappearances);
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, BorrowedVectorWithoutReciprocalLinksPreservesParent) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& recipient = makeObject();
	owner.cov()->put(recipient.entry());

	cleanup(departing, owner);

	EXPECT_EQ(0, recipient.disappearances);
	EXPECT_TRUE(owner.cov()->contains(recipient.entry()));
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, BorrowedVectorRetriesCallbackReinsertionIntoEarlierNeighbor) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& first = makeObject();
	auto& second = makeObject();
	owner.cov()->put(first.entry());
	owner.cov()->put(second.entry());
	first.cov()->put(departing.entry());
	second.cov()->put(departing.entry());

	// Derive actual iteration order rather than relying on pointer or ID order.
	auto ordered = owner.cov()->getSafeCopy();
	ASSERT_EQ(2, ordered.size());
	auto* earlier = ordered.get(0).get() == first.entry() ? &first : &second;
	auto* later = earlier == &first ? &second : &first;
	later->onDisappear = [earlier](TreeEntry* object) {
		earlier->cov()->put(object);
	};

	cleanup(departing, owner);

	EXPECT_FALSE(first.cov()->contains(departing.entry()));
	EXPECT_FALSE(second.cov()->contains(departing.entry()));
	EXPECT_EQ(2, earlier->disappearances);
	EXPECT_EQ(1, later->disappearances);
	EXPECT_EQ(2, owner.cov()->size());
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, BorrowedVectorFindsNeighborAddedDuringNotification) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& initial = makeObject();
	auto& added = makeObject();
	owner.cov()->put(initial.entry());
	initial.cov()->put(departing.entry());
	initial.onDisappear = [&owner, &added](TreeEntry* object) {
		added.cov()->put(object);
		owner.cov()->put(added.entry());
	};

	cleanup(departing, owner);

	EXPECT_FALSE(initial.cov()->contains(departing.entry()));
	EXPECT_FALSE(added.cov()->contains(departing.entry()));
	EXPECT_EQ(1, added.disappearances);
	EXPECT_EQ(2, owner.cov()->size());
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, BorrowedVectorFindsReplacementWithoutSizeChange) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& initial = makeObject();
	auto& replacement = makeObject();
	owner.cov()->put(initial.entry());
	initial.cov()->put(departing.entry());
	initial.onDisappear = [&owner, &initial, &replacement](TreeEntry* object) {
		replacement.cov()->put(object);
		owner.cov()->drop(initial.entry());
		owner.cov()->put(replacement.entry());
	};

	cleanup(departing, owner);

	EXPECT_FALSE(replacement.cov()->contains(departing.entry()));
	EXPECT_EQ(1, replacement.disappearances);
	EXPECT_EQ(1, owner.cov()->size());
	EXPECT_TRUE(owner.cov()->contains(replacement.entry()));
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, OwnedVectorDrainsSelfAndPeersWithoutOwnerNotifications) {
	auto& departing = makeObject();
	auto& recipient = makeObject();
	auto& unrelated = makeObject(false);
	departing.cov()->put(departing.entry());
	departing.cov()->put(recipient.entry());
	departing.cov()->put(unrelated.entry());
	recipient.cov()->put(departing.entry());
	recipient.cov()->put(unrelated.entry());

	cleanup(departing, departing);

	EXPECT_EQ(0, departing.cov()->size());
	EXPECT_FALSE(recipient.cov()->contains(departing.entry()));
	EXPECT_TRUE(recipient.cov()->contains(unrelated.entry()));
	EXPECT_EQ(1, recipient.disappearances);
	EXPECT_EQ(0, departing.disappearances);
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, OwnedVectorStillProcessesNeighborsAddedDuringCleanup) {
	auto& departing = makeObject();
	auto& initial = makeObject();
	auto& added = makeObject();
	departing.cov()->put(initial.entry());
	initial.cov()->put(departing.entry());
	initial.onDisappear = [&departing, &added](TreeEntry* object) {
		added.cov()->put(object);
		departing.cov()->put(added.entry());
	};

	cleanup(departing, departing);

	EXPECT_EQ(0, departing.cov()->size());
	EXPECT_FALSE(added.cov()->contains(departing.entry()));
	EXPECT_EQ(1, added.disappearances);
	EXPECT_EQ(0, departing.disappearances);
	EXPECT_EQ(0, scratch.size());
}

TEST_P(CloseObjectsCleanupTest, BorrowedVectorRetainsHundredPassRetryLimit) {
	auto& owner = makeObject();
	auto& departing = makeObject(false);
	auto& recipient = makeObject();
	owner.cov()->put(recipient.entry());
	recipient.cov()->put(departing.entry());
	recipient.onDisappear = [&recipient](TreeEntry* object) {
		recipient.cov()->put(object);
	};

	cleanup(departing, owner);

	EXPECT_EQ(100, recipient.disappearances);
	EXPECT_TRUE(recipient.cov()->contains(departing.entry()));
	EXPECT_TRUE(owner.cov()->contains(recipient.entry()));
	EXPECT_EQ(0, scratch.size());
}

INSTANTIATE_TEST_SUITE_P(GroundAndSpace, CloseObjectsCleanupTest, ::testing::Bool());

} // namespace
