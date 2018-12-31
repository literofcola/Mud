#include "stdafx.h"
#include "mud.h"
#include "CCharacter.h"
#include "CGame.h"
#include "CServer.h"
#include "CPlayer.h"
#include "CNPCIndex.h"
#include "CNPC.h"
#include "CReset.h"
#include "CTrigger.h"
#include "CRoom.h"
#include "CItem.h"
#include "CQuest.h"
#include "CSkill.h"
#include "CClass.h"
#include "CArea.h"
#include "CHelp.h"
#include "utils.h"
#include "CLogFile.h"

void cmd_edit(Player * ch, std::string argument)
{
	std::string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
	//argument = Utilities::one_argument(argument, arg3);
	
	if(!Utilities::str_cmp(arg1, "room"))
	{
		if (!Utilities::str_cmp("create", arg2))
		{
			ch->editData = Game::GetGame()->CreateRoomAnyID();
			ch->editState = Player::ED_ROOM;
			ch->Send("Room " + Utilities::itos(((Room*)(ch->editData))->id) + " created.\r\n");
			return;
		}

		if(ch && ch->room)
		{
            ch->editState = Player::ED_ROOM;
			ch->editData = ch->room;
            ch->Send("Ok.\r\n");
		}
		else
		{
            LogFile::Log("error", "cmd_edit, (user->character == nullptr || user->character->room == nullptr) with no room id argument");
			ch->editData = nullptr;
		}
	}
    else if(!Utilities::str_cmp(arg1, "skill"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit skill create <name>\r\n");
                return;
            }
            
            ch->editData = Game::GetGame()->CreateSkillAnyID(arg3);
			if (ch->editData == nullptr)
			{
				ch->Send("Error: skill with that name already exists\r\n");
				return;
			}
			ch->editState = Player::ED_SKILL;
            ch->Send("Ok.\r\n");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Skill * sk = Game::GetGame()->GetSkill(Utilities::atoi(arg2));
            if(sk == nullptr)
            {
                ch->Send("Skill not found.\r\n");
                return;
            }
            ch->editState = Player::ED_SKILL;
            ch->editData = sk;
            ch->Send("Ok.\r\n");
        }
		else if (!Utilities::str_cmp("luadebug", arg2))
		{
			//prints debug.getinfo(): short_src, linedefined, lastlinedefined
			std::string func_name[5] = { "_cost", "_cast", "_apply", "_tick", "_remove" };

			sol::function getinfowrapper = Server::lua["getinfowrapper"];
			std::tuple<std::string, int, int> return_values;
			sol::protected_function_result result;
			sol::protected_function skill_func;
			for (auto iter = Game::GetGame()->skills.begin(); iter != Game::GetGame()->skills.end(); ++iter)
			{
				for (int i = 0; i <= 4; ++i)
				{
					try
					{
						skill_func = Server::lua[iter->second->function_name + func_name[i]];
						if (skill_func == sol::nil)
							continue;
						result = getinfowrapper(skill_func);
						if (!result.valid())
						{
							sol::error err = result;
							std::string what = err.what();
							LogFile::Log("error", "getinfo call failed, sol::error::what() is: " + what);
						}
						return_values = result;
						ch->Send(std::get<0>(return_values) + "\r\n");
						ch->Send(Utilities::itos(std::get<1>(return_values)) + "\r\n");
						ch->Send(Utilities::itos(std::get<2>(return_values)) + "\r\n");

					}
					catch (const std::exception & e)
					{
						LogFile::Log("error", e.what());
					}
				}
			}
		}
        else
        {
            ch->Send("Syntax: edit skill <id> || edit skill create <name>\r\n");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "npc"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit npc create <name>\r\n");
                return;
            }
            ch->editState = Player::ED_NPC;
            ch->editData = Game::GetGame()->CreateNPCAnyID(arg3);
            ch->Send("Ok.\r\n");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            NPCIndex * tch = Game::GetGame()->GetNPCIndex(Utilities::atoi(arg2));
            if(tch == nullptr)
            {
                ch->Send("NPC " + arg2 + " not found.\r\n");
                return;
            }
            ch->editState = Player::ED_NPC;
            ch->editData = tch;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit npc <id> || edit npc create <name>\r\n");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "item"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit item create <name>\r\n");
                return;
            }
            ch->editState = Player::ED_ITEM;
            ch->editData = Game::GetGame()->CreateItemAnyID(arg3);
            ch->Send("Ok.\r\n");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Item * item = Game::GetGame()->GetItem(Utilities::atoi(arg2));
            if(item == nullptr)
            {
                ch->Send("Item " + arg2 + " not found.\r\n");
                return;
            }
            ch->editState = Player::ED_ITEM;
            ch->editData = item;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit item <id> || edit item create <name>\r\n");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "quest"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit quest create <name>\r\n");
                return;
            }
            ch->editState = Player::ED_QUEST;
            ch->editData = Game::GetGame()->CreateQuestAnyID(arg3);
            ch->Send("Ok.\r\n");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Quest * q = Game::GetGame()->GetQuest(Utilities::atoi(arg2));
            if(q == nullptr)
            {
                ch->Send("Quest " + arg2 + " not found.\r\n");
                return;
            }
            ch->editState = Player::ED_QUEST;
            ch->editData = q;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit quest <id> || edit quest create <name>\r\n");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "area"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit area create <name>\r\n");
                return;
            }
            ch->editState = Player::ED_AREA;
            ch->editData = Game::GetGame()->CreateAreaAnyID(arg3);
            ch->Send("Ok.\r\n");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Area * a = Game::GetGame()->GetArea(Utilities::atoi(arg2));
            if(a == nullptr)
            {
                ch->Send("Area " + arg2 + " not found.\r\n");
                return;
            }
            ch->editState = Player::ED_AREA;
            ch->editData = a;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit area <id> || edit area create <name>\r\n");
            return;
        }
    }
	else if(!Utilities::str_cmp(arg1, "help"))
	{
		if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit help create <name>\r\n");
                return;
            }
            ch->editState = Player::ED_HELP;
            ch->editData = Game::GetGame()->CreateHelpAnyID(arg3);
            ch->Send("Ok.\r\n");
        }
		else if(Utilities::IsNumber(arg2))
        {  
			Help * h = Game::GetGame()->GetHelp(Utilities::atoi(arg2));
            if(h == nullptr)
            {
                ch->Send("Help topic '" + arg2 + "' not found.\r\n");
                return;
            }
            ch->editState = Player::ED_HELP;
            ch->editData = h;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit help <id> || edit help create <name>\r\n");
            return;
        }
	}
    else if(!Utilities::str_cmp(arg1, "class"))
    {
        /*if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit class create <name>\r\n");
                return;
            }
            user->editState = User::ED_QUEST;
            user->editData = Game::GetGame()->CreateQuestAnyID(arg3);
            ch->Send("Ok.\r\n");
        }*/
        //else if(Utilities::IsNumber(arg2))
        if(Utilities::IsNumber(arg2))
        {  
            Class * c = Game::GetGame()->GetClass(Utilities::atoi(arg2));
            if(c == nullptr)
            {
                ch->Send("Class " + arg2 + " not found.\r\n");
                return;
            }
            ch->editState = Player::ED_CLASS;
            ch->editData = c;
            ch->Send("Ok.\r\n");
        }
        else
        {
            ch->Send("Syntax: edit class <id>\r\n");
            return;
        }
    }
	else if(!Utilities::str_cmp(arg1, "player"))
	{
        Player *vch;
        vch = Game::GetGame()->GetPlayerByName(arg2);
        //arg2 = ReplaceApostrophe(arg2);
        if(vch == nullptr)// && server->sqlQueue->Read("select name from players where name='" + arg2 + "'").empty())
        {
            ch->Send("That player is not online.\r\n");
            return;
        }
        //have to get creative to ensure we're not trying to edit bad data if a player quits
        ch->editData = (void*)(new std::string(arg2.c_str()));
        ch->editState = Player::ED_PLAYER;
        ch->Send("Ok.\r\n");
        //todo
		//to edit a player, the name must be specified: edit player <name>
		//if the player is online, fill a new character/playerdata structure with that info
		// this is now editable
		//when saving, if player is online, change values immediately
		// if player is offline, overwrite the save file
	}
	else if(!Utilities::str_cmp(arg1, "exit"))
	{
		ch->Send("Exiting editor.\r\n");
        if(ch->editState == Player::ED_PLAYER) //special cleanup for players
        {
            std::string * deleteme = (std::string *)ch->editData;
            delete deleteme;
        }
        ch->editState = Player::ED_NONE;
		ch->editData = nullptr;
	}
    else if(!Utilities::str_cmp(arg1, "save"))
	{
		switch(ch->editState)
		{
			case Player::ED_NONE:
				ch->Send("You're not currently editing anything.\r\n");
				break;
            case Player::ED_SKILL:
                Game::GetGame()->SaveSkills();
                ch->Send("Skills saved.\r\n");
                break;
			case Player::ED_ROOM:
				Game::GetGame()->SaveRooms();
				ch->Send("Rooms saved.\r\n");
				break;
            case Player::ED_NPC:
                Game::GetGame()->SaveNPCIndex();
                ch->Send("NPCs saved.\r\n");
                break;
            case Player::ED_ITEM:
                Game::GetGame()->SaveItems();
                ch->Send("Items saved.\r\n");
                break;
            case Player::ED_QUEST:
                Game::GetGame()->SaveQuests();
                ch->Send("Quests saved.\r\n");
                break;
            case Player::ED_AREA:
                Game::GetGame()->SaveAreas();
                ch->Send("Areas saved.\r\n");
                break;
            case Player::ED_CLASS:
                Game::GetGame()->SaveClasses();
                ch->Send("Classes saved.\r\n");
                break;
			case Player::ED_HELP:
				if(((Help*)ch->editData)->remove)
				{
					ch->editState = Player::ED_NONE;
					ch->editData = nullptr;
				}
                Game::GetGame()->SaveHelp();
                ch->Send("Help saved.\r\n");
                break;
            case Player::ED_PLAYER:
				break;
		}
	}
	else if (!Utilities::str_cmp(arg1, "reloadlua"))
	{
        try 
        {
            Server::lua.script_file("lua_constants.lua");
            Server::lua.script_file("lua_skills.lua");
            ch->Send("Reloaded files: lua_constants.lua, lua_skills.lua\r\n");
        }
        catch (const std::exception & e)
        {
            LogFile::Log("error", e.what());
        }
	}
    else if (!Utilities::str_cmp(arg1, "importall"))
    {
        for (auto iter = Game::GetGame()->skills.begin(); iter != Game::GetGame()->skills.end(); ++iter)
        {
            Skill * pSkill = iter->second;

            std::string func_name[5] = { "_cost", "_cast", "_apply", "_tick", "_remove" };
            sol::function getinfowrapper = Server::lua["getinfowrapper"];
            std::tuple<std::string, int, int> return_values;
            sol::protected_function_result result;
            sol::protected_function skill_func;

            for (int i = 0; i <= 4; ++i)
            {
                try
                {
                    skill_func = Server::lua[pSkill->function_name + func_name[i]];
                    if (skill_func == sol::nil)
                        continue;
                    result = getinfowrapper(skill_func);
                    if (!result.valid())
                    {
                        sol::error err = result;
                        std::string what = err.what();
                        LogFile::Log("error", "getinfo call failed, sol::error::what() is: " + what);
                    }
                    return_values = result;
                }
                catch (const std::exception & e)
                {
                    LogFile::Log("error", e.what());
                }

                std::string short_src = std::get<0>(return_values);
                if (short_src == "lua_skills.lua")
                {
                    ch->Send(pSkill->function_name + func_name[i] + " found in lua_skills.lua\r\n");
                    std::string variable_lookup[5] = { "cost_script", "cast_script", "apply_script", "tick_script", "remove_script" };
                    int first_line = std::get<1>(return_values);
                    int last_line = std::get<2>(return_values);
                    std::stringstream new_file;
                    std::fstream importfile("lua_skills.lua", std::fstream::in);
                    std::string one_line;
                    std::string imported_function;
                    for (int line_number = 1; !importfile.eof(); ++line_number)
                    {
                        std::getline(importfile, one_line);
                        if (line_number < first_line || line_number > last_line) //not part of the function we're looking for
                        {   //copy to new file
                            new_file << one_line << '\n';
                        }
                        else //import this to pSkill
                        {
                            imported_function += one_line + '\n';
                        }
                    }
                    importfile.close();
                    *(pSkill->stringTable[variable_lookup[i].c_str()]) = imported_function;
                    pSkill->changed = true;
                    importfile.open("lua_skills.lua", std::fstream::out | std::fstream::trunc);
                    importfile << new_file.str();
                    importfile.close();
                    
                    ch->Send("Reloading LUA scripts (overrides lua_skills.lua)\r\n");
                    try {
                        Server::lua.script(pSkill->costScript.c_str());
                        Server::lua.script(pSkill->castScript.c_str());
                        Server::lua.script(pSkill->applyScript.c_str());
                        Server::lua.script(pSkill->tickScript.c_str());
                        Server::lua.script(pSkill->removeScript.c_str());
                    }
                    catch (const std::exception & e)
                    {
                        LogFile::Log("error", e.what());
                    }

                    Server::lua.script_file("lua_skills.lua");
                    ch->Send("Reloaded file lua_skills.lua\r\n");
                }
            }
        }
    }
    else if (!Utilities::str_cmp(arg1, "exportall"))
    {
        for (auto iter = Game::GetGame()->skills.begin(); iter != Game::GetGame()->skills.end(); ++iter)
        {
            Skill * pSkill = iter->second;

            std::string func_name[5] = { "_cost", "_cast", "_apply", "_tick", "_remove" };
            sol::function getinfowrapper = Server::lua["getinfowrapper"];
            std::tuple<std::string, int, int> return_values;
            sol::protected_function_result result;
            sol::protected_function skill_func;

            for (int i = 0; i <= 4; ++i)
            {
                try
                {
                    skill_func = Server::lua[pSkill->function_name + func_name[i]];
                    if (skill_func == sol::nil)
                        continue;
                    result = getinfowrapper(skill_func);
                    if (!result.valid())
                    {
                        sol::error err = result;
                        std::string what = err.what();
                        LogFile::Log("error", "getinfo call failed, sol::error::what() is: " + what);
                    }
                    return_values = result;
                }
                catch (const std::exception & e)
                {
                    LogFile::Log("error", e.what());
                    continue;
                }

                std::string short_src = std::get<0>(return_values);
                if (short_src == "lua_skills.lua")
                {
                    ch->Send(pSkill->function_name + func_name[i] + " found in lua_skills.lua, unable to export\r\n");
                    continue;
                }
                std::fstream exportfile("lua_skills.lua", std::fstream::binary | std::fstream::out | std::fstream::app);
                std::string variable_lookup[5] = { "cost_script", "cast_script", "apply_script", "tick_script", "remove_script" };
                exportfile.write((pSkill->stringTable[variable_lookup[i].c_str()])->c_str(), pSkill->stringTable[variable_lookup[i].c_str()]->length());
                exportfile.close();
                ch->Send("exported " + pSkill->function_name + func_name[i] + "\r\n");
            }
            Server::lua.script_file("lua_skills.lua");
            ch->Send("Reloaded file lua_skills.lua\r\n");
        }
    }
	else
	{
		//this should be a help file
		std::string syntax = "";
		syntax += "Syntax: Edit: room\r\n";
        syntax += "              skill <id> || skill create <name>\r\n";
        syntax += "              npc <id> || npc create <name>\r\n";
        syntax += "              item <id> || item create <name>\r\n";
        syntax += "              quest <id> || quest create <name>\r\n";
        syntax += "              area <id> || area create <name>\r\n";
        syntax += "              class <id>\r\n";
		syntax += "              help <name> || help create <name>\r\n";
		syntax += "              player <name>\r\n";
		syntax += "              save\r\n";
		syntax += "              reloadlua\r\n";
		syntax += "              exit\r\n";
		ch->Send(syntax);
	}
}


