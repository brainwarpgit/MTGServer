#include "gtest/gtest.h"

#include "conf/ConfigManager.h"
#include "server/zone/managers/player/OnlineZoneClientMap.h"

namespace {

class ShutdownClientSnapshotTest : public ::testing::Test {
protected:
	OnlineZoneClientMap clients;
	int originalClientLogLevel = -1;

	void SetUp() override {
		// These sessions have no network transport. Disable transport logging
		// during construction even when the user's config enables it.
		auto config = ConfigManager::instance();
		originalClientLogLevel = config->getInt("Core3.ZoneServer.ClientLogLevel", -1);
		config->setInt("Core3.ZoneServer.ClientLogLevel", -1);
	}

	void TearDown() override {
		ConfigManager::instance()->setInt("Core3.ZoneServer.ClientLogLevel", originalClientLogLevel);
	}

	Reference<ZoneClientSession*> addSession(uint32 accountID, const String& ip = "192.0.2.1") {
		Reference<ZoneClientSession*> session = new ZoneClientSession(static_cast<BaseClientProxy*>(nullptr));
		session->setIPAddress(ip);

		Vector<Reference<ZoneClientSession*> > accountSessions;

		if (clients.containsKey(accountID)) {
			accountSessions = clients.get(accountID);
		}

		accountSessions.add(session);
		clients.put(accountID, accountSessions);
		return session;
	}

	int countSession(const Vector<Reference<ZoneClientSession*> >& snapshot, ZoneClientSession* session) {
		int count = 0;

		for (int i = 0; i < snapshot.size(); ++i) {
			if (snapshot.get(i) == session) {
				++count;
			}
		}

		return count;
	}
};

TEST_F(ShutdownClientSnapshotTest, EmptyMapHasNoSessions) {
	EXPECT_EQ(0, clients.getSessionsSnapshot().size());
}

TEST_F(ShutdownClientSnapshotTest, KeepsSeparateAccountsOnTheSameIPAddress) {
	auto first = addSession(1);
	auto second = addSession(2);
	auto third = addSession(3);

	ASSERT_TRUE(first->getIPAddress() == second->getIPAddress());
	ASSERT_TRUE(first->getIPAddress() == third->getIPAddress());
	auto snapshot = clients.getSessionsSnapshot();

	ASSERT_EQ(3, snapshot.size());
	EXPECT_EQ(1, countSession(snapshot, first));
	EXPECT_EQ(1, countSession(snapshot, second));
	EXPECT_EQ(1, countSession(snapshot, third));
}

TEST_F(ShutdownClientSnapshotTest, KeepsAllSessionsForOneAccount) {
	auto first = addSession(7);
	auto second = addSession(7);
	auto third = addSession(7, "192.0.2.2");
	auto snapshot = clients.getSessionsSnapshot();

	ASSERT_EQ(1, clients.size());
	ASSERT_EQ(3, snapshot.size());
	EXPECT_EQ(1, countSession(snapshot, first));
	EXPECT_EQ(1, countSession(snapshot, second));
	EXPECT_EQ(1, countSession(snapshot, third));
}

TEST_F(ShutdownClientSnapshotTest, IgnoresNullSessionsAndEmptyAccounts) {
	auto valid = addSession(1);
	auto accountSessions = clients.get(1);
	accountSessions.add(nullptr);
	clients.put(1, accountSessions);

	Vector<Reference<ZoneClientSession*> > emptyAccount;
	clients.put(2, emptyAccount);
	emptyAccount.add(nullptr);
	clients.put(3, emptyAccount);

	auto snapshot = clients.getSessionsSnapshot();
	ASSERT_EQ(1, snapshot.size());
	EXPECT_EQ(valid.get(), snapshot.get(0).get());
}

TEST_F(ShutdownClientSnapshotTest, RemovingAccountsDuringTraversalDoesNotSkipSessions) {
	Vector<Reference<ZoneClientSession*> > expected;

	for (uint32 accountID = 1; accountID <= 64; ++accountID) {
		expected.add(addSession(accountID));
	}

	auto snapshot = clients.getSessionsSnapshot();
	Vector<Reference<ZoneClientSession*> > visited;

	for (int i = 0; i < snapshot.size(); ++i) {
		auto session = snapshot.get(i);
		visited.add(session);

		for (int j = 0; j < expected.size(); ++j) {
			if (expected.get(j) == session) {
				clients.remove(static_cast<uint32>(j + 1));
				break;
			}
		}
	}

	EXPECT_EQ(0, clients.size());
	ASSERT_EQ(expected.size(), visited.size());

	for (int i = 0; i < expected.size(); ++i) {
		EXPECT_EQ(1, countSession(visited, expected.get(i)));
	}
}

TEST_F(ShutdownClientSnapshotTest, SnapshotKeepsSessionsAliveAfterMapIsCleared) {
	auto session = addSession(1);
	auto original = session.get();
	auto snapshot = clients.getSessionsSnapshot();

	clients.removeAll();
	session = nullptr;

	ASSERT_EQ(1, snapshot.size());
	EXPECT_EQ(original, snapshot.get(0).get());
	EXPECT_TRUE(snapshot.get(0)->getIPAddress() == "192.0.2.1");
	EXPECT_EQ(0, clients.getSessionsSnapshot().size());
}

TEST_F(ShutdownClientSnapshotTest, LaterRegistrationsDoNotChangeExistingSnapshot) {
	auto first = addSession(1);
	auto snapshot = clients.getSessionsSnapshot();
	auto second = addSession(1);
	auto third = addSession(2);

	ASSERT_EQ(1, snapshot.size());
	EXPECT_EQ(first.get(), snapshot.get(0).get());
	EXPECT_EQ(0, countSession(snapshot, second));
	EXPECT_EQ(0, countSession(snapshot, third));
	EXPECT_EQ(3, clients.getSessionsSnapshot().size());
}

} // namespace
