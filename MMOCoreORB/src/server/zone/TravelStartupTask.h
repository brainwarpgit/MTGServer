#ifndef TRAVELSTARTUPTASK_H_
#define TRAVELSTARTUPTASK_H_

#include "engine/engine.h"

#include <memory>

namespace server {
namespace zone {
class ZoneServer;
namespace objects {
namespace creature {
class CreatureObject;
}
}
}
}

class ShuttleDepartureTask;

// One transient startup barrier per galaxy. Ordinary shuttle cycles never reset it.
class TravelStartupTask : public Task, public Logger {
	class State;
	std::unique_ptr<State> state;
	void scheduleNextCheck();
	void checkShuttles();

public:
	TravelStartupTask(server::zone::ZoneServer* server, int64 bootDeadline);
	~TravelStartupTask();

	void registerShuttle(server::zone::objects::creature::CreatureObject* shuttle);
	void setTravelPoint(uint64 shuttleID, const String& pointName, bool bound);
	void registerDeparture(uint64 shuttleID, ShuttleDepartureTask* departure);
	void finishRegistration(uint64 shuttleID, bool failed, bool discarded = false, const String& reason = String());
	void arm();
	void stop();
	void run() override;
	bool checkTravel(server::zone::objects::creature::CreatureObject* creature);
	String getStartupReport(const String& zoneFilter, int page, bool includeReady = false);

	static int64 secondsRemaining(int64 milliseconds);
	static int64 landingSecondsRemaining(int scheduledSeconds, int totalLandedTime, int landingTime);
	static String formatDuration(int64 seconds);
};

#endif /* TRAVELSTARTUPTASK_H_ */
