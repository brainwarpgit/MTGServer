#include "OrphanStructureReport.h"

#include <algorithm>
#include <cmath>
#include <vector>

#include "server/zone/Zone.h"
#include "server/zone/ZoneServer.h"
#include "server/zone/managers/player/PlayerManager.h"
#include "server/zone/objects/player/PlayerObject.h"
#include "server/zone/objects/player/sui/SuiCallback.h"
#include "server/zone/objects/player/sui/listbox/SuiListBox.h"
#include "server/zone/objects/structure/StructureObject.h"
#include "server/zone/objects/structure/StructureOwnership.h"
#include "server/zone/objects/waypoint/WaypointObject.h"
#include "templates/SharedObjectTemplate.h"

namespace {

constexpr int PAGE_SIZE = 50;
const String SCAN_TASK = "orphanStructureReport";

struct OrphanEntry {
	uint64 objectID;
	String description;
};

bool canUseReport(CreatureObject* player) {
	if (player == nullptr || !player->isPlayerCreature())
		return false;

	auto ghost = player->getPlayerObject();
	return ghost != nullptr && ghost->getAdminLevel() >= 15;
}

// The player registry includes offline characters. Being offline is not an orphan condition.
bool isSuspectedOrphan(StructureObject* structure, PlayerManager* playerManager) {
	if (structure == nullptr || playerManager == nullptr || structure->getZone() == nullptr)
		return false;

	if (!StructureOwnership::requiresPlayerOwner(structure->getObjectID(), structure->isPersistent(),
		structure->isClientObject(), structure->isCivicStructure()))
		return false;

	// Base defenses can refer to their building as their owner instead of a player.
	if (structure->isTurret() || structure->isMinefield() || structure->isScanner())
		return false;

	auto ownerID = structure->getOwnerObjectID();
	return ownerID == 0 || !playerManager->existsPlayerCreatureOID(ownerID);
}

class OrphanStructureSuiCallback : public SuiCallback {
	String planet;
	int page;

public:
	OrphanStructureSuiCallback(ZoneServer* server, const String& planetFilter, int pageNumber)
		: SuiCallback(server), planet(planetFilter), page(pageNumber) {
	}

	void run(CreatureObject* player, SuiBox* sui, uint32 eventIndex, Vector<UnicodeString>* args) override {
		if (eventIndex == 1 || sui == nullptr || !sui->isListBox() || args == nullptr || args->size() < 1 ||
			!canUseReport(player) || server == nullptr || server->isServerLoading() || server->isServerShuttingDown())
			return;

		const String selectedRow = args->get(0).toString();
		if (selectedRow.isEmpty() || selectedRow.length() > 3)
			return;
		int index = 0;
		for (int i = 0; i < selectedRow.length(); ++i) {
			if (selectedRow[i] < '0' || selectedRow[i] > '9')
				return;
			index = index * 10 + (selectedRow[i] - '0');
		}

		auto list = cast<SuiListBox*>(sui);
		if (index < 0 || index >= list->getMenuSize())
			return;

		const auto objectID = list->getMenuObjectID(index);
		// Report candidates use a nonzero database namespace, so 0 and 1 are navigation entries.
		if (objectID <= 1) {
			OrphanStructureReport::execute(player, planet + " " + String::valueOf(page + (objectID == 0 ? -1 : 1)));
			return;
		}

		auto object = server->getObject(objectID);
		if (object == nullptr || !object->isStructureObject()) {
			player->sendSystemMessage("That structure no longer exists. Run /server orphanstructures again to refresh the report.");
			return;
		}

		StructureObject* structure = cast<StructureObject*>(object.get());
		Vector3 position;
		uint32 planetCRC;
		String name;
		{
			Locker locker(structure, player);
			if (!isSuspectedOrphan(structure, server->getPlayerManager())) {
				player->sendSystemMessage("That structure no longer matches the orphan report. Refresh the report before continuing.");
				return;
			}

			position = structure->getWorldPosition();
			planetCRC = structure->getPlanetCRC();
			name = "Orphan: " + structure->getDisplayedName();
		}

		if (!canUseReport(player) || !player->isOnline() || server->isServerShuttingDown())
			return;
		auto ghost = player->getPlayerObject();
		ManagedReference<WaypointObject*> waypoint = server->createObject(0xc456e788, 1).castTo<WaypointObject*>();
		if (waypoint == nullptr) {
			player->sendSystemMessage("Unable to create the structure waypoint.");
			return;
		}

		Locker waypointLocker(waypoint);
		waypoint->setPlanetCRC(planetCRC);
		waypoint->setPosition(position.getX(), 0.f, position.getY());
		waypoint->setColor(WaypointObject::COLOR_GREEN);
		waypoint->setCustomObjectName(name, false);
		waypoint->setActive(true);
		ghost->addWaypoint(waypoint, false, true);
		player->sendSystemMessage("Created a waypoint to the suspected orphaned structure.");
	}
};

class OrphanStructureScanTask : public Task {
	ManagedWeakReference<CreatureObject*> requester;
	String planet;
	int page;

public:
	OrphanStructureScanTask(CreatureObject* player, const String& planetFilter, int pageNumber)
		: requester(player), planet(planetFilter), page(pageNumber) {
		setCustomTaskQueue("slowQueue");
	}

