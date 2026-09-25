#include "server/zone/TravelStartupTask.h"

#include "server/chat/ChatManager.h"
#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/player/PlayerMap.h"
#include "server/zone/objects/building/tasks/ShuttleDepartureTask.h"
#include "server/zone/objects/creature/CreatureObject.h"
#include "server/zone/objects/player/PlayerObject.h"

#include <algorithm>
#include <exception>
#include <map>
#include <mutex>
#include <vector>

using namespace server::zone;
using namespace server::zone::objects::creature;

class TravelStartupTask::State {
public:
	enum class Progress { REGISTERING, LANDING, READY, FAILED, DISCARDED };

	// This part survives startup; it contains no live object or task references.
	struct ReportRow {
		uint64 id = 0;
		String planet = "unknown";
		String point;
		float x = 0;
		float y = 0;
		bool bound = false;
		Progress progress = Progress::REGISTERING;
		String reason = "Waiting for startup registration.";
	};

	struct Entry : ReportRow {
		ManagedWeakReference<CreatureObject*> shuttle;
		Reference<ShuttleDepartureTask*> departure;
	};

	ManagedWeakReference<ZoneServer*> server;
	std::mutex mutex;
	std::map<uint64, Entry> shuttles;
	int64 bootDeadline;
	int64 landingDeadline = 0;
	bool armed = false;
	bool stopped = false;
	bool released = false;
	bool registering = true;

	State(ZoneServer* zoneServer, int64 deadline) : server(zoneServer), bootDeadline(deadline) {
	}

	static const char* statusName(Progress progress) {
		switch (progress) {
		case Progress::REGISTERING: return "REGISTERING";
		case Progress::LANDING: return "LANDING";
		case Progress::READY: return "READY";
		case Progress::FAILED: return "ISSUE";
		case Progress::DISCARDED: return "DISCARDED DUPLICATE";
		}

		return "UNKNOWN";
	}

	static String formatRow(const ReportRow& row) {
		StringBuffer text;
		text << "[" << statusName(row.progress) << "] " << row.planet << " | ";

		if (row.point.isEmpty())
			text << "port=unmatched";
		else
			text << (row.bound ? "port=" : "nearest=") << row.point;

		text << " | x=" << row.x << " y=" << row.y << " | shuttle=0x" << String::hexvalueOf(row.id)
			<< "\n  " << row.reason;
		return text.toString();
	}
};

TravelStartupTask::TravelStartupTask(ZoneServer* server, int64 bootDeadline) : state(new State(server, bootDeadline)) {
	setLoggingName("TravelStartupTask");
}

TravelStartupTask::~TravelStartupTask() = default;

int64 TravelStartupTask::secondsRemaining(int64 milliseconds) {
	if (milliseconds <= 0)
		return 0;

	return milliseconds / 1000 + (milliseconds % 1000 != 0);
}

int64 TravelStartupTask::landingSecondsRemaining(int scheduledSeconds, int totalLandedTime, int landingTime) {
	// Match the existing strict isLanded() boundary without changing shuttle timing.
	const int64 remaining = int64(scheduledSeconds) - totalLandedTime + 2 * int64(landingTime) + 1;
	return remaining > 0 ? remaining : 0;
}

String TravelStartupTask::formatDuration(int64 seconds) {
	if (seconds <= 0)
		return "0 seconds";

	const int64 minutes = seconds / 60;
	seconds %= 60;
	StringBuffer message;

	if (minutes > 0)
		message << minutes << " minute" << (minutes == 1 ? "" : "s");

	if (seconds > 0) {
		if (minutes > 0)
			message << " and ";

		message << seconds << " second" << (seconds == 1 ? "" : "s");
	}

	return message.toString();
}

void TravelStartupTask::registerShuttle(CreatureObject* shuttle) {
	if (shuttle == nullptr)
		return;

	// Capture world metadata before taking the tracker lock. Registration updates
	// the port name using its existing lookup, without adding another manager lock.
	State::Entry entry;
	entry.id = shuttle->getObjectID();
	entry.shuttle = shuttle;
	entry.x = shuttle->getWorldPositionX();
	entry.y = shuttle->getWorldPositionY();
	ManagedReference<Zone*> zone = shuttle->getZone();

	if (zone != nullptr)
		entry.planet = zone->getZoneName();

	std::lock_guard<std::mutex> guard(state->mutex);

	if (state->armed || state->stopped || state->released)
		return;

	state->shuttles.emplace(entry.id, entry);
}

