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
#include <string>

void cmd_edit(Player * ch, std::string argument)
{
    /*if(!user->character)
		return;
    */
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
			ch->Send("Room " + Utilities::itos(((Room*)(ch->editData))->id) + " created.\n\r");
			return;
		}

		if(ch && ch->room)
		{
            ch->editState = Player::ED_ROOM;
			ch->editData = ch->room;
            ch->Send("Ok.\n\r");
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
                ch->Send("Syntax: edit skill create <name>\n\r");
                return;
            }
            
            ch->editData = Game::GetGame()->CreateSkillAnyID(arg3);
			if (ch->editData == nullptr)
			{
				ch->Send("Error: skill with that name already exists\n\r");
				return;
			}
			ch->editState = Player::ED_SKILL;
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Skill * sk = Game::GetGame()->GetSkill(Utilities::atoi(arg2));
            if(sk == nullptr)
            {
                ch->Send("Skill not found.\n\r");
                return;
            }
            ch->editState = Player::ED_SKILL;
            ch->editData = sk;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit skill <id> || edit skill create <name>\n\r");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "npc"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit npc create <name>\n\r");
                return;
            }
            ch->editState = Player::ED_NPC;
            ch->editData = Game::GetGame()->CreateNPCAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            NPCIndex * tch = Game::GetGame()->GetNPCIndex(Utilities::atoi(arg2));
            if(tch == nullptr)
            {
                ch->Send("NPC " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Player::ED_NPC;
            ch->editData = tch;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit npc <id> || edit npc create <name>\n\r");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "item"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit item create <name>\n\r");
                return;
            }
            ch->editState = Player::ED_ITEM;
            ch->editData = Game::GetGame()->CreateItemAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Item * item = Game::GetGame()->GetItem(Utilities::atoi(arg2));
            if(item == nullptr)
            {
                ch->Send("Item " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Player::ED_ITEM;
            ch->editData = item;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit item <id> || edit item create <name>\n\r");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "quest"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit quest create <name>\n\r");
                return;
            }
            ch->editState = Player::ED_QUEST;
            ch->editData = Game::GetGame()->CreateQuestAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Quest * q = Game::GetGame()->GetQuest(Utilities::atoi(arg2));
            if(q == nullptr)
            {
                ch->Send("Quest " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Player::ED_QUEST;
            ch->editData = q;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit quest <id> || edit quest create <name>\n\r");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "area"))
    {
        if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit area create <name>\n\r");
                return;
            }
            ch->editState = Player::ED_AREA;
            ch->editData = Game::GetGame()->CreateAreaAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Area * a = Game::GetGame()->GetArea(Utilities::atoi(arg2));
            if(a == nullptr)
            {
                ch->Send("Area " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Player::ED_AREA;
            ch->editData = a;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit area <id> || edit area create <name>\n\r");
            return;
        }
    }
	else if(!Utilities::str_cmp(arg1, "help"))
	{
		if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit help create <name>\n\r");
                return;
            }
            ch->editState = Player::ED_HELP;
            ch->editData = Game::GetGame()->CreateHelpAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
		else if(Utilities::IsNumber(arg2))
        {  
			Help * h = Game::GetGame()->GetHelp(Utilities::atoi(arg2));
            if(h == nullptr)
            {
                ch->Send("Help topic '" + arg2 + "' not found.\n\r");
                return;
            }
            ch->editState = Player::ED_HELP;
            ch->editData = h;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit help <id> || edit help create <name>\n\r");
            return;
        }
	}
    else if(!Utilities::str_cmp(arg1, "class"))
    {
        /*if(!Utilities::str_cmp("create", arg2))
        {
            if(arg3.empty())
            {
                ch->Send("Syntax: edit class create <name>\n\r");
                return;
            }
            user->editState = User::ED_QUEST;
            user->editData = Game::GetGame()->CreateQuestAnyID(arg3);
            ch->Send("Ok.\n\r");
        }*/
        //else if(Utilities::IsNumber(arg2))
        if(Utilities::IsNumber(arg2))
        {  
            Class * c = Game::GetGame()->GetClass(Utilities::atoi(arg2));
            if(c == nullptr)
            {
                ch->Send("Class " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Player::ED_CLASS;
            ch->editData = c;
            ch->Send("Ok.\n\r");
        }
        else
        {
            ch->Send("Syntax: edit class <id>\n\r");
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
            ch->Send("That player is not online.\n\r");
            return;
        }
        //have to get creative to ensure we're not trying to edit bad data if a player quits
        ch->editData = (void*)(new std::string(arg2.c_str()));
        ch->editState = Player::ED_PLAYER;
        ch->Send("Ok.\n\r");
        //todo
		//to edit a player, the name must be specified: edit player <name>
		//if the player is online, fill a new character/playerdata structure with that info
		// this is now editable
		//when saving, if player is online, change values immediately
		// if player is offline, overwrite the save file
	}
	else if(!Utilities::str_cmp(arg1, "exit"))
	{
		ch->Send("Exiting editor.\n\r");
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
				ch->Send("You're not currently editing anything.\n\r");
				break;
            case Player::ED_SKILL:
                Game::GetGame()->SaveSkills();
                ch->Send("Skills saved.\n\r");
                break;
			case Player::ED_ROOM:
				Game::GetGame()->SaveRooms();
				ch->Send("Rooms saved.\n\r");
				break;
            case Player::ED_NPC:
                Game::GetGame()->SaveNPCIndex();
                ch->Send("NPCs saved.\n\r");
                break;
            case Player::ED_ITEM:
                Game::GetGame()->SaveItems();
                ch->Send("Items saved.\n\r");
                break;
            case Player::ED_QUEST:
                Game::GetGame()->SaveQuests();
                ch->Send("Quests saved.\n\r");
                break;
            case Player::ED_AREA:
                Game::GetGame()->SaveAreas();
                ch->Send("Areas saved.\n\r");
                break;
            case Player::ED_CLASS:
                Game::GetGame()->SaveClasses();
                ch->Send("Classes saved.\n\r");
                break;
			case Player::ED_HELP:
				if(((Help*)ch->editData)->remove)
				{
					ch->editState = Player::ED_NONE;
					ch->editData = nullptr;
				}
                Game::GetGame()->SaveHelp();
                ch->Send("Help saved.\n\r");
                break;
            case Player::ED_PLAYER:
				break;
		}
	}
	else
	{
		//this should be a help file
		std::string syntax = "";
		syntax += "Syntax: Edit: room\n\r";
        syntax += "              skill <id> || skill create <name>\n\r";
        syntax += "              npc <id> || npc create <name>\n\r";
        syntax += "              item <id> || item create <name>\n\r";
        syntax += "              quest <id> || quest create <name>\n\r";
        syntax += "              area <id> || area create <name>\n\r";
        syntax += "              class <id>\n\r";
		syntax += "              help <name> || help create <name>\n\r";
		syntax += "              player <name>\n\r";
		syntax += "              save\n\r";
		syntax += "              exit\n\r";
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

	ch->Send("Name:      [" + pRoom->name + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pRoom->id) + "]\n\r");
    ch->Send("Area:      [" + Utilities::itos(pRoom->area) + "]\n\r");

	ch->Send("Flags: ");
    std::vector<int>::iterator flagiter;
    for(flagiter = pRoom->flags.begin(); flagiter != pRoom->flags.end(); ++flagiter)
    {
        ch->Send(Room::flag_table[(*flagiter)].flag_name + " ");
    }
    ch->Send("\n\r");

	ch->Send("Exits:\n\r");
    for(int i = 0; i < Exit::DIR_LAST; i++)
	{
		if(pRoom->exits[i] && pRoom->exits[i]->to)
		{
            ch->Send("-" + std::string(Exit::exitNames[i]) + " to [" + Utilities::itos(pRoom->exits[i]->to->id) + "]\n\r");
		}
	}
	ch->Send("Description:\n\r" + pRoom->description + "\n\r");

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
	characters += "]\n\r";
	ch->Send(characters);

    //Resets
    if(!pRoom->resets.empty())
        ch->Send("Resets:\n\r");
    for(std::map<int, Reset*>::iterator iter = pRoom->resets.begin(); iter != pRoom->resets.end(); iter++)
    {
        Reset * r = (*iter).second;
        ch->Send(Utilities::itos(r->id) + ". " + (r->type == 1 ? "NPC " + Utilities::itos(r->targetID) : "ITEM " + Utilities::itos(r->targetID))
            + ", interval: " + Utilities::itos(r->interval) + ", wander_distance: " + Utilities::itos(r->wanderDistance)
            + ", leash_distance: " + Utilities::itos(r->leashDistance) + " " + (r->removeme ? "|RX|X" : "") + "\n\r");
    }

    //Triggers
    if(!pRoom->triggers.empty())
        ch->Send("Triggers:\n\r");
    for(std::map<int, Trigger>::iterator iter = pRoom->triggers.begin(); iter != pRoom->triggers.end(); iter++)
    {
        Trigger * t = &((*iter).second);
        ch->Send(Utilities::itos(t->id) + ". Type: " + Utilities::itos(t->GetType()) + " Argument: " + t->GetArgument() + 
                  "Function name: " + t->GetFunction() + "\n\r");
    }
}

void roomEditCmd_name(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

	if(argument.empty())
	{
		pRoom->name.clear();
		ch->Send("Name cleared.\n\r");
		return;
	}

	if(argument.length() > 50)
	{
		ch->Send("Maximum length is 50 characters.\n\r");
		return;
	}

	pRoom->name = argument;
    pRoom->changed = true;
	ch->Send("Name set.\n\r");
}

void roomEditCmd_area(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("area <#>\n\r");
        return;
    }
    int area = Utilities::atoi(arg1);
    if(area < 0)
    {
        ch->Send("area must be >= 0.\n\r");
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

    ch->Send( "Syntax:  desc\n\r" );
}

void roomEditCmd_flag(Player * ch, std::string argument)
{
    Room * pRoom = (Room *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("flags: recall nopvp\n\r");
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
                ch->Send(arg1 + " flag removed.\n\r");
            }
            else
            {
                Utilities::FlagSet(pRoom->flags, Room::flag_table[i].flag);
                ch->Send(arg1 + " flag set.\n\r");
            }
            pRoom->changed = true;
            return;
        }
    }
    ch->Send("flags: recall nopvp\n\r");
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
            ch->Send("Reset " + Utilities::itos(resetnum) + " not found.\n\r");
            return;
        }

        /*if(!Utilities::str_cmp(arg2, "type"))
        {
            if(!Utilities::str_cmp(arg3, "npc"))
            {
                r->type = 1;
                pRoom->changed = true;
                ch->Send("Reset " + Utilities::itos(resetnum) + " type changed to npc.\n\r");
                return;
            }
            else if(!Utilities::str_cmp(arg3, "obj"))
            {
                r->type = 2;
                pRoom->changed = true;
                ch->Send("Reset " + Utilities::itos(resetnum) + " type changed to obj.\n\r");
                return;
            }
            else
            {
                ch->Send("reset <#> type npc||obj\n\r");
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
                        ch->Send("NPC with that id does not exist.\n\r");
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
							ch->Send("Item with that id does not exist.\n\r");
							return;
						}
					}
                }
            }
            else
            {
                ch->Send("reset <#> target_id <#id>\n\r");
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
                    ch->Send("Interval must be >= 0\n\r");
                    return;
                }
                r->interval = newinterval;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\n\r");
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
                    ch->Send("wander_distance must be >= 0\n\r");
                    return;
                }
                r->wanderDistance = newwd;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\n\r");
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
                    ch->Send("leash_distance must be >= 0\n\r");
                    return;
                }
                r->leashDistance = newld;
                pRoom->changed = true;
            }
            else
            {
                ch->Send("reset <#> interval <seconds>\n\r");
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
                    ch->Send("NPC with that id does not exist.\n\r");
                    return;
                }
                Reset * new_reset = new Reset(pRoom, 1, id);
                pRoom->AddReset(new_reset);
                ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\n\r");
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
					ch->Send("Item with that id does not exist.\n\r");
					return;
				}
				Reset * new_reset = new Reset(pRoom, 2, id);
				pRoom->AddReset(new_reset);
				ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\n\r");
			}
		}
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("reset delete <#id>\n\r");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Reset * deleteme = pRoom->GetReset(delid);
        if(deleteme != nullptr)
        {
            if(!deleteme->removeme)
            {
                deleteme->removeme = true;
                ch->Send("Reset marked for deletion. Save rooms to delete.\n\r");
            }
            else
            {
                deleteme->removeme = false;
                ch->Send("Reset unmarked for deletion.\n\r");
            }
            pRoom->changed = true;
            return;
        }
    }
    else
    {
        ch->Send("Syntax: reset add npc||obj id\n\r");
        ch->Send("        reset delete id\n\r");
        ch->Send("        reset <#> target_id||interval||wander_distance||leash_distance\n\r");
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
            ch->Send("Trigger " + Utilities::itos(trignum) + " not found.\n\r");
            return;
        }

        if(!Utilities::str_cmp(arg2, "type"))
        {
            int trigtype;
            if(Utilities::IsNumber(arg3) &&  (trigtype = Utilities::atoi(arg3)) >= 0 && trigtype < Trigger::TRIG_LAST)
            {
                t->SetType(trigtype);
                pRoom->changed = true;
                ch->Send("Trigger " + Utilities::itos(trignum) + " type changed to " + Utilities::itos(trigtype) + ".\n\r");
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
            ch->Send("Trigger " + Utilities::itos(trignum) + " argument changed to " + arg3 + ".\n\r");
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
            ch->Send("Trigger " + Utilities::itos(trignum) + " function changed to " + arg3 + ".\n\r");
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
        ch->Send("Added trigger #" + Utilities::itos(new_trig.id) + "\n\r");
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("trigger delete <#id>\n\r");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Trigger * deleteme = pRoom->GetTrigger(delid);
        if(deleteme != nullptr)
        {
            if(!deleteme->removeme)
            {
                deleteme->removeme = true;
                ch->Send("Trigger marked for deletion. Save rooms to delete.\n\r");
            }
            else
            {
                deleteme->removeme = false;
                ch->Send("Trigger unmarked for deletion.\n\r");
            }
            pRoom->changed = true;
            return;
        }
        else
        {
            ch->Send("Trigger " + Utilities::itos(delid) + " not found.\n\r");
            return;
        }
    }
    else
    {
        ch->Send("Syntax: trigger add <type> <argument>\n\r");
        ch->Send("        trigger delete id\n\r");
        ch->Send("        trigger <#> type||argument||script||function\n\r");
    }
}

void roomEditCmd_create(Player * ch, std::string argument)
{
    if(argument.empty() || !Utilities::IsNumber(argument))
    {
        ch->Send("Syntax: create [room id > 0]\n\r");
        return;
    }

    int value = Utilities::atoi(argument);

    if(value <= 0)
    {
	    ch->Send("Syntax: create [room id > 0]\n\r");
	    return;
    }

    Room * pRoom;
    if((pRoom = Game::GetGame()->GetRoom(value)) != nullptr)
    {
	    ch->Send( "REdit: Room id already exists.\n\r");
	    return;
    }

    pRoom = Game::GetGame()->CreateRoom(value);

    ch->editData = (void*)pRoom;
    ch->ChangeRooms(pRoom);

    ch->Send("Room created.\n\r");
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
	        ch->Send("REdit: Cannot delete a null exit.\n\r");
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
	    ch->Send("Exit unlinked.\n\r");
	    return;
    }

    if(!Utilities::str_cmp(command, "link"))
    {
        if(pRoom->exits[direction] && pRoom->exits[direction]->to)
        {
            ch->Send("REdit: Exit already exists.\n\r");
	        return;

        }

        if(arg.empty() || (Utilities::str_cmp(arg, "new") && !Utilities::IsNumber(arg)))
        {
            ch->Send("Syntax: [direction] link [room id]\n\r");
            ch->Send("        [direction] link new\n\r");
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
	        ch->Send("REdit: Cannot link to non-existant room.\n\r");
	        return;
	    }

	    if(pToRoom->exits[Exit::exitOpposite[direction]] && pToRoom->exits[Exit::exitOpposite[direction]]->to)
	    {
	        ch->Send("REdit: Remote side's exit already exists.\n\r");
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
	    ch->Send("Two-way link established.\n\r");
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

	ch->Send("long_name: [" + pSkill->long_name + "]\n\r");
    ch->Send("name:      [" + pSkill->name + "]\n\r");
    ch->Send("id:        [" + Utilities::itos(pSkill->id) + "]\n\r");
    ch->Send("cast_time: [" + Utilities::dtos(pSkill->castTime, 2) + " seconds]\n\r");
	ch->Send("flags: ");
	std::vector<int>::iterator flagiter;
	for (flagiter = pSkill->flags.begin(); flagiter != pSkill->flags.end(); ++flagiter)
	{
		ch->Send(Skill::flag_table[(*flagiter)].flag_name + " ");
	}
	ch->Send("\n\r");
	ch->Send("interrupt_flags: [");
	for (int i = 0; i < (int)pSkill->interruptFlags.size(); ++i)
	{
		if(pSkill->interruptFlags[i])
			ch->Send(Utilities::itos(i) + " ");
	}
	ch->Send("]\n\r");
    ch->Send("cooldown:  [" + Utilities::dtos(pSkill->cooldown, 2) + " seconds]\n\r");
    ch->Send("target_type:   [" + Utilities::itos(pSkill->targetType) + "]\n\r");
    ch->Send("function_name: [" + pSkill->function_name + "]\n\r");
    ch->Send("description:   [" + pSkill->description + "]\n\r");
	ch->Send("cost_desc:   [" + pSkill->costDescription + "]\n\r");
    ch->SendBW("Cost function:\n\r" + pSkill->costFunction + "\n\r");
    ch->SendBW("Cast script:\n\r" + pSkill->castScript + "\n\r");
    ch->SendBW("Apply script:\n\r" + pSkill->applyScript + "\n\r");
    ch->SendBW("Tick script:\n\r" + pSkill->tickScript + "\n\r");
    ch->SendBW("Remove script:\n\r" + pSkill->removeScript + "\n\r");
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
		ch->Send("Maximum length is 100 characters.\n\r");
		return;
	}

    pSkill->name = Utilities::ToLower(argument);
    pSkill->changed = true;
	ch->Send("Name set.\n\r");
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
        ch->Send("Maximum length is 100 characters.\n\r");
        return;
    }
    pSkill->long_name = argument;
    pSkill->changed = true;
    ch->Send("Long name set.\n\r");
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
		ch->Send("Maximum length is 106 characters.\n\r");
		return;
	}
    //TODO: more checks here   No whitespace, one_argument should have solved that?

	pSkill->function_name = arg;
    pSkill->changed = true;
	ch->Send("function_name set.\n\r");
}

void skillEditCmd_target_type(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("target_type 0(TARGET_SELF), 1(TARGET_OTHER), 2(TARGET_HOSTILE), 3(TARGET_ANY), 4(TARGET_FRIENDLY), 5(TARGET_NONE), 6(TARGET_PASSIVE)\n\r");
        return;
    }
    pSkill->changed = true;
    pSkill->targetType = (Skill::TargetType)Utilities::atoi(arg1);
}

void skillEditCmd_reload(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

	ch->Send("Reloading LUA scripts.\n\r");

	try {
		Server::lua.script(pSkill->costFunction.c_str());
		Server::lua.script(pSkill->castScript.c_str());
		Server::lua.script(pSkill->applyScript.c_str());
		Server::lua.script(pSkill->tickScript.c_str());
		Server::lua.script(pSkill->removeScript.c_str());
	}
	catch (const std::exception & e)
	{
		LogFile::Log("error", e.what());
	}

	/*
    if(luaL_dostring(Server::luaState, pSkill->costFunction.c_str()))
    {
        LogFile::Log("error", std::string("Cost script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->castScript.c_str()))
    {
        LogFile::Log("error", std::string("Cast script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->applyScript.c_str()))
    {
        LogFile::Log("error", std::string("Apply script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->tickScript.c_str()))
    {
        LogFile::Log("error", std::string("Tick script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->removeScript.c_str()))
    {
        LogFile::Log("error", std::string("Remove script: ") + lua_tostring(Server::luaState, -1));
    }
	*/
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
		ch->Send("Maximum length is 255 characters.\n\r");
		return;
	}

	pSkill->description = argument;
    pSkill->changed = true;
	ch->Send("description set.\n\r");
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
		ch->Send("Maximum length is 255 characters.\n\r");
		return;
	}

	pSkill->costDescription = argument;
	pSkill->changed = true;
	ch->Send("cost_description set.\n\r");
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