void roomEditCmd_show(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

	if(!pRoom)
	{
        LogFile::Log("error", "roomEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("Name:      [" + pRoom->name + "]\r\n");
    ch->Send("ID:        [" + Utilities::itos(pRoom->id) + "]\r\n");
    ch->Send("Area:      [" + Utilities::itos(pRoom->area) + "]\r\n");

	ch->Send("Flags: ");
    std::vector<int>::iterator flagiter;
    for(flagiter = pRoom->flags.begin(); flagiter != pRoom->flags.end(); ++flagiter)
    {
        ch->Send(Room::flag_table[(*flagiter)].flag_name + " ");
    }
    ch->Send("\r\n");

	ch->Send("Exits:\r\n");
    for(int i = 0; i < Exit::DIR_LAST; i++)
	{
		if(pRoom->exits[i] && pRoom->exits[i]->to)
		{
            ch->Send("-" + std::string(Exit::exitNames[i]) + " to [" + Utilities::itos(pRoom->exits[i]->to->id) + "]\r\n");
		}
	}
	ch->Send("Description:\r\n" + pRoom->description + "\r\n");

	std::string characters = "Characters: [";
    int limit = 0;
    for(std::list<Character *>::iterator i = pRoom->characters.begin(); i != pRoom->characters.end(); ++i)
	{
        if(++limit > 10)
        {
            characters += "...";
            break;
        }
		characters += (*i)->GetName() + " ";
	}
	characters += "]\r\n";
	ch->Send(characters);

    //Resets
    if(!pRoom->resets.empty())
        ch->Send("Resets:\r\n");
    for(std::map<int, Reset*>::iterator iter = pRoom->resets.begin(); iter != pRoom->resets.end(); iter++)
    {
        Reset * r = (*iter).second;
        ch->Send(Utilities::itos(r->id) + ". " + (r->type == 1 ? "NPC " + Utilities::itos(r->targetID) : "ITEM " + Utilities::itos(r->targetID))
            + ", interval: " + Utilities::itos(r->interval) + ", wander_distance: " + Utilities::itos(r->wanderDistance)
            + ", leash_distance: " + Utilities::itos(r->leashDistance) + " " + (r->removeme ? "|RX|X" : "") + "\r\n");
    }

    //Triggers
    if(!pRoom->triggers.empty())
        ch->Send("Triggers:\r\n");
    for(std::map<int, Trigger>::iterator iter = pRoom->triggers.begin(); iter != pRoom->triggers.end(); iter++)
    {
        Trigger * t = &((*iter).second);
        ch->Send(Utilities::itos(t->id) + ". Type: " + Utilities::itos(t->GetType()) + " Argument: " + t->GetArgument() + 
                  "Function name: " + t->GetFunction() + "\r\n");
    }
}

void roomEditCmd_name(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

	if(argument.empty())
	{
		pRoom->name.clear();
		ch->Send("Name cleared.\r\n");
		return;
	}

	if(argument.length() > 50)
	{
		ch->Send("Maximum length is 50 characters.\r\n");
		return;
	}

	pRoom->name = argument;
    pRoom->changed = true;
	ch->Send("Name set.\r\n");
}

void roomEditCmd_area(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("area <#>\r\n");
        return;
    }
    int area = Utilities::atoi(arg1);
    if(area < 0)
    {
        ch->Send("area must be >= 0.\r\n");
        return;
    }
    pRoom->changed = true;
    pRoom->area = area;
}

void roomEditCmd_description(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    if(argument.empty() && ch->user)
    {
        StringEdit::string_append( ch->user, &pRoom->description );
        pRoom->changed = true;
        return;
    }

    ch->Send( "Syntax:  desc\r\n" );
}

void roomEditCmd_flag(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("flags: recall nopvp\r\n");
        return;
    }

    arg1 = Utilities::ToLower(arg1);

    for(int i = 0; Room::flag_table[i].flag != -1; i++)
    {
        if(!Utilities::str_cmp(Room::flag_table[i].flag_name, arg1))
        {
            if(Utilities::FlagIsSet(pRoom->flags, Room::flag_table[i].flag))
            {
                Utilities::FlagUnSet(pRoom->flags, Room::flag_table[i].flag);
                ch->Send(arg1 + " flag removed.\r\n");
            }
            else
            {
                Utilities::FlagSet(pRoom->flags, Room::flag_table[i].flag);
                ch->Send(arg1 + " flag set.\r\n");
            }
            pRoom->changed = true;
            return;
        }
    }
    ch->Send("flags: recall nopvp\r\n");
}

