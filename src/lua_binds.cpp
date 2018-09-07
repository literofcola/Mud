#include "CGame.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CCharacter.h"
#include "CPlayer.h"
#include "CUser.h"
#include "CSpellAffect.h"
#include "CRoom.h"
#include "CSkill.h"
#include "utils.h"
#include "mud.h"

#define SOL_CHECK_ARGUMENTS 1
#define SOL_USING_CXX_LUA
#include <sol.hpp>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

void Lua_DefineFunctions(sol::state * lua)
{
	(*lua)["ExperienceForLevel"] = Game::ExperienceForLevel; //static int ExperienceForLevel(int level);
	lua->set("LevelDifficulty", Game::LevelDifficulty); //static int LevelDifficulty(int level1, int level2);
	lua->set_function("LoadNPCRoom", Game::LoadNPCRoom); //LoadNPCRoom(int id, Room * toroom);
	lua->set_function("cmd_cast", cmd_cast); //cmd_cast(Player * ch, string argument);
	lua->set_function("cmd_look", cmd_look); //cmd_look(Player * ch, string argument);
	lua->set_function("FlagIsSet", Utilities::FlagIsSet); //bool FlagIsSet(std::vector<int> & flags, const int flag)
}

void Lua_DefineClasses(sol::state * lua)
{
		(*lua).new_usertype<Subscriber>("Subscriber");

		(*lua).new_usertype<SubscriberManager>("SubscriberManager");

		/*
		(*lua).new_usertype<Server>("Server", 
			sol::constructors<Server(Game *, int)>()
			);

		(*lua).new_usertype<Client>("Client",
			sol::constructors<Client(SOCKET, std::string)>()
			);
		*/

		(*lua).new_usertype<Game>("Game",
			"GetGame", &Game::GetGame,
			"GetSkill", (Skill *(Game::*)(int)) &Game::GetSkill
			);

		(*lua).new_usertype<User>("User",
			sol::constructors<User(std::shared_ptr<Client>)>()
			);

		(*lua).new_usertype<Character>("Character",
			//sol::base_classes, sol::bases<Subscriber, SubscriberManager>(),
			//sol::constructors<Character(), Character(std::string, User*)>(),
			"GetLevel",  &Character::GetLevel,
			"Send", (void(Character::*)(std::string)) &Character::Send,
			"Message", &Character::Message,
			"AddSpellAffect", &Character::AddSpellAffect,
			"CleanseSpellAffect", &Character::CleanseSpellAffect,
			"GetHealth", &Character::GetHealth,
			"GetMana", &Character::GetMana,
			"GetMaxMana", &Character::GetMaxMana,
			"GetMaxHealth", &Character::GetMaxHealth,
			"HasResource", &Character::HasResource,
			"OneHit", &Character::OneHit,
			"OneHeal", &Character::OneHeal,
			"AdjustHealth", &Character::AdjustHealth,
			"AdjustMana", &Character::AdjustMana,
			"AdjustRage", &Character::AdjustRage,
			"ConsumeMana", &Character::ConsumeMana,
			"ConsumeEnergy", &Character::ConsumeEnergy,
			"ConsumeRage", &Character::ConsumeRage,
			"GenerateComboPoint", &Character::GenerateComboPoint,
			"SpendComboPoints", &Character::SpendComboPoints,
			"GetMainhandDamagePerSecond", &Character::GetMainhandDamagePerSecond,
			"GetOffhandDamagePerSecond", &Character::GetOffhandDamagePerSecond,
			"GetName", &Character::GetName,
			"HisHer", &Character::HisHer,
			"HimHer", &Character::HisHer,
			"HisHers", &Character::HisHer,
			"EnterCombat", &Character::EnterCombat,
			"EnterCombatAssist", &Character::EnterCombatAssist,
			"CanAttack", &Character::CanAttack,
			"IsFighting", &Character::IsFighting,
			"InCombat", &Character::InCombat,
			"IsAlive", &Character::IsAlive,
			"IsNPC", &Character::IsNPC,
			"GetThreat", &Character::GetThreat,
			"UpdateThreat", &Character::UpdateThreat,
			"SetCooldown", &Character::SetCooldown,
			"ChangeRoomsID", &Character::ChangeRoomsID,
			"GetAuraModifier", &Character::GetAuraModifier,
			"GetTarget", &Character::GetTarget,
			"GetRoom", &Character::GetRoom
			//"level", &Character::level,
			//"flags", &Character::flags
			);

		(*lua).new_usertype<Room>("Room",
			"characters", &Room::characters,
			"id", &Room::id,
			"name", &Room::name,
			"description", &Room::description
			);

		(*lua).new_usertype<Player>("Player",
			"GetClassLevel", &Player::GetClassLevel,
			"recall", &Player::recall,
			"GetIntellect", &Player::GetIntellect,
			"GetStrength", &Player::GetStrength,
			"SetLevel", &Player::SetLevel
			);

		(*lua).new_usertype<Skill>("Skill",
			sol::constructors<Skill(int, std::string)>()
			);

		(*lua).new_usertype<SpellAffect>("SpellAffect",
			//sol::base_classes, sol::bases<Subscriber>(),
			sol::constructors<SpellAffect()>(),
			"ApplyAura", (void(SpellAffect::*)(int, int)) &SpellAffect::ApplyAura,
			"RemoveAura", &SpellAffect::RemoveAura,
			"name", &SpellAffect::name,
			"GetCasterName", &SpellAffect::GetCasterName,
			"SaveDataInt", &SpellAffect::SaveDataInt,
			"SaveDataDouble", &SpellAffect::SaveDataDouble,
			"SaveDataString", &SpellAffect::SaveDataString,
			"GetDataInt", &SpellAffect::GetDataInt,
			"GetDataDouble", &SpellAffect::GetDataDouble,
			"GetDataString", &SpellAffect::GetDataString
			);
}