void skillEditCmd_cost_function(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;


        StringEdit::string_append( ch->user, &pSkill->costFunction );
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
        ch->Send("Cast time must be a number >= 0.\n\r");
        return;
    }
    pSkill->castTime = ct;
    pSkill->changed = true;
    ch->Send("cast_time set.\n\r");
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
		ch->Send("Interrupt flags: 0 INTERRUPT_MOVE, 1 INTERRUPT_HIT, 2 INTERRUPT_NOPUSHBACK\n\r");
		return;
	}
	pSkill->interruptFlags.set(flag);
	pSkill->changed = true;
	ch->Send("interrupt_flag set.\n\r");
}

void skillEditCmd_cooldown(Player * ch, std::string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    std::string arg;
    Utilities::one_argument(argument, arg);

    double cd;
    if(!Utilities::IsNumber(arg) || (cd = Utilities::atof(arg)) < 0)
    {
        ch->Send("Cooldown must be a number >= 0.\n\r");
        return;
    }
    pSkill->cooldown = cd;
    pSkill->changed = true;
    ch->Send("Cooldown set.\n\r");
}

void skillEditCmd_flag(Player * ch, std::string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (arg1.empty())
	{
		ch->Send("flags: nogcd gcdimmune\n\r");
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
				ch->Send(arg1 + " flag removed.\n\r");
			}
			else
			{
				Utilities::FlagSet(pSkill->flags, Skill::flag_table[i].flag);
				ch->Send(arg1 + " flag set.\n\r");
			}
			pSkill->changed = true;
			return;
		}
	}
	ch->Send("flags: nogcd gcdimmune\n\r");
}

