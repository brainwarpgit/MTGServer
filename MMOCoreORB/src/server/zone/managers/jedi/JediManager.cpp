/*
				Copyright <SWGEmu>
		See file COPYING for copying conditions.*/

#include "JediManager.h"
#include "server/zone/managers/director/DirectorManager.h"
#include "server/zone/objects/creature/variables/Skill.h"
#include "server/zone/objects/creature/variables/SkillList.h"
#include "server/zone/objects/player/PlayerObject.h"

JediManager::JediManager() : Logger("JediManager") {
	jediProgressionType = NOJEDIPROGRESSION;
	setJediManagerName("JediManager");
}

JediManager::~JediManager() {

}

const String& JediManager::getJediManagerName() {
	ReadLocker locker(this);

	return jediManagerName;
}

int JediManager::getJediProgressionType() {
	ReadLocker locker(this);

	return jediProgressionType;
}

void JediManager::setJediManagerName(const String& name) {
	Locker writeLock(this);

	jediManagerName = name;
}

void JediManager::setupLuaValues(Lua* luaEngine) {
	luaEngine->setGlobalInt("NOJEDIPROGRESSION", JediManager::NOJEDIPROGRESSION);
	luaEngine->setGlobalInt("HOLOGRINDJEDIPROGRESSION", JediManager::HOLOGRINDJEDIPROGRESSION);
	luaEngine->setGlobalInt("VILLAGEJEDIPROGRESSION", JediManager::VILLAGEJEDIPROGRESSION);
	luaEngine->setGlobalInt("CUSTOMJEDIPROGRESSION", JediManager::CUSTOMJEDIPROGRESSION);
	luaEngine->setGlobalInt("ITEMHOLOCRON", JediManager::ITEMHOLOCRON);
	luaEngine->setGlobalInt("ITEMWAYPOINTDATAPAD", JediManager::ITEMWAYPOINTDATAPAD);
	luaEngine->setGlobalInt("ITEMTHEATERDATAPAD", JediManager::ITEMTHEATERDATAPAD);
}

void JediManager::loadConfiguration(Lua* luaEngine) {
	setupLuaValues(luaEngine);

	luaEngine->runFile("scripts/managers/jedi/jedi_manager.lua");

	jediProgressionType = luaEngine->getGlobalInt(String("jediProgressionType"));

	switch (jediProgressionType) {
	case HOLOGRINDJEDIPROGRESSION:
		luaEngine->runFile("scripts/managers/jedi/hologrind_jedi_manager.lua");
		break;
	case VILLAGEJEDIPROGRESSION:
		luaEngine->runFile("scripts/managers/jedi/village_jedi_manager.lua");
		break;
	case CUSTOMJEDIPROGRESSION:
		luaEngine->runFile(luaEngine->getGlobalString("customJediProgressionFile"));
		break;
	default:
		break;
	}

	if (loaded.compareAndSet(false, true)) {
		auto managerName = luaEngine->getGlobalString(String("jediManagerName"));

		setJediManagerName(managerName);
	}

	info() << Thread::getCurrentThread()->getName() <<  " loaded.";
}

void JediManager::onPlayerCreated(CreatureObject* creature) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaOnPlayerCreated = lua->createFunction(getJediManagerName(), "onPlayerCreated", 0);
	*luaOnPlayerCreated << creature;

	luaOnPlayerCreated->callFunction();
}

void JediManager::onSkillRevoked(CreatureObject* creature, Skill* skill) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaOnSkillRevoked = lua->createFunction(getJediManagerName(), "onSkillRevoked", 0);
	*luaOnSkillRevoked << creature;
	*luaOnSkillRevoked << skill;

	luaOnSkillRevoked->callFunction();
}

void JediManager::onPlayerLoggedIn(CreatureObject* creature) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaOnPlayerLoggedIn = lua->createFunction(getJediManagerName(), "onPlayerLoggedIn", 0);
	*luaOnPlayerLoggedIn << creature;

	luaOnPlayerLoggedIn->callFunction();
}

void JediManager::onPlayerLoggedOut(CreatureObject* creature) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaOnPlayerLoggedOut = lua->createFunction(getJediManagerName(), "onPlayerLoggedOut", 0);
	*luaOnPlayerLoggedOut << creature;

	luaOnPlayerLoggedOut->callFunction();
}

void JediManager::checkForceStatusCommand(CreatureObject* creature) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaCheckForceStatusCommand = lua->createFunction(getJediManagerName(), "checkForceStatusCommand", 0);
	*luaCheckForceStatusCommand << creature;

	luaCheckForceStatusCommand->callFunction();
}