	void run() override {
		ManagedReference<CreatureObject*> player = requester.get();
		if (player == nullptr)
			return;

		ManagedReference<ZoneServer*> server;
		{
			Locker locker(player);
			if (!canUseReport(player) || !player->isOnline()) {
				player->removePendingTask(SCAN_TASK);
				return;
			}
			server = player->getZoneServer();
		}

		std::vector<OrphanEntry> entries;
		int zonesScanned = 0;
		bool complete = false;
		try {
			if (server != nullptr && !server->isServerLoading() && !server->isServerShuttingDown()) {
				auto playerManager = server->getPlayerManager();
				if (playerManager != nullptr) {
					complete = true;
					for (int z = 0; z < server->getZoneCount(); ++z) {
						if (server->isServerLoading() || server->isServerShuttingDown()) {
							complete = false;
							break;
						}

						ManagedReference<Zone*> zone = server->getZone(z);
						if (zone == nullptr || !zone->isGroundZone() || (planet != "all" && zone->getZoneName() != planet))
							continue;

						const float minX = zone->getMinX();
						const float maxX = zone->getMaxX();
						const float minY = zone->getMinY();
						const float maxY = zone->getMaxY();
						const float halfWidth = (maxX - minX) / 2.f;
						const float halfHeight = (maxY - minY) / 2.f;
						const float radius = std::sqrt(halfWidth * halfWidth + halfHeight * halfHeight) + 1.f;
						SortedVector<ManagedReference<TreeEntry*> > objects;
						zone->getInRangeObjects((minX + maxX) / 2.f, 0.f, (minY + maxY) / 2.f, radius, &objects, true, false);
						++zonesScanned;

						for (int i = 0; i < objects.size(); ++i) {
							if (server->isServerShuttingDown()) {
								complete = false;
								break;
							}

							auto object = objects.get(i).castTo<SceneObject*>();
							if (object == nullptr || !object->isStructureObject())
								continue;

							StructureObject* structure = cast<StructureObject*>(object.get());
							Locker locker(structure);
							if (structure->getZone() != zone || !isSuspectedOrphan(structure, playerManager))
								continue;

							StringBuffer description;
							description << zone->getZoneName() << " (" << (int)structure->getWorldPositionX() << ", "
								<< (int)structure->getWorldPositionY() << ") | " << structure->getDisplayedName();
							auto objectTemplate = structure->getObjectTemplate();
							if (objectTemplate != nullptr)
								description << " [" << objectTemplate->getTemplateFileName() << "]";
							description << " | ID " << structure->getObjectID() << " | ";
							if (structure->getOwnerObjectID() == 0)
								description << "no owner (0)";
							else
								description << "missing player " << structure->getOwnerObjectID();

							entries.push_back({structure->getObjectID(), description.toString()});
						}
						if (!complete)
							break;
					}
				}
			}
		} catch (Exception& error) {
			player->error("Orphan structure scan failed: " + error.getMessage());
			complete = false;
		} catch (...) {
			player->error("Orphan structure scan failed with an unexpected exception.");
			complete = false;
		}

		std::sort(entries.begin(), entries.end(), [](const OrphanEntry& a, const OrphanEntry& b) {
			return a.objectID < b.objectID;
		});
		entries.erase(std::unique(entries.begin(), entries.end(), [](const OrphanEntry& a, const OrphanEntry& b) {
			return a.objectID == b.objectID;
		}), entries.end());

		Locker locker(player);
		player->removePendingTask(SCAN_TASK);
		if (!canUseReport(player) || !player->isOnline())
			return;
		if (!complete || server == nullptr || server->isServerLoading() || server->isServerShuttingDown()) {
			player->sendSystemMessage("The orphan structure scan could not finish. Try again after the server is fully available.");
			return;
		}
		if (entries.empty()) {
			player->sendSystemMessage("No suspected orphaned player structures found on " + String::valueOf(zonesScanned) + " loaded planet(s).");
			return;
		}

		const int resultCount = static_cast<int>(entries.size());
		const int pages = (resultCount + PAGE_SIZE - 1) / PAGE_SIZE;
		page = std::min(page, pages);
		const int first = (page - 1) * PAGE_SIZE;
		const int last = std::min(first + PAGE_SIZE, resultCount);
		ManagedReference<SuiListBox*> list = new SuiListBox(player, SuiWindowType::ADMIN_ORPHAN_STRUCTURES);
		list->setCallback(new OrphanStructureSuiCallback(server, planet, page));
		list->setPromptTitle("Suspected orphaned player structures");
		StringBuffer prompt;
		prompt << resultCount << " result(s) on " << zonesScanned << " loaded planet(s). Page " << page << " of " << pages
			<< ".\nSelect a structure to create a waypoint. Each row shows planet, coordinates, name, template, object ID and owner issue."
			<< "\nWorld objects, temporary quest structures, civic buildings and base defenses are excluded. Offline owners are valid."
			<< "\nThis report uses the current player registry; character deletion cleanup may still be pending.";
		list->setPromptText(prompt.toString());
		list->setCancelButton(true, "@cancel");
		list->setOkButton(true, "@treasure_map/treasure_map:store_waypoint");
		if (page > 1)
			list->addMenuItem("< Previous page", 0);
		for (int i = first; i < last; ++i)
			list->addMenuItem(entries[i].description, entries[i].objectID);
		if (page < pages)
			list->addMenuItem("Next page >", 1);
		player->getPlayerObject()->addSuiBox(list);
		player->sendMessage(list->generateMessage());
	}
};

} // namespace