void npcEditCmd_show(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

	if (!pChar)
	{
		LogFile::Log("error", "npcEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("Name:      [" + pChar->name + "]\n\r");
	ch->Send("Keywords:	 [" + pChar->keywords + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pChar->id) + "]\n\r");
    ch->Send("Level:     [" + Utilities::itos(pChar->level) + "]\n\r");
    ch->Send("Title:     [" + pChar->title + "]\n\r");
    ch->Send("Gender:    [" + Utilities::itos(pChar->gender) + "]\n\r");
    ch->Send("Health:    [" + Utilities::itos(pChar->maxHealth) + "]\n\r");
    ch->Send("Mana:      [" + Utilities::itos(pChar->maxMana) + "]\n\r");
    ch->Send("Energy:    [" + Utilities::itos(pChar->maxEnergy) + "]\n\r");
    ch->Send("attack_speed: [" + Utilities::dtos(pChar->npcAttackSpeed, 2) + "]\n\r");
    ch->Send("damage_low:   [" + Utilities::itos(pChar->npcDamageLow) + "]\n\r");
    ch->Send("damage_high:  [" + Utilities::itos(pChar->npcDamageHigh) + "]\n\r");
    double dps = ((pChar->npcDamageLow + pChar->npcDamageHigh) / 2.0) / pChar->npcAttackSpeed;
    ch->Send("(" + Utilities::dtos(dps, 2) + " damage per second)\n\r");
	ch->Send("speechtext: " + pChar->speechText + "\n\r");

    //double movementSpeed; //default = 3 rooms per second
    //double lastMoveTime;

    ch->Send("Starts quests: ");
    std::vector<Quest *>::iterator questiter;
    for(questiter = pChar->questStart.begin(); questiter != pChar->questStart.end(); ++questiter)
    {
        ch->Send(Utilities::itos((*questiter)->id) + " ");
    }
    ch->Send("\n\r");
    ch->Send("Ends quests: ");
    for(questiter = pChar->questEnd.begin(); questiter != pChar->questEnd.end(); ++questiter)
    {
        ch->Send(Utilities::itos((*questiter)->id) + " ");
    }
    ch->Send("\n\r");

    ch->Send("Flags: ");
    std::vector<int>::iterator flagiter;
    for(flagiter = pChar->flags.begin(); flagiter != pChar->flags.end(); ++flagiter)
    {
        ch->Send(NPCIndex::flag_table[(*flagiter)].flag_name + " ");
    }
    ch->Send("\n\r");

    ch->Send("Drops:\n\r");
    std::list<NPCIndex::DropData>::iterator dropiter;
    int ctr = 1;
    for(dropiter = pChar->drops.begin(); dropiter != pChar->drops.end(); ++dropiter)
    {
        ch->Send(Utilities::itos(ctr++) + ". " + Utilities::itos((*dropiter).percent) + "%: ");
        for(int i = 0; i < (int)(*dropiter).id.size(); i++)
        {
            ch->Send(Utilities::itos((*dropiter).id[i]) + " ");
        }
        ch->Send("\n\r");
    }
    ch->Send("\n\r");

    //Triggers
    if(!pChar->triggers.empty())
        ch->Send("Triggers:\n\r");
    for(std::map<int, Trigger>::iterator iter = pChar->triggers.begin(); iter != pChar->triggers.end(); iter++)
    {
        Trigger * t = &((*iter).second);
        ch->Send(Utilities::itos(t->id) + ". Type: " + Utilities::itos(t->GetType()) + " Argument: " + t->GetArgument() + 
                  " Function name: " + t->GetFunction() + "\n\r");
		ch->Send("Script: " + t->GetScript() + "\n\r");
    }
}

void npcEditCmd_name(Player * ch, std::string argument)
{
    NPCIndex * pChar = (NPCIndex*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\n\r");
        return;
    }

    pChar->name = argument;
    pChar->changed = true;
    ch->Send("name set.\n\r");
}

void npcEditCmd_title(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set title to what?\n\r");
        return;
    }

    pChar->title = argument;
    pChar->changed = true;
    ch->Send("title set.\n\r");
}

