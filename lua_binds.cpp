#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
typedef boost::shared_ptr<Client> Client_ptr;
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
typedef boost::shared_ptr<Server> Server_ptr;
#include "utils.h"
#include "mud.h"

using namespace luabind;

//luabind::call_function<void>(server->luaState, "setlevel", user->character, user->character->level+1);

void Lua_DefineFunctions(lua_State * ls)
{
    module(ls) 
    [
        def("ExperienceForLevel", Game::ExperienceForLevel), //static int ExperienceForLevel(int level);
        def("LevelDifficulty", Game::LevelDifficulty), //static int LevelDifficulty(int level1, int level2);
        def("LoadNPCRoom", Game::LoadNPCRoom), //LoadNPCRoom(int id, Room * toroom);
        def("cmd_cast", cmd_cast), //cmd_cast(Character * ch, string argument);
		def("cmd_look", cmd_look) //cmd_look(Character * ch, string argument);
    ];
}

void Lua_DefineClasses(lua_State * ls)
{
    module(ls) 
    [
        class_<Listener>("Listener"),
        class_<ListenerManager>("ListenerManager"),
		
        class_<Server>("Server")
        .def(constructor<Game *, int>())
        ,
        class_<Client>("Client")
        .def(constructor<SOCKET, std::string>())
        ,
        class_<User>("User")
        .def(constructor<Client *>())
        ,
        class_<Game>("Game")
        .def("GetGame", Game::GetGame)
        ,
        class_<Character, bases<Listener, ListenerManager> >("Character")
        .def(constructor<>())
		//.def(constructor<std::string, int>())
        .def(constructor<std::string, User*>())
		//.def(constructor<const Character&>())
        .def("GetLevel", (int(Character::*)(void)) &Character::GetLevel)
        .def("SetLevel", (void(Character::*)(int)) &Character::SetLevel)
        .def("Send", (void(Character::*)(std::string)) &Character::Send)
        .def("Message", (void(Character::*)(const std::string &, Character::MessageType, Character*)) &Character::Message)
        .def("AddSpellAffect", (SpellAffect*(Character::*)(int, Character*, std::string, bool, bool, int, double, int, Skill*)) &Character::AddSpellAffect)
        .def("CleanseSpellAffect", (int(Character::*)(Character*, int, int)) &Character::CleanseSpellAffect)
        .def("GetHealth", (int(Character::*)(void)) &Character::GetHealth)
        .def("GetMana", (int(Character::*)(void)) &Character::GetMana)
        .def("GetStamina", (int(Character::*)(void)) &Character::GetStamina)
        .def("HasResource", (bool(Character::*)(int, int)) &Character::HasResource)
        .def("AdjustHealth", (void(Character::*)(Character*, int)) &Character::AdjustHealth)
        .def("AdjustMana", (void(Character::*)(Character*, int)) &Character::AdjustMana)
        .def("ConsumeMana", (void(Character::*)(int)) &Character::ConsumeMana) 
        .def("AdjustStamina", (void(Character::*)(Character*, int)) &Character::AdjustStamina)
        .def("GetName", (std::string(Character::*)(void)) &Character::GetName)
        .def("HisHer", (std::string(Character::*)(void)) &Character::HisHer)
        .def("EnterCombat", (void(Character::*)(Character*)) &Character::EnterCombat)
        .def("IsFighting", (bool(Character::*)(Character*)) &Character::IsFighting)
        .def("InCombat", (bool(Character::*)(void)) &Character::InCombat)
        .def("SetCooldown", (void(Character::*)(Skill*, std::string, bool, double)) &Character::SetCooldown)
		.def("ChangeRoomsID", (bool(Character::*)(int)) &Character::ChangeRoomsID)
		.def("GetPlayer", (Player*(Character::*)(void)) &Character::GetPlayer)
        .def("GetAuraModifier", (int(Character::*)(int, int)) &Character::GetAuraModifier)
        .def("GetTarget", (Character*(Character::*)(void)) &Character::GetTarget)
        .def_readwrite("level", &Character::level)
        ,
        class_<Room>("Room")
        .def_readonly("id", &Room::id)
        .def_readonly("name", &Room::name)
        .def_readonly("description", &Room::description)
        ,
		class_<Player>("Player")
        .def("GetClassLevel", (int(Player::*)(int)) &Player::GetClassLevel)
		.def_readonly("recall", &Player::recall)
		,
        class_<Skill>("Skill")
        .def(constructor<int, std::string>())
        ,
        class_<SpellAffect, Listener>("SpellAffect")
        .def(constructor<>())
        .def("ApplyAura", (void(SpellAffect::*)(int, int)) &SpellAffect::ApplyAura)
        .def("GetCasterName", (std::string(SpellAffect::*)(void)) &SpellAffect::GetCasterName)
        .def("SaveDataInt", (void(SpellAffect::*)(std::string, int)) &SpellAffect::SaveDataInt)
        .def("SaveDataDouble", (void(SpellAffect::*)(std::string, double)) &SpellAffect::SaveDataDouble)
        .def("SaveDataString", (void(SpellAffect::*)(std::string, std::string)) &SpellAffect::SaveDataString)
        .def("GetDataInt", (int(SpellAffect::*)(std::string)) &SpellAffect::GetDataInt)
        .def("GetDataDouble", (double(SpellAffect::*)(std::string)) &SpellAffect::GetDataDouble)
        .def("GetDataString", (std::string(SpellAffect::*)(std::string)) &SpellAffect::GetDataString)
    ];
}