void roomEditCmd_reset(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    argument = Utilities::one_argument(argument, arg3);
	if (!Utilities::str_cmp(arg1, "force"))
	{
		Room * currRoom = pRoom;
		for (std::map<int, Reset*>::iterator resetiter = currRoom->resets.begin(); resetiter != currRoom->resets.end(); ++resetiter)
		{
			Reset * currReset = (*resetiter).second;
			if (!currReset->removeme)
			{
				if (currReset->type == 1 && currReset->npc == nullptr) //npc reset type and npc no longer exists
				{
					currReset->lastReset = Game::currentTime;
					//load it
					NPCIndex * charIndex = Game::GetGame()->GetNPCIndex(currReset->targetID);
					if (charIndex == nullptr)
					{
						LogFile::Log("error", "Reset " + Utilities::itos(currReset->id) + " in room " + Utilities::itos(currRoom->id) + ": npc does not exist.");
						continue;
					}
					NPC * newChar = Game::GetGame()->NewNPC(charIndex);
					newChar->leashOrigin = currRoom;
					newChar->ChangeRooms(currRoom);
					newChar->Message("|W" + newChar->GetName() + " has arrived.|X", Character::MSG_ROOM_NOTCHAR);
					newChar->reset = currReset;
					//LogFile::Log("status", "Adding subscriber to " + newChar->name + " of reset id " + Utilities::itos(currReset->id));
					newChar->AddSubscriber(currReset);
					currReset->npc = newChar;
				}
				else if (currReset->type == 2 && !currReset->inroom->HasItem(currReset->targetID))
				{
					currReset->lastReset = Game::currentTime;
					//load it
					Item * itemindex = Game::GetGame()->GetItem(currReset->targetID);
					if (itemindex == nullptr)
					{
						LogFile::Log("error", "Reset " + Utilities::itos(currReset->id) + " in room " + Utilities::itos(currRoom->id) + ": item " + Utilities::itos(currReset->targetID) + " does not exist.");
						continue;
					}
					currReset->inroom->AddItem(itemindex);
				}
			}
		}
		return;
	}
    if(Utilities::IsNumber(arg1))
    {
        int resetnum = Utilities::atoi(arg1);
        Reset * r = pRoom->GetReset(resetnum);
        if(r == nullptr)
        {
            ch->Send("Reset " + Utilities::itos(resetnum) + " not found.\r\n");
            return;
        }

        /*if(!Utilities::str_cmp(arg2, "type"))
        {
            if(!Utilities::str_cmp(arg3, "npc"))
            {
                r->type = 1;
                pRoom->changed = true;
                ch->Send("Reset " + Utilities::itos(resetnum) + " type changed to npc.\r\n");
                return;
            }
            else if(!Utilities::str_cmp(arg3, "obj"))
            {
                r->type = 2;
                pRoom->changed = true;
                ch->Send("Reset " + Utilities::itos(resetnum) + " type changed to obj.\r\n");
                return;
            }
            else
            {
                ch->Send("reset <#> type npc||obj\r\n");
                return;
            }
        }*/
        if(!Utilities::str_cmp(arg2, "target_id"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int newid = Utilities::atoi(arg3);
                if(r->type == 1) //npc
                {
                    NPCIndex * newindex = Game::GetGame()->GetNPCIndex(newid);
                    if(newindex != nullptr)
                    {
                        r->targetID = newid;
                        pRoom->changed = true;
                        return;
                    }
                    else
                    {
                        ch->Send("NPC with that id does not exist.\r\n");
                        return;
                    }
                }
                else if(r->type == 2) //obj
                {
					int newid = Utilities::atoi(arg3);
					if (r->type == 2) //obj
					{
						Item * newindex = Game::GetGame()->GetItem(newid);
						if (newindex != nullptr)
						{
							r->targetID = newid;
							pRoom->changed = true;
							return;
						}
						else
						{
							ch->Send("Item with that id does not exist.\r\n");
							return;
						}
					}
                }
            }
            else
            {
                ch->Send("reset <#> target_id <#id>\r\n");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "interval"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int newinterval = Utilities::atoi(arg3);
                if(newinterval < 0)
                {
                    ch->Send("Interval must be >= 0\r\n");
                    return;
                }
                r->interval = newinterval;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\r\n");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "wander_distance"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int newwd = Utilities::atoi(arg3);
                if(newwd < 0)
                {
                    ch->Send("wander_distance must be >= 0\r\n");
                    return;
                }
                r->wanderDistance = newwd;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\r\n");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "leash_distance"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int newld = Utilities::atoi(arg3);
                if(newld < 0)
                {
                    ch->Send("leash_distance must be >= 0\r\n");
                    return;
                }
                r->leashDistance = newld;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\r\n");
                return;
            }
        }
    }
	else if(!Utilities::str_cmp(arg1, "add"))
	{
        if(!Utilities::str_cmp(arg2, "npc"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int id = Utilities::atoi(arg3);
                NPCIndex * npc = Game::GetGame()->GetNPCIndex(id);
                if(npc == nullptr)
                {
                    ch->Send("NPC with that id does not exist.\r\n");
                    return;
                }
                Reset * new_reset = new Reset(pRoom, 1, id);
                pRoom->AddReset(new_reset);
                ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\r\n");
            }
        }
		else if (!Utilities::str_cmp(arg2, "obj"))
		{
			if (Utilities::IsNumber(arg3))
			{
				int id = Utilities::atoi(arg3);
				Item * item = Game::GetGame()->GetItem(id);
				if (item == nullptr)
				{
					ch->Send("Item with that id does not exist.\r\n");
					return;
				}
				Reset * new_reset = new Reset(pRoom, 2, id);
				pRoom->AddReset(new_reset);
				ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\r\n");
			}
		}
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("reset delete <#id>\r\n");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Reset * deleteme = pRoom->GetReset(delid);
        if(deleteme != nullptr)
        {
            if(!deleteme->removeme)
            {
                deleteme->removeme = true;
                ch->Send("Reset marked for deletion. Save rooms to delete.\r\n");
            }
            else
            {
                deleteme->removeme = false;
                ch->Send("Reset unmarked for deletion.\r\n");
            }
            pRoom->changed = true;
            return;
        }
    }
    else
    {
        ch->Send("Syntax: reset add npc||obj id\r\n");
        ch->Send("        reset delete id\r\n");
        ch->Send("        reset <#> target_id||interval||wander_distance||leash_distance\r\n");
    }
}

void roomEditCmd_trigger(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
    //argument = Utilities::one_argument(argument, arg3);

    //<#> type|argument|script|function
    if(Utilities::IsNumber(arg1))
    {
        int trignum = Utilities::atoi(arg1);
        Trigger * t = pRoom->GetTrigger(trignum);
        if(t == nullptr)
        {
            ch->Send("Trigger " + Utilities::itos(trignum) + " not found.\r\n");
            return;
        }

        if(!Utilities::str_cmp(arg2, "type"))
        {
            int trigtype;
            if(Utilities::IsNumber(arg3) &&  (trigtype = Utilities::atoi(arg3)) >= 0 && trigtype < Trigger::TRIG_LAST)
            {
                t->SetType(trigtype);
                pRoom->changed = true;
                ch->Send("Trigger " + Utilities::itos(trignum) + " type changed to " + Utilities::itos(trigtype) + ".\r\n");
                return;
            }
            else
            {
                ch->Send("Room trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "argument"))
        {
            t->SetArgument(arg3);
            pRoom->changed = true;
            ch->Send("Trigger " + Utilities::itos(trignum) + " argument changed to " + arg3 + ".\r\n");
            return;
        }
        else if(!Utilities::str_cmp(arg2, "script"))
        {
            if(ch && ch->user)
            {
                StringEdit::string_append( ch->user, &t->GetScript() );
                pRoom->changed = true;
            }
            return;
        }
        else if(!Utilities::str_cmp(arg2, "function"))
        {
            t->SetFunction(arg3);
            pRoom->changed = true;
            ch->Send("Trigger " + Utilities::itos(trignum) + " function changed to " + arg3 + ".\r\n");
            return;
        }
    }
    //add <type> <argument>
	else if(!Utilities::str_cmp(arg1, "add"))
	{
        int trigtype;
        if(!Utilities::IsNumber(arg2) || (trigtype = Utilities::atoi(arg2)) < 0 || trigtype >= Trigger::TRIG_LAST)
        {
            ch->Send("Room trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC");
            return;
        }
        Trigger new_trig;
        new_trig.SetType(trigtype);
        new_trig.SetArgument(arg3);
        pRoom->AddTrigger(new_trig);
        ch->Send("Added trigger #" + Utilities::itos(new_trig.id) + "\r\n");
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("trigger delete <#id>\r\n");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Trigger * deleteme = pRoom->GetTrigger(delid);
        if(deleteme != nullptr)
        {
            if(!deleteme->removeme)
            {
                deleteme->removeme = true;
                ch->Send("Trigger marked for deletion. Save rooms to delete.\r\n");
            }
            else
            {
                deleteme->removeme = false;
                ch->Send("Trigger unmarked for deletion.\r\n");
            }
            pRoom->changed = true;
            return;
        }
        else
        {
            ch->Send("Trigger " + Utilities::itos(delid) + " not found.\r\n");
            return;
        }
    }
    else
    {
        ch->Send("Syntax: trigger add <type> <argument>\r\n");
        ch->Send("        trigger delete id\r\n");
        ch->Send("        trigger <#> type||argument||script||function\r\n");
    }
}

void roomEditCmd_create(Player * ch, std::string argument)
{
    if(argument.empty() || !Utilities::IsNumber(argument))
    {
        ch->Send("Syntax: create [room id > 0]\r\n");
        return;
    }

    int value = Utilities::atoi(argument);

    if(value <= 0)
    {
	    ch->Send("Syntax: create [room id > 0]\r\n");
	    return;
    }

    Room * pRoom;
    if((pRoom = Game::GetGame()->GetRoom(value)) != nullptr)
    {
	    ch->Send( "REdit: Room id already exists.\r\n");
	    return;
    }

    pRoom = Game::GetGame()->CreateRoom(value);

    ch->editData = (void*)pRoom;
    ch->ChangeRooms(pRoom);

    ch->Send("Room created.\r\n");
    return;
}

void roomEditDirection(Player * ch, std::string argument, int direction)
{
    if(direction < 0 || direction >= Exit::DIR_LAST)
    {
        LogFile::Log("error", "roomEditDirection, bad direction");
        return;
    }

    Room * pRoom = (Room *)ch->editData;
    std::string command;
    std::string arg;

    argument = Utilities::one_argument(argument, command);
    Utilities::one_argument( argument, arg );

    if(command.empty() && argument.empty())	// Move command.
    {
        ch->Move(direction);
	    return;
    }

    if(!Utilities::str_cmp(command, "delete"))
    {
	    Room *pToRoom;
	    int rev;
	
	    if(!pRoom->exits[direction] || !pRoom->exits[direction]->to)
	    {
	        ch->Send("REdit: Cannot delete a null exit.\r\n");
	        return;
	    }

	    //Remove ToRoom Exit
	    rev = Exit::exitOpposite[direction];
	    pToRoom = pRoom->exits[direction]->to;
	
	    if(pToRoom->exits[rev])
	    {
            pToRoom->exits[rev]->removeme = true;
            pToRoom->exits[rev]->to = nullptr;
	    }

	    //Remove this exit.
	    pRoom->exits[direction]->removeme = true;
        pRoom->exits[direction]->to = nullptr;

        pRoom->changed = true;
        pToRoom->changed = true;
	    ch->Send("Exit unlinked.\r\n");
	    return;
    }

    if(!Utilities::str_cmp(command, "link"))
    {
        if(pRoom->exits[direction] && pRoom->exits[direction]->to)
        {
            ch->Send("REdit: Exit already exists.\r\n");
	        return;

        }

        if(arg.empty() || (Utilities::str_cmp(arg, "new") && !Utilities::IsNumber(arg)))
        {
            ch->Send("Syntax: [direction] link [room id]\r\n");
            ch->Send("        [direction] link new\r\n");
	        return;
        }

        Room *pToRoom;
        if(!Utilities::str_cmp(arg, "new"))
        {
            pToRoom = Game::GetGame()->CreateRoomAnyID();
        }
        else
        {
            pToRoom = Game::GetGame()->GetRoom(Utilities::atoi(arg));
        }
        
	    if(!pToRoom)
	    {
	        ch->Send("REdit: Cannot link to non-existant room.\r\n");
	        return;
	    }

	    if(pToRoom->exits[Exit::exitOpposite[direction]] && pToRoom->exits[Exit::exitOpposite[direction]]->to)
	    {
	        ch->Send("REdit: Remote side's exit already exists.\r\n");
	        return;
	    }

	    if(!pRoom->exits[direction])
	    {
	        pRoom->exits[direction] = new Exit(pRoom, pToRoom, direction);
	    }
        else
        {
            pRoom->exits[direction]->removeme = false;
            pRoom->exits[direction]->to = pToRoom;
            pRoom->exits[direction]->from = pRoom;
            pRoom->exits[direction]->direction = (Exit::Direction)direction;
        }

        if(!pToRoom->exits[Exit::exitOpposite[direction]])
        {
            pToRoom->exits[Exit::exitOpposite[direction]] = new Exit(pToRoom, pRoom, direction);
        }
        else
        {
            pToRoom->exits[Exit::exitOpposite[direction]]->removeme = false;
            pToRoom->exits[Exit::exitOpposite[direction]]->to = pRoom;
            pToRoom->exits[Exit::exitOpposite[direction]]->from = pToRoom;
            pToRoom->exits[Exit::exitOpposite[direction]]->direction = (Exit::Direction)Exit::exitOpposite[direction];
        }
        pRoom->changed = true;
        pToRoom->changed = true;
	    ch->Send("Two-way link established.\r\n");
	    return;
    }
}

void roomEditCmd_north(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTH);
}

void roomEditCmd_east(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_EAST);
}

void roomEditCmd_south(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTH);
}

void roomEditCmd_west(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_WEST);
}

void roomEditCmd_northeast(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTHEAST);
}

void roomEditCmd_southeast(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTHEAST);
}

void roomEditCmd_southwest(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTHWEST);
}

void roomEditCmd_northwest(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTHWEST);
}

void roomEditCmd_up(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_UP);
}

void roomEditCmd_down(Player * ch, std::string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_DOWN);
}