void npcEditCmd_flag(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("flags: friendly neutral aggressive trainer guild repair\n\r");
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
                ch->Send(arg1 + " flag removed.\n\r");
            }
            else
            {
                Utilities::FlagSet(pChar->flags, NPCIndex::flag_table[i].flag);
                ch->Send(arg1 + " flag set.\n\r");
            }
            pChar->changed = true;
            return;
        }
    }
    ch->Send("flags: friendly neutral aggressive trainer guild repair\n\r");
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
        ch->Send("Syntax: drop add <percent>\n\r");
        ch->Send("        drop remove <#>\n\r");
        ch->Send("        drop <#> add||remove <#>\n\r");
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
                ch->Send("Percent must be from 1 to 100.\n\r");
                return;
            }
			NPCIndex::DropData dd;
            dd.percent = percent;
            pChar->drops.push_back(dd);
            ch->Send("Added drop with " + Utilities::itos(percent) + "% chance.\n\r");
            return;
        }
    }
    else if(!Utilities::str_cmp(arg1, "remove"))
    {
        if(pChar->drops.size() == 0)
        {
            ch->Send("No drops to remove.\n\r");
            return;
        }
        if(Utilities::IsNumber(arg2))
        {
            int remove = Utilities::atoi(arg2);
            if(remove <= 0 || remove > (int)pChar->drops.size())
            {
                ch->Send("Drop to remove must be from 1 to " + Utilities::itos((int)pChar->drops.size()) + ".\n\r");
                return;
            }
            int ctr = 1;
            for(std::list<NPCIndex::DropData>::iterator iter = pChar->drops.begin(); iter != pChar->drops.end(); ++iter)
            {
                if(ctr++ == remove)
                {
                    pChar->drops.erase(iter);
                    ch->Send("Drop " + Utilities::itos(remove) + " removed.\n\r");
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
            ch->Send("Drop to modify must be from 1 to " + Utilities::itos((int)pChar->drops.size()) + ".\n\r");
            return;
        }
        if(!Utilities::IsNumber(arg3))
        {
            ch->Send("Syntax: drop <#> add||remove <#>\n\r");
            return;
        }
        int itemid = Utilities::atoi(arg3);
        if(itemid <= 0)
        {
            ch->Send("Item ID must be > 0.\n\r");
            return;
        }
        Item * item;
        if((item = Game::GetGame()->GetItem(itemid)) == nullptr)
        {
            ch->Send("That item does not exist.\n\r");
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
            ch->Send("Added item ID " + Utilities::itos(itemid) + " to drop " + Utilities::itos(modify) + ".\n\r");
            return;
        }
        else if(!Utilities::str_cmp(arg2, "remove"))
        {
            for(std::vector<int>::iterator iter = dd->id.begin(); iter != dd->id.end(); ++iter)
            {
                if((*iter) == itemid)
                {
                    dd->id.erase(iter);
                    ch->Send("Removed item ID " + Utilities::itos(itemid) + " from drop " + Utilities::itos(modify) + ".\n\r");
                    return;
                }
            }
            ch->Send("Could not remove item ID " + Utilities::itos(itemid) + " from drop " + Utilities::itos(modify) + ".\n\r");
            return;
        }
    }
    ch->Send("Syntax: drop add <percent>\n\r");
    ch->Send("        drop remove <#>\n\r");
    ch->Send("        drop <#> add||remove <#>\n\r");
}

void npcEditCmd_level(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("level <#>\n\r");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0)
    {
        ch->Send("Level must be > 0.\n\r");
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
        ch->Send("gender <1|2>\n\r");
        return;
    }
    int gender = Utilities::atoi(arg1);
    if(gender != 1 && gender != 2)
    {
        ch->Send("Gender must be 1 for male, 2 for female.\n\r");
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
        ch->Send("health <#>\n\r");
        return;
    }
    int health = Utilities::atoi(arg1);
    if(health < 1)
    {
        ch->Send("Health must be > 0.\n\r");
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
        ch->Send("mana <#>\n\r");
        return;
    }
    int mana = Utilities::atoi(arg1);
    if(mana < 1)
    {
        ch->Send("Mana must be > 0.\n\r");
        return;
    }
    pChar->changed = true;
	pChar->maxMana = mana;
}

