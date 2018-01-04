#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CLogFile.h"
#include "CmySQLQueue.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CClient.h"
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
#include "CCommand.h"
#include "utils.h"
#include "mud.h"

using namespace std;

void cmd_goto(Character * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("Syntax: goto <room id>\n\r");
        return;
    }
    string arg1;
    argument = Utilities::one_argument(argument, arg1);
    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("Syntax: goto <room id>\n\r");
        return;
    }

    int rnum = Utilities::atoi(arg1);

    if(Game::GetGame()->GetRoom(rnum) != NULL)
        ch->Message(ch->name + " disappears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);

    if(rnum <= 0 || !ch->ChangeRoomsID(rnum))
    {
        ch->Send("Room " + arg1 + " does not exist.\n\r");
        return;
    }
    ch->Message(ch->name + " appears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    cmd_look(ch, "");
}

void cmd_restore(Character * ch, string argument)
{
    if(!Utilities::str_cmp(argument, "cooldown"))
    {
        std::list<User*>::iterator iter;
        for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
        {
            User * u = (*iter);
            if(u->character)
            {
                u->Send("Your cooldowns have been reset!\n\r");
                u->character->cooldowns.clear();
            }
        }
        return;
    }

    std::list<User*>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
    {
        User * u = (*iter);
        if(u->character)
        {
            u->Send("Your health has been restored!\n\r");
            u->character->health = u->character->maxHealth;
            u->character->mana = u->character->maxMana;
            u->character->stamina = u->character->maxStamina;
            if(u->character->player)
                u->character->player->isGhost = false;
        }
    }
}

void cmd_sockets(Character * ch, string argument)
{
    std::list<User *>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); iter++)
    {
        if((*iter)->IsConnected())
        {
            if((*iter)->character)
            {
                ch->Send((*iter)->character->name + " ");
            }
            else
            {
                ch->Send("<no character> ");
            }

            ch->Send((*iter)->GetClient()->GetIPAddress() + "\n\r");
        }
    }
}

void cmd_disconnect(Character * ch, string argument)
{
    ch->Send("cmd_disconnect\n\r");
}

void cmd_shutdown(Character * ch, string argument)
{
    Game::GetGame()->GlobalMessage("Server shutdown.");
    Game::GetGame()->shutdown = true;
}

void cmd_load(Character * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("load npc <#id>\n\r");
        ch->Send("load item <#id>\n\r");
        return;
    }
    string arg1;
    string arg2;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    if(!Utilities::str_cmp(arg1, "npc"))
    {
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("load npc <#id>\n\r");
            return;
        }
        int id = Utilities::atoi(arg2);
        Character * charIndex = Game::GetGame()->GetCharacterIndex(id);
        if(charIndex == NULL)
        {
            ch->Send("NPC " + arg2 + " does not exist.\n\r");
            return;
        }
        Character * newChar = Game::GetGame()->NewCharacter(charIndex);
        newChar->ChangeRooms(ch->room);
    }
    else if(!Utilities::str_cmp(arg1, "item"))
    {
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("load item <#id>\n\r");
            return;
        }
        int id = Utilities::atoi(arg2);
        Item * itemIndex = Game::GetGame()->GetItemIndex(id);
        if(itemIndex == NULL)
        {
            ch->Send("Item " + arg2 + " does not exist.\n\r");
            return;
        }
        ch->player->NewItemInventory(itemIndex);
    }
    else
    {
        ch->Send("load npc <#id>\n\r");
        ch->Send("load item <#id>\n\r");
    }
}

void cmd_purge(Character * ch, string argument)
{
    if(!ch || !ch->room)
        return;

    std::list<Character *>::iterator iter;
	for(iter = ch->room->characters.begin(); iter != ch->room->characters.end();)
	{
        Character * vch = (*iter);
        if(vch->IsNPC())
        {
            vch->ExitCombat();
            iter = ch->room->characters.erase(iter);
            Game::GetGame()->RemoveCharacter(vch);
        }
        else
        {
            ++iter;
        }
	}
}