void JediManager::useItem(SceneObject* item, const int itemType, CreatureObject* creature) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaUseItem = lua->createFunction(getJediManagerName(), "useItem", 0);
	*luaUseItem << item;
	*luaUseItem << itemType;
	*luaUseItem << creature;

	luaUseItem->callFunction();
}

bool JediManager::canLearnSkill(CreatureObject* creature, const String& skillName) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaStartTask = lua->createFunction(getJediManagerName(), "canLearnSkill", 1);
	*luaStartTask << creature;
	*luaStartTask << skillName;

	lua_State* L = luaStartTask->callFunction();

	bool result = lua_toboolean(L, -1);

	lua_pop(L, 1);

	return result;
}

bool JediManager::canSurrenderSkill(CreatureObject* creature, const String& skillName) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaStartTask = lua->createFunction(getJediManagerName(), "canSurrenderSkill", 1);
	*luaStartTask << creature;
	*luaStartTask << skillName;

	lua_State* L = luaStartTask->callFunction();

	bool result = lua_toboolean(L, -1);

	lua_pop(L, 1);

	return result;
}

bool JediManager::canSurrenderSkills(CreatureObject* creature, const Vector<String>& orderedSkillNames) {
	if (creature == nullptr || orderedSkillNames.size() == 0) {
		return false;
	}

	Locker locker(creature);

	if (creature->getPlayerObject() == nullptr) {
		return false;
	}

	const SkillList* ownedSkills = creature->getSkillList();
	Vector<Skill*> orderedSkills;
	int forceSensitiveCount = 0;
	int jediPoints = 0;
	int jediFullTrees = 0;

	for (int i = 0; i < ownedSkills->size(); ++i) {
		Skill* skill = ownedSkills->get(i);
		const String& name = skill->getSkillName();

		// Match SkillManager's force-sensitive and Village Knight counters.
		if (name.contains("force_sensitive") && name.indexOf("0") != -1) {
			++forceSensitiveCount;
		}

		if (name.contains("force_discipline_") && (name.indexOf("0") != -1 || name.contains("novice") || name.contains("master"))) {
			jediPoints += skill->getSkillPointsRequired();

			if (name.indexOf("4") != -1) {
				++jediFullTrees;
			}
		}
	}

	for (int i = 0; i < orderedSkillNames.size(); ++i) {
		const String& name = orderedSkillNames.get(i);
		Skill* skill = nullptr;

		// These skills can alter progression/FRS state in addition to the skill list.
		if (name.beginsWith("force_title_") || name.beginsWith("force_rank_")) {
			return false;
		}

		for (int j = 0; j < ownedSkills->size(); ++j) {
			Skill* candidate = ownedSkills->get(j);

			if (candidate->getSkillName() == name) {
				skill = candidate;
				break;
			}
		}

		if (skill == nullptr || orderedSkills.contains(skill)) {
			return false;
		}

		orderedSkills.add(skill);
	}

	for (int i = 0; i < orderedSkills.size(); ++i) {
		Skill* skill = orderedSkills.get(i);
		const String& name = skill->getSkillName();

		if (name.contains("force_discipline_")) {
			jediPoints -= skill->getSkillPointsRequired();

			if (name.indexOf("4") != -1) {
				--jediFullTrees;
			}
		}

		if (name.beginsWith("force_")) {
			Lua* lua = DirectorManager::instance()->getLuaInstance();
			Reference<LuaFunction*> check = lua->createFunction(getJediManagerName(), "canSurrenderSkillInBatch", 1);
			*check << creature;
			*check << name;
			*check << forceSensitiveCount;
			*check << jediPoints;
			*check << jediFullTrees;

			lua_State* L = check->callFunction();
			const bool allowed = lua_toboolean(L, -1);
			lua_pop(L, 1);

			if (!allowed) {
				return false;
			}
		}

		// The existing Village rule checks the count BEFORE surrendering even a
		// zero-point novice/master box, so update this only after checking each step.
		if (name.contains("force_sensitive") && name.indexOf("0") != -1) {
			--forceSensitiveCount;
		}
	}

	return true;
}

void JediManager::onFSTreeCompleted(CreatureObject* creature, const String& branch) {
	Lua* lua = DirectorManager::instance()->getLuaInstance();
	Reference<LuaFunction*> luaStartTask = lua->createFunction(getJediManagerName(), "onFSTreeCompleted", 0);
	*luaStartTask << creature;
	*luaStartTask << branch;

	luaStartTask->callFunction();
}