void npcEditCmd_attackSpeed(Player * ch, std::string argument)
{
	NPCIndex * pChar = (NPCIndex *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("attack_speed <#>\n\r");
        return;
    }
    double attackSpeed = Utilities::atof(arg1);
    if(attackSpeed <= 0)
    {
        ch->Send("attack_speed must be > 0\n\r");
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
        ch->Send("damage_low <#>\n\r");
        return;
    }
    int damage_low = Utilities::atoi(arg1);
    if(damage_low < 1 || damage_low > pChar->npcDamageHigh)
    {
        ch->Send("damage_low must be > 0, < damage_high\n\r");
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
        ch->Send("damage_high <#>\n\r");
        return;
    }
    int damage_high = Utilities::atoi(arg1);
    if(damage_high < 1 || damage_high < pChar->npcDamageLow)
    {
        ch->Send("damage_high must be > 0, > damage_low\n\r");
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
		ch->Send("Set speechtext to what?\n\r");
		return;
	}

	pChar->speechText = argument;
	pChar->changed = true;
	ch->Send("speechText set.\n\r");
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
            ch->Send("Trigger " + Utilities::itos(trignum) + " not found.\n\r");
            return;
        }

        if(!Utilities::str_cmp(arg2, "type"))
        {
            int trigtype;
            if(Utilities::IsNumber(arg3) &&  (trigtype = Utilities::atoi(arg3)) >= 0 && trigtype < Trigger::TRIG_LAST)
            {
                t->SetType(trigtype);
                pChar->changed = true;
                ch->Send("Trigger " + Utilities::itos(trignum) + " type changed to " + Utilities::itos(trigtype) + ".\n\r");
                return;
            }
            else
            {
                ch->Send("NPC trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC, 6 TIMER");
                return;
            }
        }
        else if(!Utilities::str_cmp(arg2, "argument"))
        {
            t->SetArgument(arg3);
            pChar->changed = true;
            ch->Send("Trigger " + Utilities::itos(trignum) + " argument changed to " + arg3 + ".\n\r");
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
            ch->Send("Trigger " + Utilities::itos(trignum) + " function changed to " + arg3 + ".\n\r");
            return;
        }
    }
    //add <type> <argument>
	else if(!Utilities::str_cmp(arg1, "add"))
	{
        int trigtype;
        if(!Utilities::IsNumber(arg2) || (trigtype = Utilities::atoi(arg2)) < 0 || trigtype >= Trigger::TRIG_LAST)
        {
            ch->Send("NPC trigger types are: 0 ENTER_CHAR, 1 ENTER_NPC, 2 ENTER_PC, 3 EXIT_CHAR, 4 EXIT_NPC, 5 EXIT_PC, 6 TIMER");
            return;
        }
        Trigger new_trig;
        new_trig.SetType(trigtype);
        new_trig.SetArgument(arg3);
        pChar->AddTrigger(new_trig);
        ch->Send("Added trigger #" + Utilities::itos(new_trig.id) + "\n\r");
    }
    else if(!Utilities::str_cmp(arg1, "delete"))
	{
        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("trigger delete <#id>\n\r");
            return;
        }
        int delid = Utilities::atoi(arg2);
        Trigger * deleteme = pChar->GetTrigger(delid);
        if(deleteme != nullptr)
        {
            if(!deleteme->removeme)
            {
                deleteme->removeme = true;
                ch->Send("Trigger marked for deletion. Save npcs to delete.\n\r");
            }
            else
            {
                deleteme->removeme = false;
                ch->Send("Trigger unmarked for deletion.\n\r");
            }
            pChar->changed = true;
            return;
        }
        else
        {
            ch->Send("Trigger " + Utilities::itos(delid) + " not found.\n\r");
            return;
        }
    }
    else
    {
        ch->Send("Syntax: trigger add <type> <argument>\n\r");
        ch->Send("        trigger delete id\n\r");
        ch->Send("        trigger <#> type||argument||script||function\n\r");
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

	ch->Send("Name:           [" + pItem->name + "]\n\r");
	ch->Send("Keywords:       [" + pItem->keywords + "]\n\r");
	ch->Send("inroom_name:    [" + pItem->inroom_name + "]\n\r");
    ch->Send("ID:             [" + Utilities::itos(pItem->id) + "]\n\r");
    ch->Send("char_level:     [" + Utilities::itos(pItem->charLevel) + "]\n\r");
    ch->Send("item_level:     [" + Utilities::itos(pItem->itemLevel) + "]\n\r");
    ch->Send("quality:        [" + Utilities::itos(pItem->quality) + "]\n\r");
    ch->Send("equip_location: [" + Utilities::itos(pItem->equipLocation) + "]\n\r");
    ch->Send("binds:          [" + Utilities::itos(pItem->binds) + "]\n\r");
    ch->Send("type:           [" + Utilities::itos(pItem->type) + "]\n\r");
	ch->Send("skillid:        [" + Utilities::itos(pItem->useSkillID) + "]\n\r");
    ch->Send("quest item:     [" + Utilities::itos(pItem->quest) + "]\n\r");
    ch->Send("unique:         [" + Utilities::itos(pItem->unique) + "]\n\r");
    ch->Send("armor:          [" + Utilities::itos(pItem->armor) + "]\n\r");
	ch->Send("stats:  agility:[" + Utilities::itos(pItem->agility) + "] intellect:[" + Utilities::itos(pItem->intellect) + "] strength:[" + Utilities::itos(pItem->strength) + "]\n\r");
	ch->Send("        stamina:[" + Utilities::itos(pItem->stamina) + "] wisdom:[" + Utilities::itos(pItem->wisdom) + "] spirit:[" + Utilities::itos(pItem->spirit) + "]\n\r");
    ch->Send("durability:     [" + Utilities::itos(pItem->durability) + "]\n\r");
    ch->Send("damage_low:     [" + Utilities::itos(pItem->damageLow) + "]\n\r");
    ch->Send("damage_high:    [" + Utilities::itos(pItem->damageHigh) + "]\n\r");
    ch->Send("speed:          [" + Utilities::dtos(pItem->speed, 2) + "]\n\r");
    if(pItem->speed != 0)
        ch->Send("(damage per second): " + Utilities::dtos(((((pItem->damageHigh - pItem->damageLow) / 2.0) + pItem->damageLow) / pItem->speed), 2) + "\n\r");
    ch->Send("value:          [" + Utilities::itos(pItem->value) + "]\n\r");
}

void itemEditCmd_name(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\n\r");
        return;
    }

    pItem->name = argument;
    pItem->changed = true;
    ch->Send("name set.\n\r");
}

void itemEditCmd_keywords(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set keywords to what?\n\r");
		return;
	}

	pItem->keywords = argument;
	pItem->changed = true;
	ch->Send("keywords set.\n\r");
}

void itemEditCmd_inroom_name(Player * ch, std::string argument)
{
	Item * pItem = (Item *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set inroom_name to what?\n\r");
		return;
	}

	pItem->inroom_name = argument;
	pItem->changed = true;
	ch->Send("inroom_name set.\n\r");
}