void TravelStartupTask::setTravelPoint(uint64 shuttleID, const String& pointName, bool bound) {
	std::lock_guard<std::mutex> guard(state->mutex);
	auto found = state->shuttles.find(shuttleID);

	if (found == state->shuttles.end())
		return;

	found->second.point = pointName;
	found->second.bound = bound;
}

void TravelStartupTask::registerDeparture(uint64 shuttleID, ShuttleDepartureTask* departure) {
	std::lock_guard<std::mutex> guard(state->mutex);
	auto found = state->shuttles.find(shuttleID);

	if (state->stopped || state->released || found == state->shuttles.end())
		return;

	if (found->second.progress == State::Progress::REGISTERING)
		found->second.departure = departure;
}

void TravelStartupTask::finishRegistration(uint64 shuttleID, bool failed, bool discarded, const String& reason) {
	std::lock_guard<std::mutex> guard(state->mutex);
	auto found = state->shuttles.find(shuttleID);

	if (found == state->shuttles.end())
		return;

	auto& entry = found->second;

	// Keep a late authoritative duplicate result accurate even if an earlier
	// removal/exception was already recorded. This never reopens startup.
	if (discarded && entry.progress == State::Progress::FAILED) {
		entry.progress = State::Progress::DISCARDED;
		entry.reason = reason.isEmpty() ? "Duplicate shuttle discarded." : reason;
		entry.shuttle = nullptr;
		entry.departure = nullptr;
		return;
	}

	if (state->stopped || state->released)
		return;

	if (entry.progress != State::Progress::REGISTERING)
		return;

	if (discarded) {
		entry.progress = State::Progress::DISCARDED;
		entry.reason = reason.isEmpty() ? "Duplicate shuttle discarded." : reason;
	} else if (failed) {
		entry.progress = State::Progress::FAILED;
		entry.reason = reason.isEmpty() ? "Shuttle registration failed." : reason;
	} else if (entry.departure == nullptr) {
		entry.progress = State::Progress::FAILED;
		entry.reason = "Registration completed without a departure task.";
	} else {
		entry.progress = State::Progress::LANDING;
		entry.reason = "Registered; waiting for initial boarding.";
	}
}

String TravelStartupTask::getStartupReport(const String& zoneFilter, int page, bool includeReady) {
	String filter = zoneFilter.toLowerCase();

	if (filter == "all")
		filter = "";

	std::vector<State::ReportRow> rows;
	unsigned int ready = 0;
	unsigned int failed = 0;
	unsigned int discarded = 0;
	unsigned int pending = 0;
	bool released;
	bool stopped;

	{
		std::lock_guard<std::mutex> guard(state->mutex);
		released = state->released;
		stopped = state->stopped;

		for (const auto& item : state->shuttles) {
			const auto& entry = item.second;

			if (!filter.isEmpty() && entry.planet != filter)
				continue;

			switch (entry.progress) {
			case State::Progress::READY: ++ready; break;
			case State::Progress::FAILED: ++failed; break;
			case State::Progress::DISCARDED: ++discarded; break;
			case State::Progress::REGISTERING:
			case State::Progress::LANDING: ++pending; break;
			}

			if (includeReady || (entry.progress != State::Progress::READY && entry.progress != State::Progress::DISCARDED))
				rows.push_back(entry);
		}
	}

	// Format only copied metadata; do not resolve objects or take world locks.
	std::sort(rows.begin(), rows.end(), [](const State::ReportRow& left, const State::ReportRow& right) {
		const int planetOrder = left.planet.compareTo(right.planet);

		if (planetOrder != 0)
			return planetOrder < 0;

		const int pointOrder = left.point.compareTo(right.point);

		if (pointOrder != 0)
			return pointOrder < 0;

		return left.id < right.id;
	});

	const size_t pageSize = 20;
	const size_t pages = rows.empty() ? 1 : (rows.size() + pageSize - 1) / pageSize;

	if (page < 1)
		page = 1;

	StringBuffer report;
	report << "Shuttle and starport startup history (this server boot)\n"
		<< "This is startup history, not a live port or boarding-status audit.\n"
		<< "Scope: " << (filter.isEmpty() ? String("all planets") : filter)
		<< "; startup " << (stopped ? "stopped" : (released ? "finished" : "in progress")) << ".\n"
		<< "Ready: " << ready << "; issues: " << failed << "; pending: " << pending
		<< "; discarded duplicates: " << discarded << ".\n"
		<< "Showing " << (includeReady ? "all entries" : "issues and pending entries")
		<< "; page " << page << " of " << uint64(pages) << " (20 entries per page).\n";

	if (size_t(page) > pages) {
		report << "That page does not exist. Choose a page from 1 to " << uint64(pages) << ".";
		return report.toString();
	}

	if (rows.empty()) {
		report << "No matching " << (includeReady ? "startup entries" : "startup issues or pending entries") << ".";
		return report.toString();
	}

	const size_t first = (size_t(page) - 1) * pageSize;
	const size_t end = std::min(first + pageSize, rows.size());

	for (size_t i = first; i < end; ++i)
		report << "\n" << State::formatRow(rows[i]) << "\n";

	return report.toString();
}