void cmd_transfer(Character * ch, string argument)
{
    string arg1;	//transfer who
	string arg2;
	string arg3;
	//Area * getarea = NULL;
	Room * getroom = NULL;
	Character * chtran = NULL;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
	argument = Utilities::one_argument(argument, arg3);

	// transfer <player name> (to ch->in_room)
    if(!arg1.empty() && ((arg2.empty() && arg3.empty()) || !Utilities::IsNumber(arg2)))
	{
		if(ch == NULL || ch->room == NULL)
		{
            LogFile::Log("error", "cmd_transfer: ch == NULL || ch->room == NULL");
			return;
		}

		if((chtran = Game::GetGame()->GetPlayerWorld(ch, arg1)) == NULL)
		{
			ch->Send("They aren't here.\n\r");
			return;
		}
		if(chtran == ch)
		{
			ch->Send("You're already here!\n\r");
			return;
		}
		getroom = ch->room;
	}
	// transfer <player name> <player name>
	// transfer <player name> <vnum>
	else
	{
		ch->Send("Syntax: transfer <player name>\n\r");
		ch->Send("        transfer <player name> <player name>\n\r");
		ch->Send("        transfer <player name> <vnum>\n\r");
		return;
	}

    chtran->Message(chtran->name + " disappears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    chtran->Send("You have been transferred!\n\r");
    chtran->ChangeRoomsID(getroom->id);
    chtran->Message(chtran->name + " appears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    
    cmd_look(chtran, "");
}

void cmd_advance(Character * ch, string argument)
{
    string player_target_arg;
	string new_level_arg;

    argument = Utilities::one_argument(argument, player_target_arg);
	argument = Utilities::one_argument(argument, new_level_arg);

    if(!Utilities::IsNumber(new_level_arg))
    {
        ch->Send("New level must be a number.\n\r");
        return;
    }
    int new_level = Utilities::atoi(new_level_arg);

    Character * player_target = Game::GetGame()->GetCharacterByPCName(player_target_arg);
    if(player_target == NULL || player_target->player == NULL)
    {
        ch->Send("Player not found.\n\r");
        return;
    }
    player_target->ApplyExperience(Game::ExperienceForLevel(new_level) - player_target->player->experience);
}

void cmd_sql(Character * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("Empty statement.\n\r");
        return;
    }

    string arg1;
    Utilities::one_argument(argument, arg1);
    
    StoreQueryResult result;

    if(!Utilities::str_cmp(arg1, "SELECT"))
    {
        //read
        try
        {
            result = Server::sqlQueue->Read(argument);
            if(result.empty())
            {
                ch->Send("Empty Resultset.\n\r");
            }
            else
            {
                for(int i = 0; i < (int)result.num_fields(); i++)
                {
                    ch->Send(result.field_name(i) + " ");
                }
                ch->Send("\n\r");

                for(StoreQueryResult::iterator i = result.begin(); i != result.end(); i++)
                {
                    Row row = *i;
                    for(int j = 0; j < (int)row.size(); j++)
                    {
                        ch->Send(string(row[j]) + " ");
                    }
                    ch->Send("\n\r");
                }
                ch->Send("Done.\n\r");
            }
        }
        catch(const BadQuery & er)
        {
            ch->Send(string("Bad Query: ") + er.what() + "\n\r");
        }
    }
    else
    {
        //write
        string status;
        Server::sqlQueue->Write(argument, status);
        
        ch->Send(status + "\n\r");
    }
}

void cmd_systeminfo(Character * ch, string argument)
{
    ch->Send("|MThere are currently |X" + Utilities::itos((int)Game::GetGame()->rooms.size()) + " |Munique locations, |X" +
        Utilities::itos((int)Game::GetGame()->characters.size() - (int)Game::GetGame()->users.size()) + "|M non player characters and |X" +
        Utilities::itos((int)Game::GetGame()->quests.size()) + "|M quests.|X\n\r");
    ch->Send("|MThere are currently |X" + Utilities::itos((int)Game::GetGame()->users.size()) + "|M players online.|X\n\r");
    ch->Send("|MThere have been |X" + Utilities::itos(Game::GetGame()->total_players_since_boot) + 
        "|M players connected to the server since last boot.|X\n\r");
    ch->Send("|MThe most players that have been online at one time since last boot is |X" + 
        Utilities::itos(Game::GetGame()->max_players_since_boot) + "|M.|X\n\r");
    ch->Send("|MYou are player [|X" + Utilities::itos(Game::GetGame()->total_past_connections) + "|M] connected since May 19th, 2010.|X\n\r\n\r");
   
    ch->Send("WorldUpdate took " + Utilities::dtos(1.0 / Game::GetGame()->worldupdateCount, 10) + " seconds\n\r");
	ch->Send("Bandwidth usage:  Total data sent (" + Utilities::i64tos(Game::GetGame()->totalBytesCompressed) +
		" bytes), without compression would have been (" + Utilities::i64tos(Game::GetGame()->totalBytesUncompressed) + ")\n\r");
}

/*struct searchinfo
{
    std::string name;
    void (*search_func)(Character * ch, std::string argument);
};*/
/*
searchinfo search_table[] = 
{
    { "rooms", Game::SearchRooms },
    { "items", Game::SearchItems }
};*/

//std::map<int, Room *> rooms;
//std::map<int, Area *>::iterator iter;

/*
Game::SearchInfo search_table[] = 
{
    { "rooms", Game::SearchRooms },
    { "items", Game::SearchItems },
    { "characters", Game::SearchCharacters }//,
    //{ "quests", Game::SearchQuests },
    //{ "skills", Game::SearchSkills }
};
*/

void cmd_search(Character * ch, string argument)
{
    string table_name;
    string field_name;
    string conditional;
    argument = Utilities::one_argument(argument, table_name);
    argument = Utilities::one_argument(argument, field_name);
    argument = Utilities::one_argument(argument, conditional);
    int data_type = 0;
    int conditional_type = 0;

    if(table_name.empty() || field_name.empty() || conditional.empty() || argument.empty())
    {
        ch->Send("Syntax: search table_name field_name conditional search_string\n\r");
        return;
    }

    if(!Utilities::str_cmp(conditional, "equals"))
    {
        conditional_type = 1;
    }
    else if(!Utilities::str_cmp(conditional, "notequals"))
    {
        conditional_type = 2;
    }
    else if(!Utilities::str_cmp(conditional, "lessthan"))
    {
        conditional_type = 3;
    }
    else if(!Utilities::str_cmp(conditional, "greaterthan"))
    {
        conditional_type = 4;
    }
    else if(!Utilities::str_cmp(conditional, "contains"))
    {
        conditional_type = 5;
    }
    else
    {
        ch->Send("Valid conditionals: equals notequals lessthan greaterthan contains\n\r");
        return;
    }


    if(Utilities::IsNumber(argument))
    {
        data_type = 1;
    }
    else
    {
        data_type = 2;
    }

    string results;
    int result_count = Game::GetGame()->Search(table_name, field_name, conditional_type, argument, data_type, results);
    
    ch->Send(results);
    ch->Send(Utilities::itos(result_count) + " results found.\n\r");
    return;
}

void cmd_peace(Character * ch, string argument)
{
    for(std::list<Character*>::iterator iter = ch->room->characters.begin(); iter != ch->room->characters.end(); ++iter)
    {
        (*iter)->ExitCombat();
    }
}