void itemEditCmd_item_level(Player * ch, std::string argument)
{
    Item * pItem = (Item *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("item_level <#>\n\r");
        return;
    }
    int item_level = Utilities::atoi(arg1);
    if(item_level <= 0)
    {
        ch->Send("item_level must be > 0.\n\r");
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
        ch->Send("char_level <#>\n\r");
        return;
    }
    int char_level = Utilities::atoi(arg1);
    if(char_level <= 0)
    {
        ch->Send("char_level must be > 0.\n\r");
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
        ch->Send("quality: 0 POOR, 1 COMMON, 2 UNCOMMON, 3 RARE, 4 EPIC, 5 LEGENDARY, 6 ARTIFACT\n\r");
        return;
    }
    int quality = Utilities::atoi(arg1);
    if(quality < 0 || quality >= Item::QUALITY_LAST)
    {
        ch->Send("quality: 0 POOR, 1 COMMON, 2 UNCOMMON, 3 RARE, 4 EPIC, 5 LEGENDARY, 6 ARTIFACT\n\r");
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
        ch->Send("equip_location: 0 NONE, 1 HEAD, 2 NECK, 3 SHOULDER, 4 BACK, 5 CHEST, 6 WRIST, 7 HANDS, 8 WAIST, 9 LEGS, 10 FEET\n\r");
        ch->Send("11 FINGER, 12 TRINKET, 13 OFFHAND, 14 MAINHAND, 15 ONEHAND, 16 TWOHAND\n\r");
        return;
    }
    int wear_location = Utilities::atoi(arg1);
    if(wear_location < 0 || wear_location >= Item::EQUIP_LAST)
    {
        ch->Send("equip_location: 0 NONE, 1 HEAD, 2 NECK, 3 SHOULDER, 4 BACK, 5 CHEST, 6 WRIST, 7 HANDS, 8 WAIST, 9 LEGS, 10 FEET\n\r");
        ch->Send("11 FINGER, 12 TRINKET, 13 OFFHAND, 14 MAINHAND, 15 ONEHAND, 16 TWOHAND\n\r");
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
        ch->Send("binds: 0 BIND_NONE, 1 BIND_PICKUP, 2 BIND_EQUIP\n\r");
        return;
    }
    int binds = Utilities::atoi(arg1);
    if(binds < 0 || binds >= Item::BIND_LAST)
    {
        ch->Send("binds: 0 BIND_NONE, 1 BIND_PICKUP, 2 BIND_EQUIP\n\r");
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
        ch->Send("type: 0 TYPE_ARMOR_CLOTH, 1 TYPE_ARMOR_LEATHER, 2 TYPE_ARMOR_MAIL, 3 TYPE_ARMOR_PLATE, 4 TYPE_WEAPON_SWORD, 5 TYPE_WEAPON_DAGGER\n\r");
        ch->Send("      6 TYPE_WEAPON_MACE, 7 TYPE_WEAPON_AXE, 8 TYPE_WEAPON_POLEARM, 9 TYPE_WEAPON_STAFF, 10 TYPE_CONTAINER, 11 TYPE_FOOD\n\r");
        ch->Send("      12 TYPE_CONSUMABLE, 13 TYPE_MISC\n\r");
        return;
    }
    int type = Utilities::atoi(arg1);
    if(type < 0 || type >= Item::TYPE_LAST)
    {
        ch->Send("type: 0 TYPE_ARMOR_CLOTH, 1 TYPE_ARMOR_LEATHER, 2 TYPE_ARMOR_MAIL, 3 TYPE_ARMOR_PLATE, 4 TYPE_WEAPON_SWORD, 5 TYPE_WEAPON_DAGGER\n\r");
        ch->Send("      6 TYPE_WEAPON_MACE, 7 TYPE_WEAPON_AXE, 8 TYPE_WEAPON_POLEARM, 9 TYPE_WEAPON_STAFF, 10 TYPE_CONTAINER, 11 TYPE_FOOD\n\r");
        ch->Send("      12 TYPE_CONSUMABLE, 13 TYPE_MISC\n\r");
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
		ch->Send("skillid: #\n\r");
		return;
	}
	int skillid = Utilities::atoi(arg1);
	if (skillid < 0)
	{
		ch->Send("skillid: >= 0\n\r");
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
        ch->Send("quest: 0,1\n\r");
        return;
    }
    int quest = Utilities::atoi(arg1);
    if(quest < 0 || quest > 1)
    {
        ch->Send("quest: 0,1\n\r");
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
        ch->Send("unique: 0,1\n\r");
        return;
    }
    int unique = Utilities::atoi(arg1);
    if(unique < 0 || unique > 1)
    {
        ch->Send("unique: 0,1\n\r");
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
        ch->Send("armor: integer >= 0\n\r");
        return;
    }
    int armor = Utilities::atoi(arg1);
    if(armor < 0)
    {
        ch->Send("armor: integer >= 0\n\r");
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
        ch->Send("durability: integer >= 0\n\r");
        return;
    }
    int durability = Utilities::atoi(arg1);
    if(durability < 0)
    {
        ch->Send("durability: integer >= 0\n\r");
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
        ch->Send("damage_low: integer >= 0, <= damage_high\n\r");
        return;
    }
    int damage_low = Utilities::atoi(arg1);
    if(damage_low < 0 || damage_low > pItem->damageHigh)
    {
        ch->Send("damage_low: integer >= 0, <= damage_high\n\r");
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
        ch->Send("damage_high: integer >= 0, >= damage_low\n\r");
        return;
    }
    int damage_high = Utilities::atoi(arg1);
    if(damage_high < 0 || damage_high < pItem->damageLow)
    {
        ch->Send("damage_high: integer >= 0, >= damage_low\n\r");
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
        ch->Send("speed: float >= 0.9\n\r");
        return;
    }
    double speed = Utilities::atof(arg1);
    if(speed < 0.9)
    {
        ch->Send("speed: float > 0.9\n\r");
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
        ch->Send("value: integer >= 0\n\r");
        return;
    }
    int value = Utilities::atoi(arg1);
    if(value < 0)
    {
        ch->Send("value: integer >= 0\n\r");
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
		ch->Send("stats agility/intellect/strength/stamina/wisdom/spirit <val>\n\r");
		return;
	}
	int val = Utilities::atoi(arg2);

	if (!Utilities::str_cmp("agility", arg1))
	{
		pItem->agility = val;
		ch->Send("agility set\n\r");
		return;
	}
	else if (!Utilities::str_cmp("intellect", arg1))
	{
		pItem->intellect = val;
		ch->Send("intellect set\n\r");
		return;
	}
	else if (!Utilities::str_cmp("strength", arg1))
	{
		pItem->strength = val;
		ch->Send("strength set\n\r");
		return;
	}
	else if (!Utilities::str_cmp("stamina", arg1))
	{
		pItem->stamina = val;
		ch->Send("stamina set\n\r");
		return;
	}
	else if (!Utilities::str_cmp("wisdom", arg1))
	{
		pItem->wisdom = val;
		ch->Send("wisdom set\n\r");
		return;
	}
	else if (!Utilities::str_cmp("spirit", arg1))
	{
		pItem->spirit = val;
		ch->Send("spirit set\n\r");
		return;
	}
	ch->Send("stats agility/intellect/strength/stamina/wisdom/spirit <val>\n\r");
}

void questEditCmd_show(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

	if(!pQuest)
	{
        LogFile::Log("error", "questEditCmd_show : user->editData == nullptr");
		return;
	}

	ch->Send("name:      [" + pQuest->name + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pQuest->id) + "]\n\r");
    ch->Send("level:     [" + Utilities::itos(pQuest->level) + "]\n\r");
    ch->Send("level_requirement: [" + Utilities::itos(pQuest->levelRequirement) + "]\n\r");
    ch->Send("quest_requirement: [" + Utilities::itos(pQuest->questRequirement) + "]\n\r");
    ch->Send("quest_restriction: [" + Utilities::itos(pQuest->questRestriction) + "]\n\r");
    ch->Send("start id:     [" + Utilities::itos(pQuest->start) + "]\n\r");
    ch->Send("end id:       [" + Utilities::itos(pQuest->end) + "]\n\r");
    ch->Send("exp_reward:   [" + Utilities::itos(pQuest->experienceReward) + "]\n\r");
    ch->Send("money_reward: [" + Utilities::itos(pQuest->moneyReward) + "]\n\r");
	std::string itemrewards = "item_reward:  [";
	for (auto itemiter = std::begin(pQuest->itemRewards); itemiter != std::end(pQuest->itemRewards); ++itemiter)
	{
		itemrewards += Utilities::itos(*itemiter) + " ";
	}
	itemrewards += "]\n\r";
	ch->Send(itemrewards);
    ch->Send("shareable:    [" + Utilities::itos(pQuest->shareable) + "]\n\r");

    ch->Send("Objectives:\n\r");
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
            case Quest::OBJECTIVE_ITEM: objid = ((Item*)((*objiter).objective))->id; break;
        }
        ch->Send(Utilities::itos(ctr) + ". Type: " + Utilities::itos((*objiter).type) + " Count: " + Utilities::itos((*objiter).count) 
                   + " Objective ID: " + Utilities::itos(objid) + " " + (*objiter).description + "\n\r");
        ctr++;
    }

    ch->Send("short_desc:     " + pQuest->shortDescription + "\n\r");
    ch->Send("long_desc:      " + pQuest->longDescription + "\n\r");
    ch->Send("progress_msg:   " + pQuest->progressMessage + "\n\r");
    ch->Send("completion_msg: " + pQuest->completionMessage + "\n\r");
}