void skillEditCmd_show(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

	if(!pSkill)
	{
        LogFile::Log("error", "skillEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("long_name: [" + pSkill->long_name + "]\r\n");
    ch->Send("name:      [" + pSkill->name + "]\r\n");
    ch->Send("id:        [" + Utilities::itos(pSkill->id) + "]\r\n");
    ch->Send("cast_time: [" + Utilities::dtos(pSkill->castTime, 2) + " seconds]\r\n");
	ch->Send("flags: ");
	std::vector<int>::iterator flagiter;
	for (flagiter = pSkill->flags.begin(); flagiter != pSkill->flags.end(); ++flagiter)
	{
		ch->Send(Skill::flag_table[(*flagiter)].flag_name + " ");
	}
	ch->Send("\r\n");
	ch->Send("interrupt_flags: [");
	for (int i = 0; i < (int)pSkill->interruptFlags.size(); ++i)
	{
		if(pSkill->interruptFlags[i])
			ch->Send(Utilities::itos(i) + " ");
	}
	ch->Send("]\r\n");
    ch->Send("cooldown:  [" + Utilities::dtos(pSkill->cooldown, 2) + " seconds]\r\n");
    ch->Send("target_type:   [" + Utilities::itos(pSkill->targetType) + "]\r\n");
    ch->Send("function_name: [" + pSkill->function_name + "]\r\n");
    ch->Send("description:   [" + pSkill->description + "]\r\n");
	ch->Send("cost_desc:   [" + pSkill->costDescription + "]\r\n");
    ch->SendBW("Cost script:\r\n" + pSkill->costScript + "\r\n");
    ch->SendBW("Cast script:\r\n" + pSkill->castScript + "\r\n");
    ch->SendBW("Apply script:\r\n" + pSkill->applyScript + "\r\n");
    ch->SendBW("Tick script:\r\n" + pSkill->tickScript + "\r\n");
    ch->SendBW("Remove script:\r\n" + pSkill->removeScript + "\r\n");
}

void skillEditCmd_name(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(argument.empty())
	{
		return;
	}

	if(argument.length() > 100)
	{
		ch->Send("Maximum length is 100 characters.\r\n");
		return;
	}

    pSkill->name = argument;
    pSkill->changed = true;
	ch->Send("Name set.\r\n");
}

void skillEditCmd_long_name(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill*)ch->editData;

    if(argument.empty())
    {
        return;
    }
    if(argument.length() > 100)
    {
        ch->Send("Maximum length is 100 characters.\r\n");
        return;
    }
    pSkill->long_name = argument;
    pSkill->changed = true;
    ch->Send("Long name set.\r\n");
}

void skillEditCmd_function_name(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(argument.empty())
	{
		return;
	}

    std::string arg;
    Utilities::one_argument(argument, arg);

	if(arg.length() > 106)
	{
		ch->Send("Maximum length is 106 characters.\r\n");
		return;
	}
    //TODO: more checks here   No whitespace, one_argument should have solved that?

	pSkill->function_name = arg;
    pSkill->changed = true;
	ch->Send("function_name set.\r\n");
}

void skillEditCmd_target_type(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("target_type 0(TARGET_SELF), 1(TARGET_OTHER), 2(TARGET_HOSTILE), 3(TARGET_ANY), 4(TARGET_FRIENDLY), 5(TARGET_NONE), 6(TARGET_PASSIVE)\r\n");
        return;
    }
    pSkill->changed = true;
    pSkill->targetType = (Skill::TargetType)Utilities::atoi(arg1);
}

void skillEditCmd_reload(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

	ch->Send("Reloading LUA scripts (overrides lua_skills.lua)\r\n");

	try {
		Server::lua.script(pSkill->costScript.c_str());
		Server::lua.script(pSkill->castScript.c_str());
		Server::lua.script(pSkill->applyScript.c_str());
		Server::lua.script(pSkill->tickScript.c_str());
		Server::lua.script(pSkill->removeScript.c_str());
	}
	catch (const std::exception & e)
	{
		LogFile::Log("error", e.what());
	}
}

void skillEditCmd_import(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	std::string func_name[5] = { "_cost", "_cast", "_apply", "_tick", "_remove" };
	sol::function getinfowrapper = Server::lua["getinfowrapper"];
	std::tuple<std::string, int, int> return_values;
	sol::protected_function_result result;
	sol::protected_function skill_func;

	for (int i = 0; i <= 4; ++i)
	{
		try
		{
			skill_func = Server::lua[pSkill->function_name + func_name[i]];
			if (skill_func == sol::nil)
				continue;
			result = getinfowrapper(skill_func);
			if (!result.valid())
			{
				sol::error err = result;
				std::string what = err.what();
				LogFile::Log("error", "getinfo call failed, sol::error::what() is: " + what);
			}
			return_values = result;
		}
		catch (const std::exception & e)
		{
			LogFile::Log("error", e.what());
		}

		std::string short_src = std::get<0>(return_values);
		if (short_src == "lua_skills.lua")
		{
			ch->Send(pSkill->function_name + func_name[i] + " found in lua_skills.lua\r\n");
			std::string variable_lookup[5] = { "cost_script", "cast_script", "apply_script", "tick_script", "remove_script" };
			int first_line = std::get<1>(return_values);
			int last_line = std::get<2>(return_values);
			std::stringstream new_file;
			std::fstream importfile("lua_skills.lua", std::fstream::in);
			std::string one_line;
			std::string imported_function;
			for (int line_number = 1; !importfile.eof(); ++line_number)
			{
				std::getline(importfile, one_line);
				if (line_number < first_line || line_number > last_line) //not part of the function we're looking for
				{   //copy to new file
					new_file << one_line << '\n';
				}
				else //import this to pSkill
				{
					imported_function += one_line + '\n';
				}
			}
			importfile.close();
			*(pSkill->stringTable[variable_lookup[i].c_str()]) = imported_function;
			pSkill->changed = true;
			importfile.open("lua_skills.lua", std::fstream::out | std::fstream::trunc);
			importfile << new_file.str();
			importfile.close();
			skillEditCmd_reload(ch, "");
			Server::lua.script_file("lua_skills.lua");
			ch->Send("Reloaded file lua_skills.lua\r\n");
		}
	}
}

void skillEditCmd_export(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	std::string func_name[5] = { "_cost", "_cast", "_apply", "_tick", "_remove" };
	sol::function getinfowrapper = Server::lua["getinfowrapper"];
	std::tuple<std::string, int, int> return_values;
	sol::protected_function_result result;
	sol::protected_function skill_func;

	for (int i = 0; i <= 4; ++i)
	{
		try
		{
			skill_func = Server::lua[pSkill->function_name + func_name[i]];
			if (skill_func == sol::nil)
				continue;
			result = getinfowrapper(skill_func);
			if (!result.valid())
			{
				sol::error err = result;
				std::string what = err.what();
				LogFile::Log("error", "getinfo call failed, sol::error::what() is: " + what);
			}
			return_values = result;
		}
		catch (const std::exception & e)
		{
			LogFile::Log("error", e.what());
			continue;
		}

		std::string short_src = std::get<0>(return_values);
		if (short_src == "lua_skills.lua")
		{
			ch->Send(pSkill->function_name + func_name[i] + " found in lua_skills.lua, unable to export\r\n");
			continue;
		}
		std::fstream exportfile("lua_skills.lua", std::fstream::binary | std::fstream::out | std::fstream::app);
		std::string variable_lookup[5] = { "cost_script", "cast_script", "apply_script", "tick_script", "remove_script" };
		exportfile.write((pSkill->stringTable[variable_lookup[i].c_str()])->c_str(), pSkill->stringTable[variable_lookup[i].c_str()]->length());
		exportfile.close();
		ch->Send("exported " + pSkill->function_name + func_name[i] + "\r\n");
	}
	Server::lua.script_file("lua_skills.lua");
	ch->Send("Reloaded file lua_skills.lua\r\n");
}

void skillEditCmd_description(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(argument.empty())
	{
		return;
	}

	if(argument.length() > 255)
	{
		ch->Send("Maximum length is 255 characters.\r\n");
		return;
	}

	pSkill->description = argument;
    pSkill->changed = true;
	ch->Send("description set.\r\n");
}
void skillEditCmd_cost_description(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	if (argument.empty())
	{
		return;
	}

	if (argument.length() > 255)
	{
		ch->Send("Maximum length is 255 characters.\r\n");
		return;
	}

	pSkill->costDescription = argument;
	pSkill->changed = true;
	ch->Send("cost_description set.\r\n");
}

void skillEditCmd_cast_script(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;


        StringEdit::string_append( ch->user, &pSkill->castScript );
        pSkill->changed = true;
}

void skillEditCmd_apply_script(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;


        StringEdit::string_append( ch->user, &pSkill->applyScript );
        pSkill->changed = true;
}

void skillEditCmd_tick_script(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;


        StringEdit::string_append( ch->user, &pSkill->tickScript );
        pSkill->changed = true;
}

void skillEditCmd_remove_script(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

        StringEdit::string_append( ch->user, &pSkill->removeScript );
        pSkill->changed = true;
}

void skillEditCmd_cost_script(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;


        StringEdit::string_append( ch->user, &pSkill->costScript);
        pSkill->changed = true;
}

void skillEditCmd_cast_time(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    std::string arg;
    Utilities::one_argument(argument, arg);

    double ct;
    if(!Utilities::IsNumber(arg) || (ct = Utilities::atof(arg)) < 0)
    {
        ch->Send("Cast time must be a number >= 0.\r\n");
        return;
    }
    pSkill->castTime = ct;
    pSkill->changed = true;
    ch->Send("cast_time set.\r\n");
}

void skillEditCmd_interrupt_flags(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	std::string arg;
	Utilities::one_argument(argument, arg);
	//enum Interrupt { INTERRUPT_MOVE, INTERRUPT_HIT, INTERRUPT_NOPUSHBACK };
	int flag;
	if (!Utilities::IsNumber(arg) || (flag = Utilities::atoi(arg)) < 0)
	{
		ch->Send("Interrupt flags: 0 INTERRUPT_MOVE, 1 INTERRUPT_HIT, 2 INTERRUPT_NOPUSHBACK\r\n");
		return;
	}
	pSkill->interruptFlags.flip(flag);
	pSkill->changed = true;
	ch->Send("interrupt_flag set.\r\n");
}

void skillEditCmd_cooldown(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    std::string arg;
    Utilities::one_argument(argument, arg);

    double cd;
    if(!Utilities::IsNumber(arg) || (cd = Utilities::atof(arg)) < 0)
    {
        ch->Send("Cooldown must be a number >= 0.\r\n");
        return;
    }
    pSkill->cooldown = cd;
    pSkill->changed = true;
    ch->Send("Cooldown set.\r\n");
}

void skillEditCmd_flag(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (arg1.empty())
	{
		ch->Send("flags: nogcd gcdimmune\r\n");
		return;
	}

	arg1 = Utilities::ToLower(arg1);

	for (int i = 0; Skill::flag_table[i].flag != -1; i++)
	{
		if (!Utilities::str_cmp(Skill::flag_table[i].flag_name, arg1))
		{
			if (Utilities::FlagIsSet(pSkill->flags, Skill::flag_table[i].flag))
			{
				Utilities::FlagUnSet(pSkill->flags, Skill::flag_table[i].flag);
				ch->Send(arg1 + " flag removed.\r\n");
			}
			else
			{
				Utilities::FlagSet(pSkill->flags, Skill::flag_table[i].flag);
				ch->Send(arg1 + " flag set.\r\n");
			}
			pSkill->changed = true;
			return;
		}
	}
	ch->Send("flags: nogcd gcdimmune\r\n");
}

