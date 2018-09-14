#include "stdafx.h"
#include "mud.h"
#include "CPlayer.h"
#include "CServer.h"
#include "CGame.h"
#include "CCharacter.h"
#include "CUser.h"
#include "CClient.h"
#include "CNPC.h"
#include "CNPCIndex.h"
#include "CItem.h"
#include "CRoom.h"
#include "utils.h"
#include "CLogFile.h"
#include "CmySQLQueue.h"
#include <string>

using std::string;

void cmd_goto(Player * ch, string argument)
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

    if(Game::GetGame()->GetRoom(rnum) != nullptr)
        ch->Message(ch->GetName() + " disappears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);

    if(rnum <= 0 || !ch->ChangeRoomsID(rnum))
    {
        ch->Send("Room " + arg1 + " does not exist.\n\r");
        return;
    }
    ch->Message(ch->GetName() + " appears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    cmd_look(ch, "");
}

void cmd_restore(Player * ch, string argument)
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
            u->character->SetHealth(u->character->GetMaxHealth());
            u->character->SetMana(u->character->GetMaxMana());
			u->character->SetEnergy(u->character->GetMaxEnergy());
			u->character->SetRage(u->character->GetMaxRage());
            if(u->character)
                u->character->SetResurrectTime(0);
        }
    }
}

void cmd_sockets(Player * ch, string argument)
{
    std::list<User *>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); iter++)
    {
        if((*iter)->IsConnected())
        {
            if((*iter)->character)
            {
                ch->Send((*iter)->character->GetName() + " ");
            }
            else
            {
                ch->Send("<no character> ");
            }

            ch->Send((*iter)->GetClient()->GetIPAddress() + "\n\r");
        }
    }
}

void cmd_disconnect(Player * ch, string argument)
{
    ch->Send("cmd_disconnect\n\r");
}

void cmd_shutdown(Player * ch, string argument)
{
    Game::GetGame()->GlobalMessage("Server shutdown.");
    Game::GetGame()->shutdown = true;
}

void cmd_load(Player * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("load npc <#id>\n\r");
        ch->Send("load item <#id> <player>\n\r");
        return;
    }
    string arg1;
    string arg2;
	string arg3;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);
	argument = Utilities::one_argument(argument, arg3);

    if(!Utilities::str_cmp(arg1, "npc"))
    {
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("load npc <#id>\n\r");
            return;
        }
        int id = Utilities::atoi(arg2);
        NPCIndex * charIndex = Game::GetGame()->GetNPCIndex(id);
        if(charIndex == nullptr)
        {
            ch->Send("NPC " + arg2 + " does not exist.\n\r");
            return;
        }
        NPC * newChar = Game::GetGame()->NewNPC(charIndex);
		newChar->leashOrigin = ch->room;
        newChar->ChangeRooms(ch->room);
		ch->Send(newChar->GetName() + " loaded into room.\n\r");
    }
    else if(!Utilities::str_cmp(arg1, "item"))
    {
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("load item <#id>\n\r");
            return;
        }
        int id = Utilities::atoi(arg2);
        Item * itemIndex = Game::GetGame()->GetItem(id);
        if(itemIndex == nullptr)
        {
            ch->Send("Item " + arg2 + " does not exist.\n\r");
            return;
        }
		Player * player_target = nullptr;
		if (!arg3.empty())
		{
			player_target = Game::GetGame()->GetPlayerByName(arg3);
			if (player_target == nullptr)
			{
				ch->Send("Could not find a player with that name.\n\r");
				return;
			}
			player_target->AddItemInventory(itemIndex);
			ch->Send(itemIndex->GetName() + " loaded into " + player_target->GetName() + "'s inventory.\n\r");
			return;
		}
		ch->AddItemInventory(itemIndex);
		ch->Send(itemIndex->GetName() + " loaded into inventory.\n\r");
    }
    else
    {
        ch->Send("load npc <#id>\n\r");
        ch->Send("load item <#id>\n\r");
    }
}

void cmd_purge(Player * ch, string argument)
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

void cmd_transfer(Player * ch, string argument)
{
    string arg1;	//transfer who
	string arg2;
	string arg3;
	//Area * getarea = nullptr;
	Room * getroom = nullptr;
	Player * chtran = nullptr;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
	argument = Utilities::one_argument(argument, arg3);

	// transfer <player name> (to ch->in_room)
    if(!arg1.empty() && ((arg2.empty() && arg3.empty()) || !Utilities::IsNumber(arg2)))
	{
		if(ch == nullptr || ch->room == nullptr)
		{
            LogFile::Log("error", "cmd_transfer: ch == nullptr || ch->room == nullptr");
			return;
		}

		if((chtran = Game::GetGame()->GetPlayerWorld(ch, arg1)) == nullptr)
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

    chtran->Message(chtran->GetName() + " disappears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    chtran->Send("You have been transferred!\n\r");
    chtran->ChangeRoomsID(getroom->id);
    chtran->Message(chtran->GetName() + " appears in a puff of smoke.", Character::MSG_ROOM_NOTCHAR);
    
    cmd_look(chtran, "");
}

void cmd_advance(Player * ch, string argument)
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

    Player * player_target = Game::GetGame()->GetPlayerByName(player_target_arg);
    if(player_target == nullptr)
    {
        ch->Send("Player not found.\n\r");
        return;
    }
    player_target->ApplyExperience(Game::ExperienceForLevel(new_level) - player_target->experience);
}

void cmd_threat(Player * ch, string argument)
{
	if (!ch)
		return;

	ch->Send("Current threat list:\n\r");
	int ctr = 1;
	for (auto iter = ch->threatList.begin(); iter != ch->threatList.end(); ++iter)
	{
		ch->Send(Utilities::itos(ctr) + ". " + iter->ch->GetName() + ", " + Utilities::dtos(iter->damage, 2) + "/" +
			Utilities::dtos(iter->healing, 2) + "/" + Utilities::dtos(iter->threat, 2) + "\n\r");
		ch->Send("    Subscribers: " + iter->ch->DebugPrintSubscribers() + "\n\r");
		if (!iter->ch->threatList.empty())
		{
			for (auto iter2 = iter->ch->threatList.begin(); iter2 != iter->ch->threatList.end(); ++iter2)
			{
				ch->Send("    " + iter2->ch->GetName() + ", " + Utilities::dtos(iter2->damage, 2) + "/" + Utilities::dtos(iter2->healing, 2) + "/" + Utilities::dtos(iter2->threat, 2) + "\n\r");
			}
		}
	}
	//Roll this into this command because lazy
	ch->Send("My Subscribers: ");
	ch->Send(ch->DebugPrintSubscribers() + "\n\r");
}

void cmd_sql(Player * ch, string argument)
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
        //string status;
        //Server::sqlQueue->Write(argument);
		ch->Send("Commands other than select disabled\n\r");
    }
}

void cmd_systeminfo(Player * ch, string argument)
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

void cmd_search(Player * ch, string argument)
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

void cmd_peace(Player * ch, string argument)
{
    for(std::list<Character*>::iterator iter = ch->room->characters.begin(); iter != ch->room->characters.end(); ++iter)
    {
        (*iter)->ExitCombat();
		if((*iter)->IsNPC())
			(*iter)->ClearTarget();
    }
}