void questEditCmd_objective(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest*)ch->editData;
    //objective add room|npc|item id count
    //          remove #

    std::string arg1,arg2,arg3,arg4;

    if(argument.empty())
    {
        ch->Send("objective add room||npc||item id count <description>\n\r");
        ch->Send("          remove #\n\r");
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
            ch->Send("empty description\n\r");
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
            ch->Send("objective types: room, npc, item\n\r");
            return;
        }

        //id
        if(!Utilities::IsNumber(arg3))
        {
            ch->Send("objective id must be a number\n\r");
            return;
        }
        int id = Utilities::atoi(arg3);
        if(id <= 0)
        {
            ch->Send("objective id must be > 0\n\r");
            return;
        }

        //count
        if(!Utilities::IsNumber(arg4))
        {
            ch->Send("objective count must be a number\n\r");
            return;
        }
        int count = Utilities::atoi(arg4);
        if(count <= 0)
        {
            ch->Send("objective count must be > 0\n\r");
            return;
        }
        pQuest->AddObjective(type, count, id, description);
        pQuest->changed = true;
        ch->Send("Objective added.\n\r");
        return;
    }
    else if(!Utilities::str_cmp("remove", arg1))
    {
        argument = Utilities::one_argument(argument, arg2);

        if(!Utilities::IsNumber(arg2))
        {
            ch->Send("Remove which objective?\n\r");
            return;
        }
        int index = Utilities::atoi(arg2);
        if(index <= 0)
        {
            ch->Send("Index to remove must be > 0.\n\r");
            return;
        }
        pQuest->RemoveObjective(index);
        pQuest->changed = true;
        ch->Send("Objective removed.\n\r");
        return;
    }
    ch->Send("objective add room||npc||item id count\n\r");
    ch->Send("          remove #\n\r");
}

void questEditCmd_name(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\n\r");
        return;
    }

    pQuest->name = argument;
    ch->Send("name set.\n\r");
    pQuest->changed = true;
}

void questEditCmd_shortdesc(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set short description to what?\n\r");
        return;
    }

    pQuest->shortDescription = argument;
    ch->Send("short_description set.\n\r");
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
        ch->Send("level <#>\n\r");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0 || level > Game::MAX_LEVEL)
    {
        ch->Send("level must be between 1 and " + Utilities::itos(Game::MAX_LEVEL) + "\n\r");
        return;
    }
    pQuest->level = level;
    pQuest->changed = true;
    ch->Send("level set.\n\r");
}

void questEditCmd_levelrequirement(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level_requirement <#>\n\r");
        return;
    }
    int level = Utilities::atoi(arg1);
    if(level <= 0 || level > Game::MAX_LEVEL)
    {
        ch->Send("level_requirement must be between 1 and " + Utilities::itos(Game::MAX_LEVEL) + "\n\r");
        return;
    }
    pQuest->levelRequirement = level;
    pQuest->changed = true;
    ch->Send("level_requirement set.\n\r");
}

void questEditCmd_questrequirement(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("quest_requirement <#>\n\r");
        return;
    }
    int requirement = Utilities::atoi(arg1);
    if(requirement < 0)
    {
        ch->Send("quest_requirement must be >= 0\n\r");
        return;
    }
	else if (requirement == 0)
	{
		ch->Send("quest_requirement cleared\n\r");
		pQuest->questRequirement = 0;
		pQuest->changed = true;
		return;
	}
    Quest * q = Game::GetGame()->GetQuest(requirement);
    if(q == nullptr)
    {
        ch->Send("A quest with that id does not exist.\n\r");
        return;
    }
    pQuest->questRequirement = q->id;
    pQuest->changed = true;
    ch->Send("quest_requirement set.\n\r");
}

void questEditCmd_questrestriction(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("quest_restriction <#>\n\r");
        return;
    }
    int restriction = Utilities::atoi(arg1);
    if(restriction < 0)
    {
        ch->Send("quest_restriction must be >= 0\n\r");
        return;
    }
	else if (restriction == 0)
	{
		ch->Send("quest_restriction cleared\n\r");
		pQuest->questRestriction = 0;
		pQuest->changed = true;
		return;
	}
    Quest * q = Game::GetGame()->GetQuest(restriction);
    if(q == nullptr)
    {
        ch->Send("A quest with that id does not exist.\n\r");
        return;
    }
    pQuest->questRestriction = q->id;
    pQuest->changed = true;
    ch->Send("quest_restriction set.\n\r");
}

void questEditCmd_start(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("start <#>\n\r");
        return;
    }
    int start = Utilities::atoi(arg1);
    if(start <= 0)
    {
        ch->Send("start npc id must be > 0\n\r");
        return;
    }
    NPCIndex * tch = Game::GetGame()->GetNPCIndex(start);
    if(tch == nullptr)
    {
        ch->Send("An npc with that id does not exist.\n\r");
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
    ch->Send("start npc set.\n\r");
}

void questEditCmd_end(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("end <#>\n\r");
        return;
    }
    int end = Utilities::atoi(arg1);
    if(end <= 0)
    {
        ch->Send("end npc id must be > 0\n\r");
        return;
    }
    NPCIndex * tch = Game::GetGame()->GetNPCIndex(end);
    if(tch == nullptr)
    {
        ch->Send("An npc with that id does not exist.\n\r");
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
    ch->Send("end npc set.\n\r");
}

void questEditCmd_expreward(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("exp_reward <#>\n\r");
        return;
    }
    int exp = Utilities::atoi(arg1);
    if(exp < 0)
    {
        ch->Send("exp_reward must be >= 0\n\r");
        return;
    }
    pQuest->experienceReward = exp;
    pQuest->changed = true;
    ch->Send("exp_reward set.\n\r");
}

void questEditCmd_moneyreward(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("money_reward <#>\n\r");
        return;
    }
    int money = Utilities::atoi(arg1);
    if(money < 0)
    {
        ch->Send("money_reward must be >= 0\n\r");
        return;
    }
    pQuest->moneyReward = money;
    pQuest->changed = true;
    ch->Send("money_reward set.\n\r");
}

void questEditCmd_itemreward(Player * ch, std::string argument)
{
	Quest * pQuest = (Quest *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("item_reward <#>\n\r");
		return;
	}
	int itemid = Utilities::atoi(arg1);
	if (itemid <= 0)
	{
		ch->Send("itemid must be > 0\n\r");
		return;
	}
	std::vector<int>::iterator iter = std::find(pQuest->itemRewards.begin(), pQuest->itemRewards.end(), itemid);
	if (iter != pQuest->itemRewards.end())
	{
		pQuest->itemRewards.erase(iter);
		pQuest->changed = true;
		ch->Send("Item reward removed\n\r");
		return;
	}
	Item * reward = Game::GetGame()->GetItem(itemid);
	if (!reward)
	{
		ch->Send("Item with that ID does not exist.\n\r");
		return;
	}
	pQuest->itemRewards.push_back(itemid);
	pQuest->changed = true;
	ch->Send("Item reward added\n\r");
}