void npcEditCmd_show(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

	if (!pChar)
	{
		LogFile::Log("error", "npcEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("Name:      [" + pChar->name + "]\r\n");
	ch->Send("Keywords:	 [" + pChar->keywords + "]\r\n");
    ch->Send("ID:        [" + Utilities::itos(pChar->id) + "]\r\n");
    ch->Send("Level:     [" + Utilities::itos(pChar->level) + "]\r\n");
    ch->Send("Title:     [" + pChar->title + "]\r\n");
    ch->Send("Gender:    [" + Utilities::itos(pChar->gender) + "]\r\n");
    ch->Send("Health:    [" + Utilities::itos(pChar->maxHealth) + "]\r\n");
    ch->Send("Mana:      [" + Utilities::itos(pChar->maxMana) + "]\r\n");
	ch->Send("Armor:	 [" + Utilities::itos(pChar->armor) + "]\r\n");
    ch->Send("Energy:    [" + Utilities::itos(pChar->maxEnergy) + "]\r\n");
    ch->Send("attack_speed: [" + Utilities::dtos(pChar->npcAttackSpeed, 2) + "]\r\n");
    ch->Send("damage_low:   [" + Utilities::itos(pChar->npcDamageLow) + "]\r\n");
    ch->Send("damage_high:  [" + Utilities::itos(pChar->npcDamageHigh) + "]\r\n");
    double dps = ((pChar->npcDamageLow + pChar->npcDamageHigh) / 2.0) / pChar->npcAttackSpeed;
    ch->Send("(" + Utilities::dtos(dps, 2) + " damage per second)\r\n");
	ch->Send("speechtext: " + pChar->speechText + "\r\n");

    //double movementSpeed; //default = 3 rooms per second
    //double lastMoveTime;

    ch->Send("Starts quests: ");
    std::vector<Quest *>::iterator questiter;
    for(questiter = pChar->questStart.begin(); questiter != pChar->questStart.end(); ++questiter)
    {
        ch->Send(Utilities::itos((*questiter)->id) + " ");
    }
    ch->Send("\r\n");
    ch->Send("Ends quests: ");
    for(questiter = pChar->questEnd.begin(); questiter != pChar->questEnd.end(); ++questiter)
    {
        ch->Send(Utilities::itos((*questiter)->id) + " ");
    }
    ch->Send("\r\n");

    ch->Send("Flags: ");
    std::vector<int>::iterator flagiter;
    for(flagiter = pChar->flags.begin(); flagiter != pChar->flags.end(); ++flagiter)
    {
        ch->Send(NPCIndex::flag_table[(*flagiter)].flag_name + " ");
    }
    ch->Send("\r\n");

    ch->Send("Drops:\r\n");
    std::list<NPCIndex::DropData>::iterator dropiter;
    int ctr = 1;
    for(dropiter = pChar->drops.begin(); dropiter != pChar->drops.end(); ++dropiter)
    {
        ch->Send(Utilities::itos(ctr++) + ". " + Utilities::itos((*dropiter).percent) + "%: ");
        for(int i = 0; i < (int)(*dropiter).id.size(); i++)
        {
            ch->Send(Utilities::itos((*dropiter).id[i]) + " ");
        }
        ch->Send("\r\n");
    }
    ch->Send("\r\n");

    //Triggers
    if(!pChar->triggers.empty())
        ch->Send("Triggers:\r\n");
    for(std::map<int, Trigger>::iterator iter = pChar->triggers.begin(); iter != pChar->triggers.end(); iter++)
    {
        Trigger * t = &((*iter).second);
        ch->Send(Utilities::itos(t->id) + ". Type: " + Utilities::itos(t->GetType()) + " Argument: " + t->GetArgument() + 
                  " Function name: " + t->GetFunction() + "\r\n");
		ch->Send("Script: " + t->GetScript() + "\r\n");
    }
}

void npcEditCmd_name(Player * ch, std::string argument)
{
    NPCIndex * pChar = (NPCIndex*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\r\n");
        return;
    }

    pChar->name = argument;
    pChar->changed = true;
    ch->Send("name set.\r\n");
}

void npcEditCmd_title(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set title to what?\r\n");
        return;
    }

    pChar->title = argument;
    pChar->changed = true;
    ch->Send("title set.\r\n");
}

void npcEditCmd_flag(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("flags: friendly neutral aggressive trainer guild repair\r\n");
        return;
    }

    arg1 = Utilities::ToLower(arg1);

    for(int i = 0; NPCIndex::flag_table[i].flag != -1; i++)
    {
        if(!Utilities::str_cmp(NPCIndex::flag_table[i].flag_name, arg1))
        {
            if(Utilities::FlagIsSet(pChar->flags, NPCIndex::flag_table[i].flag))
            {
                Utilities::FlagUnSet(pChar->flags, NPCIndex::flag_table[i].flag);
                ch->Send(arg1 + " flag removed.\r\n");
            }
            else
            {
                Utilities::FlagSet(pChar->flags, NPCIndex::flag_table[i].flag);
                ch->Send(arg1 + " flag set.\r\n");
            }
            pChar->changed = true;
            return;
        }
    }
    ch->Send("flags: friendly neutral aggressive trainer guild repair\r\n");
}

void npcEditCmd_drop(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    //drop add percent
    //drop remove #
    //drop # add #
    //drop # remove #

    if(argument.empty())
    {
        ch->Send("Syntax: drop add <percent>\r\n");
        ch->Send("        drop remove <#>\r\n");
        ch->Send("        drop <#> add||remove <#>\r\n");
        return;
    }
    std::string arg1,arg2,arg3;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);
    argument = Utilities::one_argument(argument, arg3);

    if(!Utilities::str_cmp(arg1, "add"))
    {
        if(Utilities::IsNumber(arg2))
        {
            int percent = Utilities::atoi(arg2);
            if(percent <= 0 || percent > 100)
            {
                ch->Send("Percent must be from 1 to 100.\r\n");
                return;
            }
			NPCIndex::DropData dd;
            dd.percent = percent;
            pChar->drops.push_back(dd);
            ch->Send("Added drop with " + Utilities::itos(percent) + "% chance.\r\n");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "remove"))
    {
        if(pChar->drops.size() == 0)
        {
            ch->Send("No drops to remove.\r\n");
            return;
        }
        if(Utilities::IsNumber(arg2))
        {
            int remove = Utilities::atoi(arg2);
            if(remove <= 0 || remove > (int)pChar->drops.size())
            {
                ch->Send("Drop to remove must be from 1 to " + Utilities::itos((int)pChar->drops.size()) + ".\r\n");
                return;
            }
            int ctr = 1;
            for(std::list<NPCIndex::DropData>::iterator iter = pChar->drops.begin(); iter != pChar->drops.end(); ++iter)
            {
                if(ctr++ == remove)
                {
                    pChar->drops.erase(iter);
                    ch->Send("Drop " + Utilities::itos(remove) + " removed.\r\n");
                    return;
                }
            }
            return;
        }
    }
    else if(Utilities::IsNumber(arg1))
    {
        int modify = Utilities::atoi(arg1);
        if(modify <= 0 || modify > (int)pChar->drops.size())
        {
            ch->Send("Drop to modify must be from 1 to " + Utilities::itos((int)pChar->drops.size()) + ".\r\n");
            return;
        }
        if(!Utilities::IsNumber(arg3))
        {
            ch->Send("Syntax: drop <#> add||remove <#>\r\n");
            return;
        }
        int itemid = Utilities::atoi(arg3);
        if(itemid <= 0)
        {
            ch->Send("Item ID must be > 0.\r\n");
            return;
        }
        Item * item;
        if((item = Game::GetGame()->GetItem(itemid)) == nullptr)
        {
            ch->Send("That item does not exist.\r\n");
            return;
        }

		NPCIndex::DropData * dd = nullptr;
        int ctr = 1;
        for(std::list<NPCIndex::DropData>::iterator iter = pChar->drops.begin(); iter != pChar->drops.end(); ++iter)
        {
            if(ctr++ == modify)
            {
                dd = &(*iter);
                break;
            }
        }
        if(!Utilities::str_cmp(arg2, "add"))
        {
            dd->id.push_back(itemid);
            ch->Send("Added item ID " + Utilities::itos(itemid) + " to drop " + Utilities::itos(modify) + ".\r\n");
            return;
        }
        else if(!Utilities::str_cmp(arg2, "remove"))
        {
            for(std::vector<int>::iterator iter = dd->id.begin(); iter != dd->id.end(); ++iter)
            {
                if((*iter) == itemid)
                {
                    dd->id.erase(iter);
                    ch->Send("Removed item ID " + Utilities::itos(itemid) + " from drop " + Utilities::itos(modify) + ".\r\n");
                    return;
                }
            }
            ch->Send("Could not remove item ID " + Utilities::itos(itemid) + " from drop " + Utilities::itos(modify) + ".\r\n");
            return;
        }
    }
    ch->Send("Syntax: drop add <percent>\r\n");
    ch->Send("        drop remove <#>\r\n");
    ch->Send("        drop <#> add||remove <#>\r\n");
}

void npcEditCmd_level(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("level <#>\r\n");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0)
    {
        ch->Send("Level must be > 0.\r\n");
        return;
    }
    pChar->changed = true;
    pChar->level = level;
}

void npcEditCmd_gender(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("gender <1|2>\r\n");
        return;
    }
    int gender = Utilities::atoi(arg1);
    if(gender != 1 && gender != 2)
    {
        ch->Send("Gender must be 1 for male, 2 for female.\r\n");
        return;
    }
    pChar->changed = true;
    pChar->gender = gender;
}

void npcEditCmd_health(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("health <#>\r\n");
        return;
    }
    int health = Utilities::atoi(arg1);
    if(health < 1)
    {
        ch->Send("Health must be > 0.\r\n");
        return;
    }
    pChar->changed = true;
    pChar->maxHealth = health;
}

void npcEditCmd_mana(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("mana <#>\r\n");
        return;
    }
    int mana = Utilities::atoi(arg1);
    if(mana < 1)
    {
        ch->Send("Mana must be > 0.\r\n");
        return;
    }
    pChar->changed = true;
	pChar->maxMana = mana;
}

void npcEditCmd_armor(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (arg1.empty() || !Utilities::IsNumber(arg1))
	{
		ch->Send("armor <#>\r\n");
		return;
	}
	int armor = Utilities::atoi(arg1);
	if (armor < 0)
	{
		ch->Send("armor must be >= 0.\r\n");
		return;
	}
	pChar->changed = true;
	pChar->armor = armor;
}

void npcEditCmd_attackSpeed(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("attack_speed <#>\r\n");
        return;
    }
    double attackSpeed = Utilities::atof(arg1);
    if(attackSpeed <= 0)
    {
        ch->Send("attack_speed must be > 0\r\n");
        return;
    }
    pChar->changed = true;
    pChar->npcAttackSpeed = attackSpeed;
}

void npcEditCmd_damageLow(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("damage_low <#>\r\n");
        return;
    }
    int damage_low = Utilities::atoi(arg1);
    if(damage_low < 1 || damage_low > pChar->npcDamageHigh)
    {
        ch->Send("damage_low must be > 0, < damage_high\r\n");
        return;
    }
    pChar->changed = true;
    pChar->npcDamageLow = damage_low;
}

void npcEditCmd_damageHigh(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("damage_high <#>\r\n");
        return;
    }
    int damage_high = Utilities::atoi(arg1);
    if(damage_high < 1 || damage_high < pChar->npcDamageLow)
    {
        ch->Send("damage_high must be > 0, > damage_low\r\n");
        return;
    }
    pChar->changed = true;
    pChar->npcDamageHigh = damage_high;
}

void npcEditCmd_speechText(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set speechtext to what?\r\n");
		return;
	}

	pChar->speechText = argument;
	pChar->changed = true;
	ch->Send("speechText set.\r\n");
}

