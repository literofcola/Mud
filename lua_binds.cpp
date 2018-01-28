#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CItem.h"
#include "CSkill.h"
#include "CClass.h"
#include "CExit.h"
#include "CReset.h"
#include "CArea.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "utils.h"
#include "mud.h"

void Lua_DefineFunctions(sol::state * lua)
{
	(*lua)["ExperienceForLevel"] = Game::ExperienceForLevel; //static int ExperienceForLevel(int level);
	lua->set("LevelDifficulty", Game::LevelDifficulty); //static int LevelDifficulty(int level1, int level2);
	lua->set_function("LoadNPCRoom", Game::LoadNPCRoom); //LoadNPCRoom(int id, Room * toroom);
	lua->set_function("cmd_cast", cmd_cast); //cmd_cast(Character * ch, string argument);
	lua->set_function("cmd_look", cmd_look); //cmd_look(Character * ch, string argument);
}

void Lua_DefineClasses(sol::state * lua)
{
		(*lua).new_usertype<Listener>("Listener");

		(*lua).new_usertype<ListenerManager>("ListenerManager");

		/*
		(*lua).new_usertype<Server>("Server", 
			sol::constructors<Server(Game *, int)>()
			);

		(*lua).new_usertype<Client>("Client",
			sol::constructors<Client(SOCKET, std::string)>()
			);
		*/

		(*lua).new_usertype<Game>("Game",
			"GetGame", &Game::GetGame
			);

		(*lua).new_usertype<User>("User",
			sol::constructors<User(std::shared_ptr<Client>)>()
			);

		(*lua).new_usertype<Character>("Character",
			//sol::base_classes, sol::bases<Listener, ListenerManager>(),
			sol::constructors<Character(), Character(std::string, User*)>(),
			"GetLevel",  &Character::GetLevel,
			"SetLevel",  &Character::SetLevel,
			"Send", (void(Character::*)(std::string)) &Character::Send,
			"Message", &Character::Message,
			"AddSpellAffect", &Character::AddSpellAffect,
			"CleanseSpellAffect", &Character::CleanseSpellAffect,
			"GetHealth", &Character::GetHealth,
			"GetMana", &Character::GetMana,
			"HasResource", &Character::HasResource,
			"AdjustHealth", &Character::AdjustHealth,
			"AdjustMana", &Character::AdjustMana,
			"ConsumeMana", &Character::ConsumeMana,
			"GetName", &Character::GetName,
			"HisHer", &Character::HisHer,
			"EnterCombat", &Character::EnterCombat,
			"IsFighting", &Character::IsFighting,
			"InCombat", &Character::InCombat,
			"SetCooldown", &Character::SetCooldown,
			"ChangeRoomsID", &Character::ChangeRoomsID,
			"GetPlayer", &Character::GetPlayer,
			"GetAuraModifier", &Character::GetAuraModifier,
			"GetTarget", &Character::GetTarget,
			"level", &Character::level
			);

		(*lua).new_usertype<Room>("Room",
			"id", &Room::id,
			"name", &Room::name,
			"description", &Room::description
			);

		(*lua).new_usertype<Player>("Player",
			"GetClassLevel", &Player::GetClassLevel,
			"recall", &Player::recall
			);

		(*lua).new_usertype<Skill>("Skill",
			sol::constructors<Skill(int, std::string)>()
			);

		(*lua).new_usertype<SpellAffect>("SpellAffect",
			//sol::base_classes, sol::bases<Listener>(),
			sol::constructors<SpellAffect()>(),
			"ApplyAura", (void(SpellAffect::*)(int, int)) &SpellAffect::ApplyAura,
			"GetCasterName", &SpellAffect::GetCasterName,
			"SaveDataInt", &SpellAffect::SaveDataInt,
			"SaveDataDouble", &SpellAffect::SaveDataDouble,
			"SaveDataString", &SpellAffect::SaveDataString,
			"GetDataInt", &SpellAffect::GetDataInt,
			"GetDataDouble", &SpellAffect::GetDataDouble,
			"GetDataString", &SpellAffect::GetDataString
			);
}