void questEditCmd_shareable(Player * ch, std::string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("shareable 0||1\n\r");
        return;
    }
    int share = Utilities::atoi(arg1);
    if(share != 0 && share != 1)
    {
        ch->Send("shareable must equal 0 or 1\n\r");
        return;
    }
    pQuest->shareable = share;
    pQuest->changed = true;
    ch->Send("shareable set.\n\r");
}

void classEditCmd_show(Player * ch, std::string argument)
{
    Class * pClass = (Class *)ch->editData;
	if (pClass == nullptr)
		return;

    ch->Send("Name:      [" + pClass->name + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pClass->GetID()) + "]\n\r");
    ch->Send("Color std::string: [|" + pClass->color + "]\n\r");

    ch->Send("Skills (Level, Skill ID, long_name):\n\r");
    std::list<Class::SkillData>::iterator iter;
    for(iter = pClass->classSkills.begin(); iter != pClass->classSkills.end(); ++iter)
    {
        ch->Send("{" + Utilities::itos(iter->level) + ",    " + Utilities::itos(iter->skill->id) + ",     " + iter->skill->long_name + "}\n\r");
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
		ch->Send("skill <add/remove> <#id> <#level>\n\r");
		return;
	}
	int id = Utilities::atoi(arg2);
	if (!Utilities::str_cmp(arg1, "add"))
	{
		if (arg3.empty() || !Utilities::IsNumber(arg3))
		{
			ch->Send("skill <add/remove> <#id> <#level>\n\r");
			return;
		}
		int level = Utilities::atoi(arg3);
		if (!pClass->HasSkill(id))
		{
			pClass->changed = true;
			pClass->AddSkill(id, level);
			ch->Send("Added skill " + Utilities::itos(id) + "\n\r");
			return;
		}
		ch->Send("Class already has skill id " + Utilities::itos(id) + "\n\r");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "remove"))
	{
		if (!pClass->HasSkill(id))
		{
			ch->Send("Class does not have skill id : " + Utilities::itos(id) + "\n\r");
			return;
		}
		pClass->changed = true;
		pClass->RemoveSkill(id);
		ch->Send("Removed skill " + Utilities::itos(id) + "\n\r");
		return;
	}
	ch->Send("skill <add/remove> <#id>\n\r");
	return;
}

void helpEditCmd_show(Player * ch, std::string argument)
{
    Help * pHelp = (Help *)ch->editData;
	if(pHelp == nullptr)
		return;

	ch->Send("ID:			[" + Utilities::itos(pHelp->id) + "]\n\r");
    ch->Send("Title:		[" + pHelp->title + "]\n\r");
	ch->Send("Search String:[" + pHelp->search_string + "]\n\r");
    ch->Send("Text:\n\r");
    ch->Send(pHelp->text + "\n\r");
}

void helpEditCmd_title(Player * ch, std::string argument)
{
	Help * pHelp = (Help *)ch->editData;
	if(pHelp == nullptr)
		return;

	if(argument.empty())
	{
		ch->Send("Syntax: title <newtitle>\n\r");
		return;
	}
	if(!Utilities::IsAlpha(argument))
	{
		ch->Send("Invalid title.\n\r");
		return;
	}
	pHelp->title = argument;
	ch->Send("Title changed.\n\r");
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

    ch->Send( "Syntax:  text\n\r" );
}

void helpEditCmd_delete(Player * ch, std::string argument)
{
    Help * pHelp = (Help *)ch->editData;

	if(pHelp->remove)
	{
		ch->Send("ID " + Utilities::itos(pHelp->id) + " unmarked for deletion.\n\r");
		pHelp->remove = false;
	}
	else
	{
		ch->Send("ID " + Utilities::itos(pHelp->id) + " marked for deletion. Save to delete.\n\r");
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
        ch->Send("Error: Editing a null character. Target offline?\n\r");
		return;
	}

    std::string arg1;
    Utilities::one_argument(argument, arg1);
    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level <#arg>\n\r");
        return;
    }
    int newlevel = Utilities::atoi(arg1);
    if(vch->GetLevel() == newlevel)
    {
        ch->Send("Level is already " + arg1 + "\n\r");
        return;
    }
    if(newlevel > Game::MAX_LEVEL || newlevel < 1)
    {
        ch->Send("Level out of range: 1 to " + Utilities::itos(Game::MAX_LEVEL) + "\n\r");
        return;
    }
    vch->SetLevel(newlevel);
    //vch->player->SetExperience(Game::ExperienceForLevel(newlevel));
    ch->Send("Level set to " + arg1 + "\n\r");
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

	ch->Send("Name:             [" + pArea->name + "]\n\r");
    ch->Send("ID:               [" + Utilities::itos(pArea->GetID()) + "]\n\r");
	ch->Send("pvp:              [" + Utilities::itos(pArea->pvp) + "]\n\r");
    ch->Send("level_range_low:  [" + Utilities::itos(pArea->level_range_low) + "]\n\r");
    ch->Send("level_range_high: [" + Utilities::itos(pArea->level_range_high) + "]\n\r");
	ch->Send("death_room:       [" + Utilities::itos(pArea->death_room) + "]\n\r");
}

void areaEditCmd_name(Player * ch, std::string argument)
{
    Area * pArea = (Area *)ch->editData;

	if(argument.empty())
	{
		pArea->name.clear();
		ch->Send("Name cleared.\n\r");
		return;
	}

	if(argument.length() > 50)
	{
		ch->Send("Maximum length is 50 characters.\n\r");
		return;
	}

	pArea->name = argument;
    pArea->changed = true;
	ch->Send("Name set.\n\r");
}

void areaEditCmd_pvp(Player * ch, std::string argument)
{
    Area * pArea = (Area *)ch->editData;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("pvp: 0 - 3\n\r");
        return;
    }
    int pvp = Utilities::atoi(arg1);
    if(pvp < 0 || pvp > 3)
    {
        ch->Send("pvp: 0 - 3\n\r");
        return;
    }
    pArea->pvp = pvp;
    pArea->changed = true;
    ch->Send("pvp set.\n\r");
}

void areaEditCmd_death_room(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("death_room: #\n\r");
		return;
	}
	int death_room = Utilities::atoi(arg1);
	if (death_room < 0)
	{
		ch->Send("death_room: # >= 0\n\r");
		return;
	}
	pArea->death_room = death_room;
	pArea->changed = true;
	ch->Send("death_room set.\n\r");
}

void areaEditCmd_levelRangeLow(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("level_range_low: 0 < # <= max_level\n\r");
		return;
	}
	int level_range_low = Utilities::atoi(arg1);
	if (level_range_low <= 0 || level_range_low > Game::MAX_LEVEL)
	{
		ch->Send("level_range_low: 0 < # <= max_level\n\r");
		return;
	}
	pArea->level_range_low = level_range_low;
	pArea->changed = true;
	ch->Send("level_range_low set.\n\r");
}

void areaEditCmd_levelRangeHigh(Player * ch, std::string argument)
{
	Area * pArea = (Area *)ch->editData;

	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("level_range_high: 0 < # <= max_level\n\r");
		return;
	}
	int level_range_high = Utilities::atoi(arg1);
	if (level_range_high <= 0 || level_range_high > Game::MAX_LEVEL)
	{
		ch->Send("level_range_high: 0 < # <= max_level\n\r");
		return;
	}
	pArea->level_range_high = level_range_high;
	pArea->changed = true;
	ch->Send("level_range_high set.\n\r");
}