void TravelStartupTask::arm() {
	{
		std::lock_guard<std::mutex> guard(state->mutex);

		if (state->armed || state->stopped || state->released)
			return;

		state->armed = true;
	}

	scheduleNextCheck();
}

void TravelStartupTask::scheduleNextCheck() {
	std::lock_guard<std::mutex> guard(state->mutex);

	// Serialize scheduling with stop(), so cancellation cannot be followed by
	// a late retry being queued by an already-running monitor tick.
	if (state->armed && !state->stopped && !state->released)
		schedule(1000);
}

void TravelStartupTask::stop() {
	{
		std::lock_guard<std::mutex> guard(state->mutex);
		state->stopped = true;
	}

	cancel();
}

bool TravelStartupTask::checkTravel(CreatureObject* creature) {
	String message;
	const int64 now = System::getMiliTime();

	{
		std::lock_guard<std::mutex> guard(state->mutex);

		if (state->stopped) {
			message = "Shuttles and starports are unavailable while the server is shutting down.";
		} else if (state->released) {
			return true;
		} else {
			const int64 bootSeconds = secondsRemaining(state->bootDeadline - now);
			const int64 landingSeconds = secondsRemaining(state->landingDeadline - now);

			if (bootSeconds > 0) {
				message = "Shuttle and starport startup delay remaining: " + formatDuration(bootSeconds)
					+ ". Travel opens after the initial shuttle landings.";
			} else if (!state->registering && landingSeconds > 0) {
				message = "Shuttles and starports are completing their initial landings. Travel should be ready in approximately "
					+ formatDuration(landingSeconds) + ".";
			} else {
				message = "Shuttles and starports are finishing startup. Please try again shortly.";
			}
		}
	}

	if (creature != nullptr)
		creature->sendSystemMessage(message);

	return false;
}

void TravelStartupTask::run() {
	try {
		checkShuttles();
	} catch (const Exception& exception) {
		error(exception.getMessage());
		exception.printStackTrace();
		scheduleNextCheck();
	} catch (const std::exception& exception) {
		error(exception.what());
		scheduleNextCheck();
	} catch (...) {
		error("Unexpected exception checking shuttle startup readiness.");
		scheduleNextCheck();
	}
}

