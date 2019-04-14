#include "stdafx.h"
#include "CGame.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CCharacter.h"
#include "CPlayer.h"
#include "CNPC.h"
#include "CUser.h"
#include "CSpellAffect.h"
#include "CGroup.h"
#include "CRoom.h"
#include "CExit.h"
#include "CSkill.h"
#include "utils.h"
#include "mud.h"

void Lua_DefineFunctions(sol::state * lua)
{
	(*lua)["ExperienceForLevel"] = Game::ExperienceForLevel; //static int ExperienceForLevel(int level);
	lua->set("LevelDifficulty", Game::LevelDifficulty); //static int LevelDifficulty(int level1, int level2);
	lua->set_function("FlagIsSet", Utilities::FlagIsSet); //bool FlagIsSet(std::vector<int> & flags, const int flag)
    lua->set_function("FindDirection", (Exit::Direction(*)(Character *, Character *, int))FindDirection);

	Server::lua.script(R"(
						function getinfowrapper(whichfunc)
						  local info = debug.getinfo(whichfunc)
						  return info.short_src, info.linedefined, info.lastlinedefined
						end
					)");
}

void Lua_DefineClasses(sol::state * lua)
{
		(*lua).new_usertype<Subscriber>("Subscriber");

		(*lua).new_usertype<SubscriberManager>("SubscriberManager");

        (*lua).new_usertype<Exit>("Exit");

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
			"GetSkill", (Skill *(Game::*)(int)) &Game::GetSkill,
            "LoadNPCRoom", &Game::LoadNPCRoom   //LoadNPCRoom(int id, Room * toroom);
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
            "RemoveSpellAffect", (void(Character::*)(bool, std::string))&Character::RemoveSpellAffect,
			"HasSpellAffect", &Character::HasSpellAffect,
			"GetHealth", &Character::GetHealth,
			"GetMana", &Character::GetMana,
			"GetMaxMana", &Character::GetMaxMana,
			"GetMaxHealth", &Character::GetMaxHealth,
            "ResetMaxStats", &Character::ResetMaxStats,
			"HasResource", &Character::HasResource,
			"GetIntellect", &Character::GetIntellect,
			"GetStrength", &Character::GetStrength,
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
			"GetName", &Character::GetName,
			"HisHer", &Character::HisHer,
			"HimHer", &Character::HisHer,
			"HisHers", &Character::HisHer,
            "CancelActiveDelay", &Character::CancelActiveDelay,
            "GetDelaySkill", &Character::GetDelaySkill,
            "HasActiveDelay", &Character::HasActiveDelay,
			"EnterCombat", &Character::EnterCombat,
			"EnterCombatAssist", &Character::EnterCombatAssist,
            "CancelAutoAttack", &Character::CancelAutoAttack,
			"CanAttack", &Character::CanAttack,
            "CanHeal", &Character::CanHeal,
			"DoAttackRoll", &Character::DoAttackRoll,
			"CalculateArmorMitigation", &Character::CalculateArmorMitigation,
            "HandleDamageAbsorb", &Character::HandleDamageAbsorb,
			"IsFighting", &Character::IsFighting,
			"InCombat", &Character::InCombat,
			"IsAlive", &Character::IsAlive,
            "IsGhost", &Character::IsGhost,
			"IsNPC", &Character::IsNPC,
			"IsPlayer", &Character::IsPlayer,
			"AsPlayer", &Character::AsPlayer,
			"GetThreatValue", &Character::GetThreatValue,
            "GetNthHighestThreatData", &Character::GetNthHighestThreatData,
			"UpdateThreat", &Character::UpdateThreat,
            "HasTap", &Character::HasTap,
            "IsTapped", &Character::IsTapped,
			"SetCooldown", &Character::SetCooldown,
            "GetCooldownRemaining", &Character::GetCooldownRemaining,
			"ChangeRoomsID", &Character::ChangeRoomsID,
            "Move", &Character::Move,
			"GetAuraModifier", &Character::GetAuraModifier,
			"GetTarget", &Character::GetTarget,
            "SetTarget", &Character::SetTarget,
			"GetRoom", &Character::GetRoom,
			"GetRecall", &Character::GetRecall,
			"GetMainhandWeaponSpeed", &Character::GetMainhandWeaponSpeed,
			"GetOffhandWeaponSpeed", &Character::GetOffhandWeaponSpeed,
			"GetMainhandDamagePerSecond", &Character::GetMainhandDamagePerSecond,
			"GetOffhandDamageRandomHit", &Character::GetOffhandDamageRandomHit,
			"GetOffhandDamagePerSecond", &Character::GetOffhandDamagePerSecond,
			"GetMainhandDamageRandomHit", &Character::GetMainhandDamageRandomHit,
			"GetCharacterRoom", (Character *(Character::*)(Character *)) &Character::GetCharacterRoom,
			"GetCharacterAdjacentRoom", (Character *(Character::*)(Character *)) &Character::GetCharacterAdjacentRoom,
            "GetGroup", &Character::GetGroup,
			"Cast", (void(Character::*)(std::string)) &Character::Cast,
            "CastByID", (void(Character::*)(int, Character *)) &Character::CastByID,
			"Look", &Character::Look
			//"level", &Character::level,
			//"flags", &Character::flags
			);

		(*lua).new_usertype<NPC>("NPC",
			sol::base_classes, sol::bases<Character>()
			);

		(*lua).new_usertype<Player>("Player",
			sol::base_classes, sol::bases<Character>(),
            "ResetMaxStats", &Player::ResetMaxStats,
			"GetClassLevel", &Player::GetClassLevel,
			"SetLevel", &Player::SetLevel,
            "LuaAddQuery", &Player::LuaAddQuery
            );

        (*lua).new_usertype<Character::Threat>("Threat",
            "ch", &Character::Threat::ch,
            "threat", &Character::Threat::threat,
            "damage", &Character::Threat::damage,
            "healing", &Character::Threat::healing,
            "tapped", &Character::Threat::tapped //This 'ch' has a valid "tap" on us
            );

		(*lua).new_usertype<Room>("Room",
			"characters", &Room::characters,
			"id", &Room::id,
			"name", &Room::name,
			"description", &Room::description,
            "AddSpellAffect", &Room::AddSpellAffect,
            "CleanseSpellAffect", &Room::CleanseSpellAffect,
            "RemoveSpellAffect", (void(Room::*)(std::string))&Room::RemoveSpellAffect,
            "HasSpellAffect", &Room::HasSpellAffect
			);

		(*lua).new_usertype<Skill>("Skill",
			sol::constructors<Skill(int, std::string)>(),
            "GetShortName", &Skill::GetShortName,
            "GetLongName", &Skill::GetLongName
			);

		(*lua).new_usertype<SpellAffect>("SpellAffect",
			//sol::base_classes, sol::bases<Subscriber>(),
			sol::constructors<SpellAffect()>(),
			"ApplyAura", (void(SpellAffect::*)(int, int)) &SpellAffect::ApplyAura,
			"RemoveAura", &SpellAffect::RemoveAura,
            "GetAuraModifier", &SpellAffect::GetAuraModifier,
            "HasAura", &SpellAffect::HasAura,
			"name", &SpellAffect::name,
            "GetCaster", &SpellAffect::GetCaster,
			"GetCasterName", &SpellAffect::GetCasterName,
			"SaveDataInt", &SpellAffect::SaveDataInt,
			"SaveDataDouble", &SpellAffect::SaveDataDouble,
			"SaveDataString", &SpellAffect::SaveDataString,
			"GetDataInt", &SpellAffect::GetDataInt,
			"GetDataDouble", &SpellAffect::GetDataDouble,
			"GetDataString", &SpellAffect::GetDataString
			);

        (*lua).new_usertype<Group>("Group",
            //"MAX_RAID_SIZE", &Group::MAX_RAID_SIZE,
            "GetMember", &Group::GetMember
            );
}