void npcEditCmd_trigger(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
    //argument = Utilities::one_argument(argument, arg3);

    //<#> type|argument|script|function
    if(Utilities::IsNumber(arg1))
    {
        int trignum = Utilities::atoi(arg1);
        Trigger * t = pChar->GetTrigger(trignum);
        if(t == nullptr)
        {
            ch->Send("Trigger " + Utilities::itos(trignum) + " not found.\r\n");
            return;
        }

        if(!Utilities::str_cmp(arg2, "type"))
        {
            int trigtype;
            if(Utilities::IsNumber(arg3) &&  (trigtype = Utilities::atoi(arg3)) >= 0 && trigtype < Trigger::TRIG_LAST)
            {
                t->SetType(trigtype);
                pChar->changed = true;
                ch->Send("Trigger " + Utilities::itos(trignum) + " type changed to " + Utilities::itos(trigtype) + ".\r\n");
                return;
            }
            else
            {
                ch->Send("NPC trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC, 6 TIMER, 7 COMBAT_TIMER, 8 ENTER_COMBAT, 9 EXIT_COMBAT\r\n");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "argument"))
        {
            t->SetArgument(arg3);
            pChar->changed = true;
            ch->Send("Trigger " + Utilities::itos(trignum) + " argument changed to " + arg3 + ".\r\n");
            return;
        }
        else if(!Utilities::str_cmp(arg2, "script"))
        {
            if(ch && ch->user)
            {
                StringEdit::string_append( ch->user, &t->GetScript() );
                pChar->changed = true;
            }
            return;
        }
        else if(!Utilities::str_cmp(arg2, "function"))
        {
            t->SetFunction(arg3);
            pChar->changed = true;
            ch->Send("Trigger " + Utilities::itos(trignum) + " function changed to " + arg3 + ".\r\n");
            return;
        }
    }
    //add <type> <argument>
	else if(!Utilities::str_cmp(arg1, "add"))
	{
        int trigtype;
        if(!Utilities::IsNumber(arg2) || (trigtype = Utilities::atoi(arg2)) < 0 || trigtype >= Trigger::TRIG_LAST)
        {
            ch->Send("NPC trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC, 6 TIMER, 7 COMBAT_TIMER, 8 ENTER_COMBAT, 9 EXIT_COMBAT\r\n");
            return;
        }
        Trigger new_trig;
        new_trig.SetType(trigtype);
        new_trig.SetArgument(arg3);
        pChar->AddTrigger(new_trig);
        ch->Send("Added trigger #" + Utilities::itos(new_trig.id) + "\r\n");
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("trigger delete <#id>\r\n");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Trigger * deleteme = pChar->GetTrigger(delid);
        if(deleteme != nullptr)
        {
            pChar->triggers.erase(deleteme->id);
            ch->Send("Trigger deleted!\r\n");
            pChar->changed = true;
            return;
        }
        else
        {
            ch->Send("Trigger " + Utilities::itos(delid) + " not found.\r\n");
            return;
        }
    }
    else
    {
        ch->Send("Syntax: trigger add <type> <argument>\r\n");
        ch->Send("        trigger delete id\r\n");
        ch->Send("        trigger <#> type||argument||script||function\r\n");
    }
}

void itemEditCmd_show(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    if(!pItem)
	{
        LogFile::Log("error", "itemEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("Name:           [" + pItem->GetName() + "]\r\n");
	ch->Send("Keywords:       [" + pItem->keywords + "]\r\n");
	ch->Send("inroom_name:    [" + pItem->inroom_name + "]\r\n");
    ch->Send("ID:             [" + Utilities::itos(pItem->GetID()) + "]\r\n");
    ch->Send("char_level:     [" + Utilities::itos(pItem->charLevel) + "]\r\n");
    ch->Send("item_level:     [" + Utilities::itos(pItem->itemLevel) + "]\r\n");
    ch->Send("quality:        [" + Utilities::itos(pItem->quality) + "]\r\n");
    ch->Send("equip_location: [" + Utilities::itos(pItem->equipLocation) + "]\r\n");
    ch->Send("binds:          [" + Utilities::itos(pItem->binds) + "]\r\n");
    ch->Send("type:           [" + Utilities::itos(pItem->type) + "]\r\n");
	ch->Send("skillid:        [" + Utilities::itos(pItem->useSkillID) + "]\r\n");
    ch->Send("quest item:     [" + Utilities::itos(pItem->quest) + "]\r\n");
    ch->Send("unique:         [" + Utilities::itos(pItem->unique) + "]\r\n");
    ch->Send("armor:          [" + Utilities::itos(pItem->armor) + "]\r\n");
	ch->Send("stats:  agility:[" + Utilities::itos(pItem->agility) + "] intellect:[" + Utilities::itos(pItem->intellect) + "] strength:[" + Utilities::itos(pItem->strength) + "]\r\n");
	ch->Send("        stamina:[" + Utilities::itos(pItem->stamina) + "] wisdom:[" + Utilities::itos(pItem->wisdom) + "] spirit:[" + Utilities::itos(pItem->spirit) + "]\r\n");
    ch->Send("durability:     [" + Utilities::itos(pItem->durability) + "]\r\n");
    ch->Send("damage_low:     [" + Utilities::itos(pItem->damageLow) + "]\r\n");
    ch->Send("damage_high:    [" + Utilities::itos(pItem->damageHigh) + "]\r\n");
    ch->Send("speed:          [" + Utilities::dtos(pItem->speed, 2) + "]\r\n");
    if(pItem->speed != 0)
        ch->Send("(damage per second): " + Utilities::dtos(((((pItem->damageHigh - pItem->damageLow) / 2.0) + pItem->damageLow) / pItem->speed), 2) + "\r\n");
    ch->Send("value:          [" + Utilities::itos(pItem->value) + "]\r\n");
}

void itemEditCmd_name(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\r\n");
        return;
    }

    pItem->name = argument;
    pItem->changed = true;
    ch->Send("name set.\r\n");
}

void itemEditCmd_keywords(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set keywords to what?\r\n");
		return;
	}

	pItem->keywords = argument;
	pItem->changed = true;
	ch->Send("keywords set.\r\n");
}

void itemEditCmd_inroom_name(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set inroom_name to what?\r\n");
		return;
	}

	pItem->inroom_name = argument;
	pItem->changed = true;
	ch->Send("inroom_name set.\r\n");
}

void itemEditCmd_item_level(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("item_level <#>\r\n");
        return;
    }
    int item_level = Utilities::atoi(arg1);
    if(item_level <= 0)
    {
        ch->Send("item_level must be > 0.\r\n");
        return;
    }
    pItem->changed = true;
    pItem->itemLevel = item_level;
}

void itemEditCmd_char_level(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("char_level <#>\r\n");
        return;
    }
    int char_level = Utilities::atoi(arg1);
    if(char_level <= 0)
    {
        ch->Send("char_level must be > 0.\r\n");
        return;
    }
    pItem->changed = true;
    pItem->charLevel = char_level;
}

void itemEditCmd_quality(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("quality: 0 POOR, 1 COMMON, 2 UNCOMMON, 3 RARE, 4 EPIC, 5 LEGENDARY, 6 ARTIFACT\r\n");
        return;
    }
    int quality = Utilities::atoi(arg1);
    if(quality < 0 || quality >= Item::QUALITY_LAST)
    {
        ch->Send("quality: 0 POOR, 1 COMMON, 2 UNCOMMON, 3 RARE, 4 EPIC, 5 LEGENDARY, 6 ARTIFACT\r\n");
        return;
    }
    pItem->changed = true;
    pItem->quality = quality;
}

void itemEditCmd_equip_location(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("equip_location: 0 NONE, 1 HEAD, 2 NECK, 3 SHOULDER, 4 BACK, 5 CHEST, 6 WRIST, 7 HANDS, 8 WAIST, 9 LEGS, 10 FEET\r\n");
        ch->Send("11 FINGER, 12 TRINKET, 13 OFFHAND, 14 MAINHAND, 15 ONEHAND, 16 TWOHAND\r\n");
        return;
    }
    int wear_location = Utilities::atoi(arg1);
    if(wear_location < 0 || wear_location >= Item::EQUIP_LAST)
    {
        ch->Send("equip_location: 0 NONE, 1 HEAD, 2 NECK, 3 SHOULDER, 4 BACK, 5 CHEST, 6 WRIST, 7 HANDS, 8 WAIST, 9 LEGS, 10 FEET\r\n");
        ch->Send("11 FINGER, 12 TRINKET, 13 OFFHAND, 14 MAINHAND, 15 ONEHAND, 16 TWOHAND\r\n");
        return;
    }
    pItem->changed = true;
    pItem->equipLocation = wear_location;
}

void itemEditCmd_binds(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("binds: 0 BIND_NONE, 1 BIND_PICKUP, 2 BIND_EQUIP\r\n");
        return;
    }
    int binds = Utilities::atoi(arg1);
    if(binds < 0 || binds >= Item::BIND_LAST)
    {
        ch->Send("binds: 0 BIND_NONE, 1 BIND_PICKUP, 2 BIND_EQUIP\r\n");
        return;
    }
    pItem->changed = true;
    pItem->binds = binds;
}

void itemEditCmd_type(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("type: 0 TYPE_ARMOR_CLOTH, 1 TYPE_ARMOR_LEATHER, 2 TYPE_ARMOR_MAIL, 3 TYPE_ARMOR_PLATE, 4 TYPE_WEAPON_SWORD, 5 TYPE_WEAPON_DAGGER\r\n");
        ch->Send("      6 TYPE_WEAPON_MACE, 7 TYPE_WEAPON_AXE, 8 TYPE_WEAPON_POLEARM, 9 TYPE_WEAPON_STAFF, 10 TYPE_CONTAINER, 11 TYPE_FOOD\r\n");
        ch->Send("      12 TYPE_CONSUMABLE, 13 TYPE_MISC, 14 TYPE_SHIELD, 15 TYPE_DRINK\r\n");
        return;
    }
    int type = Utilities::atoi(arg1);
    if(type < 0 || type >= Item::TYPE_LAST)
    {
        ch->Send("type: 0 TYPE_ARMOR_CLOTH, 1 TYPE_ARMOR_LEATHER, 2 TYPE_ARMOR_MAIL, 3 TYPE_ARMOR_PLATE, 4 TYPE_WEAPON_SWORD, 5 TYPE_WEAPON_DAGGER\r\n");
        ch->Send("      6 TYPE_WEAPON_MACE, 7 TYPE_WEAPON_AXE, 8 TYPE_WEAPON_POLEARM, 9 TYPE_WEAPON_STAFF, 10 TYPE_CONTAINER, 11 TYPE_FOOD\r\n");
        ch->Send("      12 TYPE_CONSUMABLE, 13 TYPE_MISC, 14 TYPE_SHIELD, 15 TYPE_DRINK\r\n");
        return;
    }
    pItem->changed = true;
    pItem->type = type;
}

void itemEditCmd_skillid(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (arg1.empty() || !Utilities::IsNumber(arg1))
	{
		ch->Send("skillid: #\r\n");
		return;
	}
	int skillid = Utilities::atoi(arg1);
	if (skillid < 0)
	{
		ch->Send("skillid: >= 0\r\n");
		return;
	}
	pItem->changed = true;
	pItem->useSkillID = skillid;
}

void itemEditCmd_quest(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("quest: 0,1\r\n");
        return;
    }
    int quest = Utilities::atoi(arg1);
    if(quest < 0 || quest > 1)
    {
        ch->Send("quest: 0,1\r\n");
        return;
    }
    pItem->changed = true;
    pItem->quest = quest;
}

void itemEditCmd_unique(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("unique: 0,1\r\n");
        return;
    }
    int unique = Utilities::atoi(arg1);
    if(unique < 0 || unique > 1)
    {
        ch->Send("unique: 0,1\r\n");
        return;
    }
    pItem->changed = true;
    pItem->unique = unique;
}

void itemEditCmd_armor(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("armor: integer >= 0\r\n");
        return;
    }
    int armor = Utilities::atoi(arg1);
    if(armor < 0)
    {
        ch->Send("armor: integer >= 0\r\n");
        return;
    }
    pItem->changed = true;
    pItem->armor = armor;
}

void itemEditCmd_durability(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("durability: integer >= 0\r\n");
        return;
    }
    int durability = Utilities::atoi(arg1);
    if(durability < 0)
    {
        ch->Send("durability: integer >= 0\r\n");
        return;
    }
    pItem->changed = true;
    pItem->durability = durability;
}