bool OrphanStructureReport::parseArguments(const String& arguments, String& planet, int& page) {
	planet = "all";
	page = 1;
	StringTokenizer tokens(arguments.toLowerCase());
	if (tokens.hasMoreTokens())
		tokens.getStringToken(planet);
	if (planet == "help" || planet == "?")
		return false;
	if (tokens.hasMoreTokens()) {
		String value;
		tokens.getStringToken(value);
		if (value.isEmpty() || value.length() > 6)
			return false;
		page = 0;
		for (int i = 0; i < value.length(); ++i) {
			if (value[i] < '0' || value[i] > '9')
				return false;
			page = page * 10 + (value[i] - '0');
		}
	}
	return page > 0 && !tokens.hasMoreTokens();
}

int OrphanStructureReport::execute(CreatureObject* player, const String& arguments) {
	if (!canUseReport(player)) {
		if (player != nullptr)
			player->sendSystemMessage("This command requires administrator level 15.");
		return 1;
	}

	String planet;
	int page;
	if (!parseArguments(arguments, planet, page)) {
		player->sendSystemMessage("Syntax: /server orphanstructures [all|planet] [page]. Lists suspected orphaned buildings and installations; select a result to create a waypoint.");
		return 1;
	}

	auto server = player->getZoneServer();
	if (server == nullptr || server->isServerLoading() || server->isServerShuttingDown()) {
		player->sendSystemMessage("Wait until the server is fully available before scanning structures.");
		return 1;
	}
	if (planet != "all") {
		auto zone = server->getZone(planet);
		if (zone == nullptr || !zone->isGroundZone()) {
			player->sendSystemMessage("Unknown or disabled ground planet: " + planet);
			return 1;
		}
	}
	if (player->getPendingTask(SCAN_TASK) != nullptr) {
		player->sendSystemMessage("Your orphan structure scan is already running.");
		return 1;
	}

	Reference<Task*> task = new OrphanStructureScanTask(player, planet, page);
	player->addPendingTask(SCAN_TASK, task, 0);
	player->sendSystemMessage("Scanning " + planet + " for suspected orphaned player structures...");
	return 0;
}