void TravelStartupTask::checkShuttles() {
	ManagedReference<ZoneServer*> server = state->server.get();

	if (server == nullptr || server->isServerOffline() || server->isServerShuttingDown()) {
		stop();
		return;
	}

	if (server->isServerLoading()) {
		scheduleNextCheck();
		return;
	}

	std::vector<std::pair<uint64, State::Entry>> snapshot;

	{
		std::lock_guard<std::mutex> guard(state->mutex);

		if (!state->armed || state->stopped || state->released)
			return;

		for (const auto& entry : state->shuttles)
			snapshot.push_back(entry);
	}

	struct Update {
		uint64 id;
		State::Progress before;
		State::Progress after;
		String reason;
	};

	std::vector<Update> updates;
	int64 landingDeadline = 0;
	const int64 now = System::getMiliTime();

	// Never hold the tracker mutex while taking a shuttle or manager lock.
	// Resolving a ManagedWeakReference requires a mutable reference wrapper.
	for (auto& item : snapshot) {
		auto& entry = item.second;

		// Registration owns its terminal outcome. In particular, duplicate
		// disposal removes the object before its registration scope guard runs.
		// Inspecting REGISTERING objects here would race that benign removal.
		if (entry.progress != State::Progress::LANDING)
			continue;

		try {
			ManagedReference<CreatureObject*> shuttle = entry.shuttle.get();

			if (shuttle == nullptr) {
				updates.push_back({item.first, entry.progress, State::Progress::FAILED, "Shuttle expired before initial boarding."});
				continue;
			}

			Locker locker(shuttle);

			if (shuttle->getZone() == nullptr || shuttle->getZoneServer() != server.get()) {
				updates.push_back({item.first, entry.progress, State::Progress::FAILED, "Shuttle was removed from the world before initial boarding."});
				continue;
			}

			auto departure = entry.departure;

			if (departure == nullptr) {
				updates.push_back({item.first, entry.progress, State::Progress::FAILED, "Departure task unavailable before initial boarding."});
			} else if (departure->hasCompletedFirstLanding() || departure->isLanded()) {
				updates.push_back({item.first, entry.progress, State::Progress::READY, "Initial boarding became available."});
			} else {
				const int64 seconds = landingSecondsRemaining(departure->getSecondsRemaining(), departure->getLandedTime(), departure->getLandingTime());
				const int64 deadline = now + seconds * 1000;

				if (deadline > landingDeadline)
					landingDeadline = deadline;
			}
		} catch (const Exception& exception) {
			error() << "Failed to inspect startup shuttle " << item.first << ": " << exception.getMessage();
			exception.printStackTrace();
			updates.push_back({item.first, entry.progress, State::Progress::FAILED, "Exception checking initial boarding: " + exception.getMessage()});
		} catch (const std::exception& exception) {
			error() << "Failed to inspect startup shuttle " << item.first << ": " << exception.what();
			updates.push_back({item.first, entry.progress, State::Progress::FAILED, String("Exception checking initial boarding: ") + exception.what()});
		} catch (...) {
			error() << "Unexpected exception inspecting startup shuttle " << item.first;
			updates.push_back({item.first, entry.progress, State::Progress::FAILED, "Unknown exception checking initial boarding."});
		}
	}

	bool release = false;
	unsigned int failures = 0;
	unsigned int ready = 0;
	std::vector<State::ReportRow> issues;

	{
		std::lock_guard<std::mutex> guard(state->mutex);

		if (state->stopped || state->released)
			return;

		for (const auto& update : updates) {
			auto found = state->shuttles.find(update.id);

			if (found != state->shuttles.end() && found->second.progress == update.before) {
				found->second.progress = update.after;
				found->second.reason = update.reason;
			}
		}

		unsigned int registering = 0;
		unsigned int landing = 0;

		for (const auto& item : state->shuttles) {
			switch (item.second.progress) {
			case State::Progress::REGISTERING: ++registering; break;
			case State::Progress::LANDING: ++landing; break;
			case State::Progress::READY: ++ready; break;
			case State::Progress::FAILED: ++failures; break;
			case State::Progress::DISCARDED: break;
			}
		}

		state->registering = registering != 0;
		state->landingDeadline = landingDeadline;

		if (registering == 0 && landing == 0 && now >= state->bootDeadline) {
			state->released = true;

			for (auto& item : state->shuttles) {
				auto& entry = item.second;

				if (entry.progress == State::Progress::FAILED)
					issues.push_back(entry);

				// Retain diagnostic metadata for this boot without retaining
				// runtime references or extending any shuttle task lifetime.
				entry.shuttle = nullptr;
				entry.departure = nullptr;
			}

			release = true;
		}
	}

	if (!release) {
		scheduleNextCheck();
		return;
	}

	for (const auto& issue : issues)
		info(false) << "Startup shuttle issue: " << State::formatRow(issue);

	if (server->isServerOffline() || server->isServerShuttingDown())
		return;

	String message;

	if (ready == 0) {
		message = "Shuttle and starport startup has finished. Service availability could not be confirmed.";
	} else {
		message = "Shuttle and starport services are now operating. Normal boarding schedules apply.";
	}

	info(true) << message;
	ManagedReference<::server::chat::ChatManager*> chat = server->getChatManager();

	if (chat == nullptr || server->isServerOffline() || server->isServerShuttingDown())
		return;

	chat->broadcastGalaxy(nullptr, message);

	if (failures == 0 && ready != 0)
		return;

	StringBuffer details;
	details << "Shuttle and starport startup needs administrator review: " << failures
		<< " shuttle(s) recorded issues; " << ready
		<< " reached initial boarding. Use /server travelstatus for details.";
	const String adminMessage = details.toString();
	info(true) << adminMessage;

	// ChatManager owns this map's iteration lock. Copy references before
	// inspecting recipients or sending messages, without holding that lock.
	std::vector<ManagedReference<CreatureObject*>> players;

	{
		Locker chatLocker(chat);
		PlayerMap* playerMap = chat->getPlayerMap();

		if (playerMap == nullptr)
			return;

		playerMap->resetIterator(false);

		while (playerMap->hasNext(false)) {
			ManagedReference<CreatureObject*> player = playerMap->getNextValue(false);

			if (player != nullptr)
				players.push_back(player);
		}
	}

	for (const auto& player : players) {
		if (!player->isOnline())
			continue;

		auto ghost = player->getPlayerObject();

		// Match the permission required to inspect /server travelstatus.
		if (ghost == nullptr || ghost->getAdminLevel() < 15)
			continue;

		player->sendSystemMessage(adminMessage);
	}
}