void itemEditCmd_damageLow(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("damage_low: integer >= 0, <= damage_high\r\n");
        return;
    }
    int damage_low = Utilities::atoi(arg1);
    if(damage_low < 0 || damage_low > pItem->damageHigh)
    {
        ch->Send("damage_low: integer >= 0, <= damage_high\r\n");
        return;
    }
    pItem->changed = true;
    pItem->damageLow = damage_low;
}

void itemEditCmd_damageHigh(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("damage_high: integer >= 0, >= damage_low\r\n");
        return;
    }
    int damage_high = Utilities::atoi(arg1);
    if(damage_high < 0 || damage_high < pItem->damageLow)
    {
        ch->Send("damage_high: integer >= 0, >= damage_low\r\n");
        return;
    }
    pItem->changed = true;
    pItem->damageHigh = damage_high;
}

void itemEditCmd_speed(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("speed: float >= 0.9\r\n");
        return;
    }
    double speed = Utilities::atof(arg1);
    if(speed < 0.9)
    {
        ch->Send("speed: float > 0.9\r\n");
        return;
    }
    pItem->changed = true;
    pItem->speed = speed;
}

void itemEditCmd_value(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("value: integer >= 0\r\n");
        return;
    }
    int value = Utilities::atoi(arg1);
    if(value < 0)
    {
        ch->Send("value: integer >= 0\r\n");
        return;
    }
    pItem->changed = true;
    pItem->value = value;
}

void itemEditCmd_stats(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	std::string arg1;
	std::string arg2;
	argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);

	if (arg1.empty() || arg2.empty() || !Utilities::IsNumber(arg2))
	{
		ch->Send("stats agility/intellect/strength/stamina/wisdom/spirit <val>\r\n");
		return;
	}
	int val = Utilities::atoi(arg2);

	if (!Utilities::str_cmp("agility", arg1))
	{
		pItem->agility = val;
		ch->Send("agility set\r\n");
		pItem->changed = true;
		return;
	}
	else if (!Utilities::str_cmp("intellect", arg1))
	{
		pItem->intellect = val;
		ch->Send("intellect set\r\n");
		pItem->changed = true;
		return;
	}
	else if (!Utilities::str_cmp("strength", arg1))
	{
		pItem->strength = val;
		ch->Send("strength set\r\n");
		pItem->changed = true;
		return;
	}
	else if (!Utilities::str_cmp("stamina", arg1))
	{
		pItem->stamina = val;
		ch->Send("stamina set\r\n");
		pItem->changed = true;
		return;
	}
	else if (!Utilities::str_cmp("wisdom", arg1))
	{
		pItem->wisdom = val;
		ch->Send("wisdom set\r\n");
		pItem->changed = true;
		return;
	}
	else if (!Utilities::str_cmp("spirit", arg1))
	{
		pItem->spirit = val;
		ch->Send("spirit set\r\n");
		pItem->changed = true;
		return;
	}
	ch->Send("stats agility/intellect/strength/stamina/wisdom/spirit <val>\r\n");
}

void questEditCmd_show(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

	if(!pQuest)
	{
        LogFile::Log("error", "questEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("name:      [" + pQuest->name + "]\r\n");
    ch->Send("ID:        [" + Utilities::itos(pQuest->id) + "]\r\n");
    ch->Send("level:     [" + Utilities::itos(pQuest->level) + "]\r\n");
    ch->Send("level_requirement: [" + Utilities::itos(pQuest->levelRequirement) + "]\r\n");
    ch->Send("quest_requirement: [" + Utilities::itos(pQuest->questRequirement) + "]\r\n");
    ch->Send("quest_restriction: [" + Utilities::itos(pQuest->questRestriction) + "]\r\n");
    ch->Send("start id:     [" + Utilities::itos(pQuest->start) + "]\r\n");
    ch->Send("end id:       [" + Utilities::itos(pQuest->end) + "]\r\n");
    ch->Send("exp_reward:   [" + Utilities::itos(pQuest->experienceReward) + "]\r\n");
    ch->Send("money_reward: [" + Utilities::itos(pQuest->moneyReward) + "]\r\n");
	std::string itemrewards = "item_reward:  [";
	for (auto itemiter = std::begin(pQuest->itemRewards); itemiter != std::end(pQuest->itemRewards); ++itemiter)
	{
		itemrewards += Utilities::itos(*itemiter) + " ";
	}
	itemrewards += "]\r\n";
	ch->Send(itemrewards);
    ch->Send("shareable:    [" + Utilities::itos(pQuest->shareable) + "]\r\n");

    ch->Send("Objectives:\r\n");
    int ctr = 1;
    std::vector<Quest::QuestObjective>::iterator objiter;
    for(objiter = pQuest->objectives.begin(); objiter != pQuest->objectives.end(); ++objiter)
    {
        int objid = 0;
        switch((*objiter).type)
        {
            case Quest::OBJECTIVE_ROOM: objid = ((Room*)((*objiter).objective))->id; break;
            case Quest::OBJECTIVE_KILLNPC:
            case Quest::OBJECTIVE_VISITNPC: objid = ((NPCIndex*)((*objiter).objective))->id; break;
            case Quest::OBJECTIVE_ITEM: objid = ((Item*)((*objiter).objective))->GetID(); break;
        }
        ch->Send(Utilities::itos(ctr) + ". Type: " + Utilities::itos((*objiter).type) + " Count: " + Utilities::itos((*objiter).count) 
                   + " Objective ID: " + Utilities::itos(objid) + " " + (*objiter).description + "\r\n");
        ctr++;
    }

    ch->Send("short_desc:     " + pQuest->shortDescription + "\r\n");
    ch->Send("long_desc:      " + pQuest->longDescription + "\r\n");
    ch->Send("progress_msg:   " + pQuest->progressMessage + "\r\n");
    ch->Send("completion_msg: " + pQuest->completionMessage + "\r\n");
}

void questEditCmd_objective(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest*)ch->editData;
    //objective add room|npc|item id count
    //          remove #

    std::string arg1,arg2,arg3,arg4;

    if(argument.empty())
    {
        ch->Send("objective add room||npc||item id count <description>\r\n");
        ch->Send("          remove #\r\n");
        return;
    }
    argument = Utilities::one_argument(argument, arg1);
    if(!Utilities::str_cmp("add", arg1))
    {
        argument = Utilities::one_argument(argument, arg2);
        argument = Utilities::one_argument(argument, arg3);
        argument = Utilities::one_argument(argument, arg4);
        
        std::string description = argument;
        if(description.empty())
        {
            ch->Send("empty description\r\n");
        }

        //type
        int type;
        if(!Utilities::str_cmp(arg2, "room"))
        {
            type = Quest::OBJECTIVE_ROOM;
        }
        else if(!Utilities::str_cmp(arg2, "npc"))
        {
            type = Quest::OBJECTIVE_KILLNPC;
        }
        else if(!Utilities::str_cmp(arg2, "item"))
        {
            type = Quest::OBJECTIVE_ITEM;
        }
        else
        {
            ch->Send("objective types: room, npc, item\r\n");
            return;
        }

        //id
        if(!Utilities::IsNumber(arg3))
        {
            ch->Send("objective id must be a number\r\n");
            return;
        }
        int id = Utilities::atoi(arg3);
        if(id <= 0)
        {
            ch->Send("objective id must be > 0\r\n");
            return;
        }

        //count
        if(!Utilities::IsNumber(arg4))
        {
            ch->Send("objective count must be a number\r\n");
            return;
        }
        int count = Utilities::atoi(arg4);
        if(count <= 0)
        {
            ch->Send("objective count must be > 0\r\n");
            return;
        }
        pQuest->AddObjective(type, count, id, description);
        pQuest->changed = true;
        ch->Send("Objective added.\r\n");
        return;
    }
    else if(!Utilities::str_cmp("remove", arg1))
    {
        argument = Utilities::one_argument(argument, arg2);

        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("Remove which objective?\r\n");
            return;
        }
        int index = Utilities::atoi(arg2);
        if(index <= 0)
        {
            ch->Send("Index to remove must be > 0.\r\n");
            return;
        }
        pQuest->RemoveObjective(index);
        pQuest->changed = true;
        ch->Send("Objective removed.\r\n");
        return;
    }
    ch->Send("objective add room||npc||item id count\r\n");
    ch->Send("          remove #\r\n");
}

void questEditCmd_name(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\r\n");
        return;
    }

    pQuest->name = argument;
    ch->Send("name set.\r\n");
    pQuest->changed = true;
}

void questEditCmd_shortdesc(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set short description to what?\r\n");
        return;
    }

    pQuest->shortDescription = argument;
    ch->Send("short_description set.\r\n");
    pQuest->changed = true;
}

void questEditCmd_longdesc(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

        StringEdit::string_append( ch->user, &pQuest->longDescription );
        pQuest->changed = true;
}

void questEditCmd_progressmsg(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

        StringEdit::string_append( ch->user, &pQuest->progressMessage );
        pQuest->changed = true;
}

void questEditCmd_completionmsg(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

        StringEdit::string_append( ch->user, &pQuest->completionMessage );
        pQuest->changed = true;
}

void questEditCmd_level(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level <#>\r\n");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0 || level > Game::MAX_LEVEL)
    {
        ch->Send("level must be between 1 and " + Utilities::itos(Game::MAX_LEVEL) + "\r\n");
        return;
    }
    pQuest->level = level;
    pQuest->changed = true;
    ch->Send("level set.\r\n");
}

void questEditCmd_levelrequirement(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level_requirement <#>\r\n");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0 || level > Game::MAX_LEVEL)
    {
        ch->Send("level_requirement must be between 1 and " + Utilities::itos(Game::MAX_LEVEL) + "\r\n");
        return;
    }
    pQuest->levelRequirement = level;
    pQuest->changed = true;
    ch->Send("level_requirement set.\r\n");
}

void questEditCmd_questrequirement(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("quest_requirement <#>\r\n");
        return;
    }
    int requirement = Utilities::atoi(arg1);
    if(requirement < 0)
    {
        ch->Send("quest_requirement must be >= 0\r\n");
        return;
    }
	else if (requirement == 0)
	{
		ch->Send("quest_requirement cleared\r\n");
		pQuest->questRequirement = 0;
		pQuest->changed = true;
		return;
	}
    Quest * q = Game::GetGame()->GetQuest(requirement);
    if(q == nullptr)
    {
        ch->Send("A quest with that id does not exist.\r\n");
        return;
    }
    pQuest->questRequirement = q->id;
    pQuest->changed = true;
    ch->Send("quest_requirement set.\r\n");
}

void questEditCmd_questrestriction(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("quest_restriction <#>\r\n");
        return;
    }
    int restriction = Utilities::atoi(arg1);
    if(restriction < 0)
    {
        ch->Send("quest_restriction must be >= 0\r\n");
        return;
    }
	else if (restriction == 0)
	{
		ch->Send("quest_restriction cleared\r\n");
		pQuest->questRestriction = 0;
		pQuest->changed = true;
		return;
	}
    Quest * q = Game::GetGame()->GetQuest(restriction);
    if(q == nullptr)
    {
        ch->Send("A quest with that id does not exist.\r\n");
        return;
    }
    pQuest->questRestriction = q->id;
    pQuest->changed = true;
    ch->Send("quest_restriction set.\r\n");
}

void questEditCmd_start(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("start <#>\r\n");
        return;
    }
    int start = Utilities::atoi(arg1);
    if(start <= 0)
    {
        ch->Send("start npc id must be > 0\r\n");
        return;
    }
    NPCIndex * tch = Game::GetGame()->GetNPCIndex(start);
    if(tch == nullptr)
    {
        ch->Send("An npc with that id does not exist.\r\n");
        return;
    }
    //remove this quest from our current pQuest->start's questStart vector
    if(pQuest->start != 0)
    {
		NPCIndex * remove = Game::GetGame()->GetNPCIndex(pQuest->start);
        for(auto iter = remove->questStart.begin(); iter != remove->questStart.end(); ++iter)
        {
            if((*iter)->id == pQuest->id)
            {
				remove->questStart.erase(iter);
                break;
            }
        }
    }
    tch->questStart.push_back(pQuest);
    pQuest->start = tch->id;
    pQuest->changed = true;
    ch->Send("start npc set.\r\n");
}

void questEditCmd_end(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("end <#>\r\n");
        return;
    }
    int end = Utilities::atoi(arg1);
    if(end <= 0)
    {
        ch->Send("end npc id must be > 0\r\n");
        return;
    }
    NPCIndex * tch = Game::GetGame()->GetNPCIndex(end);
    if(tch == nullptr)
    {
        ch->Send("An npc with that id does not exist.\r\n");
        return;
    }
    if(pQuest->end != 0)
    {
		NPCIndex * remove = Game::GetGame()->GetNPCIndex(pQuest->end);
		for(auto iter = remove->questEnd.begin(); iter != remove->questEnd.end(); ++iter)
        {
            if((*iter)->id == pQuest->id)
            {
				remove->questEnd.erase(iter);
                break;
            }
        }
    }
    tch->questEnd.push_back(pQuest);
    pQuest->end = tch->id;
    pQuest->changed = true;
    ch->Send("end npc set.\r\n");
}

void questEditCmd_expreward(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("exp_reward <#>\r\n");
        return;
    }
    int exp = Utilities::atoi(arg1);
    if(exp < 0)
    {
        ch->Send("exp_reward must be >= 0\r\n");
        return;
    }
    pQuest->experienceReward = exp;
    pQuest->changed = true;
    ch->Send("exp_reward set.\r\n");
}

void questEditCmd_moneyreward(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("money_reward <#>\r\n");
        return;
    }
    int money = Utilities::atoi(arg1);
    if(money < 0)
    {
        ch->Send("money_reward must be >= 0\r\n");
        return;
    }
    pQuest->moneyReward = money;
    pQuest->changed = true;
    ch->Send("money_reward set.\r\n");
}

void questEditCmd_itemreward(Player * ch, std::string argument)
{
	Quest * pQuest = (Quest *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("item_reward <#>\r\n");
		return;
	}
	int itemid = Utilities::atoi(arg1);
	if (itemid <= 0)
	{
		ch->Send("itemid must be > 0\r\n");
		return;
	}
	std::vector<int>::iterator iter = std::find(pQuest->itemRewards.begin(), pQuest->itemRewards.end(), itemid);
	if (iter != pQuest->itemRewards.end())
	{
		pQuest->itemRewards.erase(iter);
		pQuest->changed = true;
		ch->Send("Item reward removed\r\n");
		return;
	}
	Item * reward = Game::GetGame()->GetItem(itemid);
	if (!reward)
	{
		ch->Send("Item with that ID does not exist.\r\n");
		return;
	}
	pQuest->itemRewards.push_back(itemid);
	pQuest->changed = true;
	ch->Send("Item reward added\r\n");
}

void questEditCmd_shareable(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("shareable 0||1\r\n");
        return;
    }
    int share = Utilities::atoi(arg1);
    if(share != 0 && share != 1)
    {
        ch->Send("shareable must equal 0 or 1\r\n");
        return;
    }
    pQuest->shareable = share;
    pQuest->changed = true;
    ch->Send("shareable set.\r\n");
}

void classEditCmd_show(Player * ch, std::string argument)
{
    Class * pClass = (Class *)ch->editData;
	if (pClass == nullptr)
		return;

    ch->Send("Name:      [" + pClass->name + "]\r\n");
    ch->Send("ID:        [" + Utilities::itos(pClass->GetID()) + "]\r\n");
    ch->Send("Color [|" + pClass->color + "]\r\n");

    ch->Send("(Level, Skill ID, long_name):\r\n");
    std::list<Class::SkillData>::iterator iter;
    for(iter = pClass->classSkills.begin(); iter != pClass->classSkills.end(); ++iter)
    {
        ch->Send("{" + Utilities::itos(iter->level) + ",    " + Utilities::itos(iter->skill->id) + ",     " + iter->skill->long_name + "}\r\n");
    }
}

void classEditCmd_skill(Player * ch, std::string argument)
{
	Class * pClass = (Class *)ch->editData;
	if (pClass == nullptr)
		return;

	std::string arg1;
	std::string arg2;
	std::string arg3;
	argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
	argument = Utilities::one_argument(argument, arg3);

	if (arg1.empty() || arg2.empty() || !Utilities::IsNumber(arg2))
	{
		ch->Send("skill <add/remove> <#id> <#level>\r\n");
		return;
	}
	int id = Utilities::atoi(arg2);
	if (!Utilities::str_cmp(arg1, "add"))
	{
		if (arg3.empty() || !Utilities::IsNumber(arg3))
		{
			ch->Send("skill <add/remove> <#id> <#level>\r\n");
			return;
		}
		int level = Utilities::atoi(arg3);
		if (!pClass->HasSkill(id))
		{
			pClass->changed = true;
			pClass->AddSkill(id, level);
			ch->Send("Added skill " + Utilities::itos(id) + "\r\n");
			return;
		}
		ch->Send("Class already has skill id " + Utilities::itos(id) + "\r\n");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "remove"))
	{
		if (!pClass->HasSkill(id))
		{
			ch->Send("Class does not have skill id : " + Utilities::itos(id) + "\r\n");
			return;
		}
		pClass->changed = true;
		pClass->RemoveSkill(id);
		ch->Send("Removed skill " + Utilities::itos(id) + "\r\n");
		return;
	}
	ch->Send("skill <add/remove> <#id>\r\n");
	return;
}

void helpEditCmd_show(Player * ch, std::string argument)
{
    Help * pHelp = (Help *)ch->editData;
	if(pHelp == nullptr)
		return;

	ch->Send("ID:			[" + Utilities::itos(pHelp->id) + "]\r\n");
    ch->Send("Title:		[" + pHelp->title + "]\r\n");
	ch->Send("Search String:[" + pHelp->search_string + "]\r\n");
    ch->Send("Text:\r\n");
    ch->Send(pHelp->text + "\r\n");
}

void helpEditCmd_title(Player * ch, std::string argument)
{
	Help * pHelp = (Help *)ch->editData;
	if(pHelp == nullptr)
		return;

	if(argument.empty())
	{
		ch->Send("Syntax: title <newtitle>\r\n");
		return;
	}
	if(!Utilities::IsAlpha(argument))
	{
		ch->Send("Invalid title.\r\n");
		return;
	}
	pHelp->title = argument;
	ch->Send("Title changed.\r\n");
}

void helpEditCmd_text(Player * ch, std::string argument)
{
    Help * pHelp = (Help *)ch->editData;

    if(argument.empty() && ch->user)
    {
        StringEdit::string_append( ch->user, &pHelp->text );
        pHelp->changed = true;
        return;
    }

    ch->Send( "Syntax:  text\r\n" );
}

void helpEditCmd_delete(Player * ch, std::string argument)
{
    Help * pHelp = (Help *)ch->editData;

	if(pHelp->remove)
	{
		ch->Send("ID " + Utilities::itos(pHelp->id) + " unmarked for deletion.\r\n");
		pHelp->remove = false;
	}
	else
	{
		ch->Send("ID " + Utilities::itos(pHelp->id) + " marked for deletion. Save to delete.\r\n");
		pHelp->remove = true;
	}
}


void playerEditCmd_show(Player * ch, std::string argument)
{

}

void playerEditCmd_name(Player * ch, std::string argument)
{

}

//Sets level but NOT experience
void playerEditCmd_level(Player * ch, std::string argument)
{
    std::string * name = (std::string*)ch->editData;
    Player * vch = Game::GetGame()->GetPlayerByName(*name);

	if(!vch)
	{
        ch->Send("Error: Editing a null character. Target offline?\r\n");
		return;
	}

    std::string arg1;
    Utilities::one_argument(argument, arg1);
    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level <#arg>\r\n");
        return;
    }
    int newlevel = Utilities::atoi(arg1);
    if(vch->GetLevel() == newlevel)
    {
        ch->Send("Level is already " + arg1 + "\r\n");
        return;
    }
    if(newlevel > Game::MAX_LEVEL || newlevel < 1)
    {
        ch->Send("Level out of range: 1 to " + Utilities::itos(Game::MAX_LEVEL) + "\r\n");
        return;
    }
    vch->SetLevel(newlevel);
    //vch->player->SetExperience(Game::ExperienceForLevel(newlevel));
    ch->Send("Level set to " + arg1 + "\r\n");
}

void playerEditCmd_exp(Player * ch, std::string argument)
{

}

void areaEditCmd_show(Player * ch, std::string argument)
{
    Area * pArea = (Area *)ch->editData;

	if(!pArea)
	{
        LogFile::Log("error", "areaEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("Name:             [" + pArea->name + "]\r\n");
    ch->Send("ID:               [" + Utilities::itos(pArea->GetID()) + "]\r\n");
	ch->Send("pvp:              [" + Utilities::itos(pArea->pvp) + "]\r\n");
    ch->Send("level_range_low:  [" + Utilities::itos(pArea->level_range_low) + "]\r\n");
    ch->Send("level_range_high: [" + Utilities::itos(pArea->level_range_high) + "]\r\n");
	ch->Send("death_room:       [" + Utilities::itos(pArea->death_room) + "]\r\n");
}

void areaEditCmd_name(Player * ch, std::string argument)
{
    Area * pArea = (Area *)ch->editData;

	if(argument.empty())
	{
		pArea->name.clear();
		ch->Send("Name cleared.\r\n");
		return;
	}

	if(argument.length() > 50)
	{
		ch->Send("Maximum length is 50 characters.\r\n");
		return;
	}

	pArea->name = argument;
    pArea->changed = true;
	ch->Send("Name set.\r\n");
}

void areaEditCmd_pvp(Player * ch, std::string argument)
{
    Area * pArea = (Area *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("pvp: 0 - 3\r\n");
        return;
    }
    int pvp = Utilities::atoi(arg1);
    if(pvp < 0 || pvp > 3)
    {
        ch->Send("pvp: 0 - 3\r\n");
        return;
    }
    pArea->pvp = pvp;
    pArea->changed = true;
    ch->Send("pvp set.\r\n");
}

void areaEditCmd_death_room(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("death_room: #\r\n");
		return;
	}
	int death_room = Utilities::atoi(arg1);
	if (death_room < 0)
	{
		ch->Send("death_room: # >= 0\r\n");
		return;
	}
	pArea->death_room = death_room;
	pArea->changed = true;
	ch->Send("death_room set.\r\n");
}

void areaEditCmd_levelRangeLow(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("level_range_low: 0 < # <= max_level\r\n");
		return;
	}
	int level_range_low = Utilities::atoi(arg1);
	if (level_range_low <= 0 || level_range_low > Game::MAX_LEVEL)
	{
		ch->Send("level_range_low: 0 < # <= max_level\r\n");
		return;
	}
	pArea->level_range_low = level_range_low;
	pArea->changed = true;
	ch->Send("level_range_low set.\r\n");
}

void areaEditCmd_levelRangeHigh(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("level_range_high: 0 < # <= max_level\r\n");
		return;
	}
	int level_range_high = Utilities::atoi(arg1);
	if (level_range_high <= 0 || level_range_high > Game::MAX_LEVEL)
	{
		ch->Send("level_range_high: 0 < # <= max_level\r\n");
		return;
	}
	pArea->level_range_high = level_range_high;
	pArea->changed = true;
	ch->Send("level_range_high set.\r\n");
}
