#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CLogFile.h"
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
#include "utils.h"
#include "mud.h"

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

using namespace std;

void cmd_edit(Character * ch, string argument)
{
    /*if(!user->character)
		return;
    */
	string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
	//argument = Utilities::one_argument(argument, arg3);
	
	/*if(!Utilities::str_cmp(arg1, "find"))
	{
		if(arg2.empty())
		{
			ch->Send("edit find <room>\n\r");
			return;
		}
		else if(!Utilities::str_cmp(arg2, "room"))
		{
			int roomnum;
			if(Utilities::IsNumber(arg3))
			{
				roomnum = Utilities::atoi(arg3);

			}
		}

		return;
	}*/
	
	if(!Utilities::str_cmp(arg1, "room"))
	{
		//check for "edit room create <id>"
		//and "edit room <id>"
		if(ch && ch->room)
		{
            ch->editState = Character::ED_ROOM;
			ch->editData = ch->room;
            ch->Send("Ok.\n\r");
		}
		else
		{
            LogFile::Log("error", "cmd_edit, (user->character == NULL || user->character->room == NULL) with no room id argument");
			ch->editData = NULL;
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
			ch->editState = Character::ED_SKILL;
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Skill * sk = Game::GetGame()->GetSkill(Utilities::atoi(arg2));
            if(sk == NULL)
            {
                ch->Send("Skill not found.\n\r");
                return;
            }
            ch->editState = Character::ED_SKILL;
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
            ch->editState = Character::ED_NPC;
            ch->editData = Game::GetGame()->CreateNPCAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Character * tch = Game::GetGame()->GetCharacterIndex(Utilities::atoi(arg2));
            if(tch == NULL)
            {
                ch->Send("NPC " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Character::ED_NPC;
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
            ch->editState = Character::ED_ITEM;
            ch->editData = Game::GetGame()->CreateItemAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Item * item = Game::GetGame()->GetItemIndex(Utilities::atoi(arg2));
            if(item == NULL)
            {
                ch->Send("Item " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Character::ED_ITEM;
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
            ch->editState = Character::ED_QUEST;
            ch->editData = Game::GetGame()->CreateQuestAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Quest * q = Game::GetGame()->GetQuest(Utilities::atoi(arg2));
            if(q == NULL)
            {
                ch->Send("Quest " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Character::ED_QUEST;
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
            ch->editState = Character::ED_AREA;
            ch->editData = Game::GetGame()->CreateAreaAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
        else if(Utilities::IsNumber(arg2))
        {  
            Area * a = Game::GetGame()->GetArea(Utilities::atoi(arg2));
            if(a == NULL)
            {
                ch->Send("Area " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Character::ED_AREA;
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
            ch->editState = Character::ED_HELP;
            ch->editData = Game::GetGame()->CreateHelpAnyID(arg3);
            ch->Send("Ok.\n\r");
        }
		else if(Utilities::IsNumber(arg2))
        {  
			Help * h = Game::GetGame()->GetHelp(Utilities::atoi(arg2));
            if(h == NULL)
            {
                ch->Send("Help topic '" + arg2 + "' not found.\n\r");
                return;
            }
            ch->editState = Character::ED_HELP;
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
            if(c == NULL)
            {
                ch->Send("Class " + arg2 + " not found.\n\r");
                return;
            }
            ch->editState = Character::ED_CLASS;
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
        Character *vch;
        vch = Game::GetGame()->GetCharacterByPCName(arg2);
        //arg2 = ReplaceApostrophe(arg2);
        if(vch == NULL)// && server->sqlQueue->Read("select name from players where name='" + arg2 + "'").empty())
        {
            ch->Send("That player is not online.\n\r");
            return;
        }
        //have to get creative to ensure we're not trying to edit bad data if a player quits
        ch->editData = (void*)(new string(arg2.c_str()));
        ch->editState = Character::ED_PLAYER;
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
        if(ch->editState == Character::ED_PLAYER) //special cleanup for players
        {
            string * deleteme = (string *)ch->editData;
            delete deleteme;
        }
        ch->editState = Character::ED_NONE;
		ch->editData = NULL;
	}
    else if(!Utilities::str_cmp(arg1, "save"))
	{
		switch(ch->editState)
		{
			case Character::ED_NONE:
				ch->Send("You're not currently editing anything.\n\r");
				break;
            case Character::ED_SKILL:
                Game::GetGame()->SaveSkills();
                ch->Send("Skills saved.\n\r");
                break;
			case Character::ED_ROOM:
				Game::GetGame()->SaveRooms();
				ch->Send("Rooms saved.\n\r");
				break;
            case Character::ED_NPC:
                Game::GetGame()->SaveCharacterIndex();
                ch->Send("NPCs saved.\n\r");
                break;
            case Character::ED_ITEM:
                Game::GetGame()->SaveItemIndex();
                ch->Send("Items saved.\n\r");
                break;
            case Character::ED_QUEST:
                Game::GetGame()->SaveQuests();
                ch->Send("Quests saved.\n\r");
                break;
            case Character::ED_AREA:
                Game::GetGame()->SaveAreas();
                ch->Send("Areas saved.\n\r");
                break;
            case Character::ED_CLASS:
                Game::GetGame()->SaveClasses();
                ch->Send("Classes saved.\n\r");
                break;
			case Character::ED_HELP:
				if(((Help*)ch->editData)->remove)
				{
					ch->editState = Character::ED_NONE;
					ch->editData = NULL;
				}
                Game::GetGame()->SaveHelp();
                ch->Send("Help saved.\n\r");
                break;
            case Character::ED_PLAYER:
				break;
		}
	}
	else
	{
		//this should be a help file
		string syntax = "";
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


void roomEditCmd_show(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

	if(!pRoom)
	{
        LogFile::Log("error", "roomEditCmd_show : user->editData == NULL");
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
            ch->Send("-" + string(Exit::exitNames[i]) + " to [" + Utilities::itos(pRoom->exits[i]->to->id) + "]\n\r");
		}
	}
	ch->Send("Description:\n\r" + pRoom->description + "\n\r");

	string characters = "Characters: [";
    int limit = 0;
    for(std::list<Character *>::iterator i = pRoom->characters.begin(); i != pRoom->characters.end(); ++i)
	{
        if(++limit > 10)
        {
            characters += "...";
            break;
        }
		characters += (*i)->name + " ";
	}
	characters += "]\n\r";
	ch->Send(characters);

    //Resets
    if(!pRoom->resets.empty())
        ch->Send("Resets:\n\r");
    for(std::map<int, Reset*>::iterator iter = pRoom->resets.begin(); iter != pRoom->resets.end(); iter++)
    {
        Reset * r = (*iter).second;
        ch->Send(Utilities::itos(r->id) + ". " + (r->type == 1 ? "NPC " + Utilities::itos(r->npcID) : "") 
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

void roomEditCmd_name(Character * ch, string argument)
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

void roomEditCmd_area(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

    string arg1;
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

void roomEditCmd_description(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

    if(argument.empty() && ch->player && ch->player->user)
    {
        StringEdit::string_append( ch->player->user, &pRoom->description );
        pRoom->changed = true;
        return;
    }

    ch->Send( "Syntax:  desc\n\r" );
}

void roomEditCmd_flag(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

    string arg1;
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

void roomEditCmd_reset(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

    string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    argument = Utilities::one_argument(argument, arg3);
    if(Utilities::IsNumber(arg1))
    {
        int resetnum = Utilities::atoi(arg1);
        Reset * r = pRoom->GetReset(resetnum);
        if(r == NULL)
        {
            ch->Send("Reset " + Utilities::itos(resetnum) + " not found.\n\r");
            return;
        }

        if(!Utilities::str_cmp(arg2, "type"))
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
        }
        else if(!Utilities::str_cmp(arg2, "target_id"))
        {
            if(Utilities::IsNumber(arg3))
            {
                int newid = Utilities::atoi(arg3);
                if(r->type == 1) //npc
                {
                    Character * newindex = Game::GetGame()->GetCharacterIndex(newid);
                    if(newindex != NULL)
                    {
                        r->npcID = newid;
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
                Character * npc = Game::GetGame()->GetCharacterIndex(id);
                if(npc == NULL)
                {
                    ch->Send("NPC with that id does not exist.\n\r");
                    return;
                }
                Reset * new_reset = new Reset();
                new_reset->type = 1;
                new_reset->npcID = id;
                pRoom->AddReset(new_reset);
                ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\n\r");
            }
        }
		else if (!Utilities::str_cmp(arg2, "obj")) //TODO
		{
			/*if (Utilities::IsNumber(arg3))
			{
				int id = Utilities::atoi(arg3);
				Character * npc = Game::GetGame()->GetCharacterIndex(id);
				if (npc == NULL)
				{
					ch->Send("NPC with that id does not exist.\n\r");
					return;
				}
				Reset * new_reset = new Reset();
				new_reset->type = 1;
				new_reset->npcID = id;
				pRoom->AddReset(new_reset);
				ch->Send("Added reset #" + Utilities::itos(new_reset->id) + "\n\r");
			}*/
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
        if(deleteme != NULL)
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
        ch->Send("        reset <#> type||target_id||interval||wander_distance||leash_distance\n\r");
    }
}

void roomEditCmd_trigger(Character * ch, string argument)
{
    Room * pRoom = (Room *)ch->editData;

    string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
    //argument = Utilities::one_argument(argument, arg3);

    //<#> type|argument|script|function
    if(Utilities::IsNumber(arg1))
    {
        int trignum = Utilities::atoi(arg1);
        Trigger * t = pRoom->GetTrigger(trignum);
        if(t == NULL)
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
            if(ch->player && ch->player->user)
            {
                StringEdit::string_append( ch->player->user, &t->GetScript() );
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
        if(deleteme != NULL)
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

void roomEditCmd_create(Character * ch, string argument)
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
    if((pRoom = Game::GetGame()->GetRoom(value)) != NULL)
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

void roomEditDirection(Character * ch, string argument, int direction)
{
    if(direction < 0 || direction >= Exit::DIR_LAST)
    {
        LogFile::Log("error", "roomEditDirection, bad direction");
        return;
    }

    Room * pRoom = (Room *)ch->editData;
    string command;
    string arg;
    //int value;
  
    // Set the exit flags, needs full argument.
    /*if ( ( value = flag_value( exit_flags, argument ) ) != NO_FLAG )
    {
        ROOM_INDEX_DATA *pToRoom;
        int rev;                                 

        if ( !pRoom->exits[direction] )
        {
            send_to_char("Exit doesn't exist.\n\r",ch);
            return FALSE;
        }
        if((value == EX_LOCKED || value == EX_CLOSED)
            && !IS_SET(pRoom->exit[door]->rs_flags, EX_ISDOOR))
        {
            send_to_char("You must set the 'door' flag first.\n\r", ch);
            return FALSE;
        }
        if((value == EX_ISDOOR && IS_SET(pRoom->exit[door]->rs_flags, EX_ISDOOR))
            && (IS_SET(pRoom->exit[door]->rs_flags, EX_CLOSED)
            || IS_SET(pRoom->exit[door]->rs_flags, EX_LOCKED)))
        {
            send_to_char("You must remove the closed/locked flag first.\n\r", ch);
            return FALSE;
        }
        //  pRoom->exit[door] = new_exit(); 


        // This room.

        TOGGLE_BIT(pRoom->exit[door]->rs_flags,  value);
        // Don't toggle exit_info because it can be changed by players.
        pRoom->exit[door]->exit_info = pRoom->exit[door]->rs_flags;


        //  Connected room.

        pToRoom = pRoom->exit[door]->u1.to_room;    
        rev = rev_dir[door];

        if (pToRoom->exit[rev] != NULL)
        {
            TOGGLE_BIT(pToRoom->exit[rev]->rs_flags,  value);
            TOGGLE_BIT(pToRoom->exit[rev]->exit_info, value);
        }

        send_to_char( "Exit flag toggled.\n\r", ch );
        return TRUE;
    }*/


    //Now parse the arguments.
    argument = Utilities::one_argument(argument, command);
    Utilities::one_argument( argument, arg );

    if(command.empty() && argument.empty())	// Move command.
    {
        ch->Move(direction);
	    return;
    }

    /*if(command[0] == '?')
    {
	    do_help( ch, "EXIT" );
	    return FALSE;
    }*/

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
            pToRoom->exits[rev]->to = NULL;
	    }

	    //Remove this exit.
	    pRoom->exits[direction]->removeme = true;
        pRoom->exits[direction]->to = NULL;

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
        
    /*if ( !str_cmp( command, "dig" ) )
    {
	    char buf[MAX_STRING_LENGTH];
    	
	    if ( arg[0] == '\0' || !is_number( arg ) )
	    {
	        send_to_char( "Syntax: [direction] dig <vnum>\n\r", ch );
	        return FALSE;
	    }
    	
	    redit_create( ch, arg );
	    sprintf( buf, "link %s", arg );
	    change_exit( ch, buf, door);
	    return TRUE;
    }*/

    /*if ( !str_cmp( command, "room" ) )
    {
	    if ( arg[0] == '\0' || !is_number( arg ) )
	    {
	        send_to_char( "Syntax:  [direction] room [vnum]\n\r", ch );
	        return FALSE;
	    }

	    if ( !pRoom->exit[door] )
	    {
	        pRoom->exit[door] = new_exit();
	    }

	    value = atoi( arg );

	    if ( !get_room_index( value ) )
	    {
	        send_to_char( "REdit:  Cannot link to non-existant room.\n\r", ch );
	        return FALSE;
	    }

	    pRoom->exit[door]->u1.to_room = get_room_index( value );    
	    pRoom->exit[door]->orig_door = door;
        //pRoom->exit[door]->vnum = value;                 Can't set vnum in ROM 

	    send_to_char( "One-way link established.\n\r", ch );
	    return TRUE;
    }*/

    /*if ( !str_cmp( command, "key" ) )
    {
	    if ( arg[0] == '\0' || !is_number( arg ) )
	    {
	        send_to_char( "Syntax:  [direction] key [vnum]\n\r", ch );
	        return FALSE;
	    }

	    if ( !pRoom->exit[door] )
        {
            send_to_char("Exit does not exist.\n\r",ch);
            return FALSE;
        }

	    value = atoi( arg );

	    if ( !get_obj_index( value ) )
	    {
	        send_to_char( "REdit:  Item doesn't exist.\n\r", ch );
	        return FALSE;
	    }

	    if ( get_obj_index( atoi( argument ) )->item_type != ITEM_KEY )
	    {
	        send_to_char( "REdit:  Key doesn't exist.\n\r", ch );
	        return FALSE;
	    }

	    pRoom->exit[door]->key = value;

	    send_to_char( "Exit key set.\n\r", ch );
	    return TRUE;
    }*/

    /*if ( !str_cmp( command, "name" ) )
    {
	    if ( arg[0] == '\0' )
	    {
	        send_to_char( "Syntax:  [direction] name [string]\n\r", ch );
	        send_to_char( "         [direction] name none\n\r", ch );
	        return FALSE;
	    }

	    if ( !pRoom->exit[door] )
	       {
	   	    send_to_char("Exit does not exist.\n\r",ch);
	   	    return FALSE;
	       }

	    free_string( pRoom->exit[door]->keyword );
	    if (str_cmp(arg,"none"))
		    pRoom->exit[door]->keyword = str_dup( arg );
	    else
		    pRoom->exit[door]->keyword = str_dup( "" );

	    send_to_char( "Exit name set.\n\r", ch );
	    return TRUE;
    }*/

    /*if ( !str_prefix( command, "description" ) )
    {
	    if ( arg[0] == '\0' )
	    {
	       if ( !pRoom->exit[door] )
	       {
	   	    send_to_char("Exit does not exist.\n\r",ch);
	   	    return FALSE;
	       }

	        string_append( ch, &pRoom->exit[door]->description );
	        return TRUE;
	    }

	    send_to_char( "Syntax:  [direction] desc\n\r", ch );
	    return FALSE;
    }*/
}

void roomEditCmd_north(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTH);
}

void roomEditCmd_east(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_EAST);
}

void roomEditCmd_south(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTH);
}

void roomEditCmd_west(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_WEST);
}

void roomEditCmd_northeast(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTHEAST);
}

void roomEditCmd_southeast(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTHEAST);
}

void roomEditCmd_southwest(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_SOUTHWEST);
}

void roomEditCmd_northwest(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_NORTHWEST);
}

void roomEditCmd_up(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_UP);
}

void roomEditCmd_down(Character * ch, string argument)
{
    roomEditDirection(ch, argument, Exit::DIR_DOWN);
}

void skillEditCmd_show(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

	if(!pSkill)
	{
        LogFile::Log("error", "skillEditCmd_show : user->editData == NULL");
		return;
	}

	ch->Send("long_name: [" + pSkill->long_name + "]\n\r");
    ch->Send("name:      [" + pSkill->name + "]\n\r");
    ch->Send("id:        [" + Utilities::itos(pSkill->id) + "]\n\r");
    ch->Send("cast_time: [" + Utilities::dtos(pSkill->castTime, 2) + " seconds]\n\r");
	ch->Send("interrupt_flags: [");
	for (std::size_t i = 0; i < pSkill->interruptFlags.size(); ++i) 
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

void skillEditCmd_name(Character * ch, string argument)
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

void skillEditCmd_long_name(Character * ch, string argument)
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

void skillEditCmd_function_name(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(argument.empty())
	{
		return;
	}

    string arg;
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

void skillEditCmd_target_type(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("target_type 0(TARGET_SELF), 1(TARGET_OTHER), 2(TARGET_HOSTILE), 3(TARGET_ANY), 4(TARGET_FRIENDLY), 5(TARGET_NONE), 6(TARGET_PASSIVE)\n\r");
        return;
    }
    pSkill->changed = true;
    pSkill->targetType = (Skill::TargetType)Utilities::atoi(arg1);
}

void skillEditCmd_reload(Character * ch, string argument)
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
        LogFile::Log("error", string("Cost script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->castScript.c_str()))
    {
        LogFile::Log("error", string("Cast script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->applyScript.c_str()))
    {
        LogFile::Log("error", string("Apply script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->tickScript.c_str()))
    {
        LogFile::Log("error", string("Tick script: ") + lua_tostring(Server::luaState, -1));
    }
    if(luaL_dostring(Server::luaState, pSkill->removeScript.c_str()))
    {
        LogFile::Log("error", string("Remove script: ") + lua_tostring(Server::luaState, -1));
    }
	*/
}

void skillEditCmd_description(Character * ch, string argument)
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
void skillEditCmd_cost_description(Character * ch, string argument)
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

void skillEditCmd_cast_script(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pSkill->castScript );
        pSkill->changed = true;
    }
}

void skillEditCmd_apply_script(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pSkill->applyScript );
        pSkill->changed = true;
    }
}

void skillEditCmd_tick_script(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pSkill->tickScript );
        pSkill->changed = true;
    }
}

void skillEditCmd_remove_script(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pSkill->removeScript );
        pSkill->changed = true;
    }
}

void skillEditCmd_cost_function(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pSkill->costFunction );
        pSkill->changed = true;
    }
}

void skillEditCmd_cast_time(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    string arg;
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

void skillEditCmd_interrupt_flags(Character * ch, string argument)
{
	Skill * pSkill = (Skill *)ch->editData;

	string arg;
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

void skillEditCmd_cooldown(Character * ch, string argument)
{
    Skill * pSkill = (Skill *)ch->editData;

    string arg;
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

void npcEditCmd_show(Character * ch, string argument)
{
	Character * pChar = (Character *)ch->editData;

	if (!pChar)
	{
		LogFile::Log("error", "npcEditCmd_show : user->editData == NULL");
		return;
	}

	ch->Send("Name:      [" + pChar->name + "]\n\r");
	ch->Send("Keywords:	 [" + pChar->keywords + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pChar->id) + "]\n\r");
    ch->Send("Level:     [" + Utilities::itos(pChar->level) + "]\n\r");
    ch->Send("Title:     [" + pChar->title + "]\n\r");
    ch->Send("Gender:    [" + Utilities::itos(pChar->gender) + "]\n\r");
    ch->Send("Agility:   [" + Utilities::itos(pChar->agility) + "]\n\r");
    ch->Send("Intellect: [" + Utilities::itos(pChar->intellect) + "]\n\r");
    ch->Send("Strength:  [" + Utilities::itos(pChar->strength) + "]\n\r");
    ch->Send("Stamina:  [" + Utilities::itos(pChar->stamina) + "]\n\r");
    ch->Send("Wisdom:    [" + Utilities::itos(pChar->wisdom) + "]\n\r");
    ch->Send("Health:    [" + Utilities::itos(pChar->health) + "]\n\r");
    ch->Send("Mana:      [" + Utilities::itos(pChar->mana) + "]\n\r");
    ch->Send("Energy:    [" + Utilities::itos(pChar->energy) + "]\n\r");
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
        ch->Send(Character::flag_table[(*flagiter)].flag_name + " ");
    }
    ch->Send("\n\r");

    ch->Send("Drops:\n\r");
    std::list<Character::DropData>::iterator dropiter;
    int ctr = 1;
    for(dropiter = pChar->drops.begin(); dropiter != pChar->drops.end(); ++dropiter)
    {
        ch->Send(Utilities::itos(ctr) + ". " + Utilities::itos((*dropiter).percent) + "%: ");
        for(int i = 0; i < (int)(*dropiter).id.size(); i++)
        {
            ch->Send(Utilities::itos((*dropiter).id[i]) + " ");
        }
        ch->Send("\n\r");
    }
    ch->Send("\n\r");

    ch->Send("Skills:\n\r");
    std::map<string, Skill *>::iterator iter;
    for(iter = pChar->knownSkills.begin(); iter != pChar->knownSkills.end(); ++iter)
    {
        ch->Send("Spell: " + (*iter).second->long_name + " Name: " + (*iter).second->name 
            + " Cast time: " + Utilities::dtos((*iter).second->castTime, 2) + " Cooldown: " + Utilities::dtos((*iter).second->cooldown, 2) + "\n\r");
    }

    //Triggers
    if(!pChar->triggers.empty())
        ch->Send("Triggers:\n\r");
    for(std::map<int, Trigger>::iterator iter = pChar->triggers.begin(); iter != pChar->triggers.end(); iter++)
    {
        Trigger * t = &((*iter).second);
        ch->Send(Utilities::itos(t->id) + ". Type: " + Utilities::itos(t->GetType()) + " Argument: " + t->GetArgument() + 
                  " Function name: " + t->GetFunction() + "\n\r");
    }
}

void npcEditCmd_name(Character * ch, string argument)
{
    Character * pChar = (Character*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set name to what?\n\r");
        return;
    }

    pChar->name = argument;
    pChar->changed = true;
    ch->Send("name set.\n\r");
}

void npcEditCmd_title(Character * ch, string argument)
{
    Character * pChar = (Character*)ch->editData;

    if(argument.empty())
    {
        ch->Send("Set title to what?\n\r");
        return;
    }

    pChar->title = argument;
    pChar->changed = true;
    ch->Send("title set.\n\r");
}

void npcEditCmd_flag(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("flags: friendly neutral aggressive trainer guild repair\n\r");
        return;
    }

    arg1 = Utilities::ToLower(arg1);

    for(int i = 0; Character::flag_table[i].flag != -1; i++)
    {
        if(!Utilities::str_cmp(Character::flag_table[i].flag_name, arg1))
        {
            if(Utilities::FlagIsSet(pChar->flags, Character::flag_table[i].flag))
            {
                Utilities::FlagUnSet(pChar->flags, Character::flag_table[i].flag);
                ch->Send(arg1 + " flag removed.\n\r");
            }
            else
            {
                Utilities::FlagSet(pChar->flags, Character::flag_table[i].flag);
                ch->Send(arg1 + " flag set.\n\r");
            }
            pChar->changed = true;
            return;
        }
    }
    ch->Send("flags: friendly neutral aggressive trainer guild repair\n\r");
}

void npcEditCmd_drop(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

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
    string arg1,arg2,arg3;
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
            Character::DropData dd;
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
            for(std::list<Character::DropData>::iterator iter = pChar->drops.begin(); iter != pChar->drops.end(); ++iter)
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
        if((item = Game::GetGame()->GetItemIndex(itemid)) == NULL)
        {
            ch->Send("That item does not exist.\n\r");
            return;
        }

        Character::DropData * dd = NULL;
        int ctr = 1;
        for(std::list<Character::DropData>::iterator iter = pChar->drops.begin(); iter != pChar->drops.end(); ++iter)
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

void npcEditCmd_level(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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

void npcEditCmd_gender(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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

void npcEditCmd_agility(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("agility <#>\n\r");
        return;
    }
    int agility = Utilities::atoi(arg1);
    if(agility < 0)
    {
        ch->Send("Agility must be >= 0.\n\r");
        return;
    }
    pChar->changed = true;
    pChar->agility = agility;
}

void npcEditCmd_intellect(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("intellect <#>\n\r");
        return;
    }
    int intellect = Utilities::atoi(arg1);
    if(intellect < 0)
    {
        ch->Send("Intellect must be >= 0.\n\r");
        return;
    }
    pChar->changed = true;
    pChar->intellect = intellect;
}

void npcEditCmd_strength(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("strength <#>\n\r");
        return;
    }
    int strength = Utilities::atoi(arg1);
    if(strength < 0)
    {
        ch->Send("Strength must be >= 0.\n\r");
        return;
    }
    pChar->changed = true;
    pChar->strength = strength;
}

void npcEditCmd_stamina(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("stamina <#>\n\r");
        return;
    }
    int stamina = Utilities::atoi(arg1);
    if(stamina < 0)
    {
        ch->Send("Stamina must be >= 0.\n\r");
        return;
    }
    pChar->changed = true;
    pChar->stamina = stamina;
}

void npcEditCmd_wisdom(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty() || !Utilities::IsNumber(arg1))
    {
        ch->Send("wisdom <#>\n\r");
        return;
    }
    int wisdom = Utilities::atoi(arg1);
    if(wisdom < 0)
    {
        ch->Send("Wisdom must be >= 0.\n\r");
        return;
    }
    pChar->changed = true;
    pChar->wisdom = wisdom;
}

void npcEditCmd_health(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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
    pChar->health = health;
}

void npcEditCmd_mana(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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
    pChar->mana = mana;
}

void npcEditCmd_attackSpeed(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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

void npcEditCmd_damageLow(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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

void npcEditCmd_damageHigh(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1;
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

void npcEditCmd_speechText(Character * ch, string argument)
{
	Character * pChar = (Character *)ch->editData;

	if (argument.empty())
	{
		ch->Send("Set speechtext to what?\n\r");
		return;
	}

	pChar->speechText = argument;
	pChar->changed = true;
	ch->Send("speechText set.\n\r");
}

void npcEditCmd_skill(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;
    string arg1;
    string skill_name;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, skill_name);

    if(!Utilities::str_cmp(arg1, "add") && !skill_name.empty())
    {
        std::map<int, Skill*>::iterator skilliter; //ALL the skills :(
        for(skilliter = Game::GetGame()->skills.begin(); skilliter != Game::GetGame()->skills.end(); ++skilliter)
        {
            if(!Utilities::str_cmp((*skilliter).second->long_name, skill_name))
            {
                Skill * dupe;
                if((dupe = pChar->GetSkillShortName((*skilliter).second->name)) != NULL)
                {
                    ch->Send("NPC already has a skill with the same name.\n\r");
                    return;
                }
                else
                {
                    pChar->AddSkill((*skilliter).second);
                    ch->Send("Skill added: " + (*skilliter).second->long_name + "\n\r");
                }
                return;
            }
        }
        ch->Send("Skill not found.\n\r");
        return;
    }
    else if(!Utilities::str_cmp(arg1, "remove") && !skill_name.empty())
    {
        if(pChar->HasSkillByName(skill_name))
        {
            pChar->RemoveSkill(skill_name);
        }
        return;
    }

    ch->Send("skill add||remove <name>\n\r");
}

void npcEditCmd_trigger(Character * ch, string argument)
{
    Character * pChar = (Character *)ch->editData;

    string arg1, arg2, arg3;

    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
    arg3 = argument;
    //argument = Utilities::one_argument(argument, arg3);

    //<#> type|argument|script|function
    if(Utilities::IsNumber(arg1))
    {
        int trignum = Utilities::atoi(arg1);
        Trigger * t = pChar->GetTrigger(trignum);
        if(t == NULL)
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
            if(ch->player && ch->player->user)
            {
                StringEdit::string_append( ch->player->user, &t->GetScript() );
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
        if(deleteme != NULL)
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

void itemEditCmd_show(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    if(!pItem)
	{
        LogFile::Log("error", "itemEditCmd_show : user->editData == NULL");
		return;
	}

	ch->Send("Name:           [" + pItem->name + "]\n\r");
	ch->Send("Keywords:       [" + pItem->keywords + "]\n\r");
    ch->Send("ID:             [" + Utilities::itos(pItem->id) + "]\n\r");
    ch->Send("char_level:     [" + Utilities::itos(pItem->charLevel) + "]\n\r");
    ch->Send("item_level:     [" + Utilities::itos(pItem->itemLevel) + "]\n\r");
    ch->Send("quality:        [" + Utilities::itos(pItem->quality) + "]\n\r");
    ch->Send("equip_location: [" + Utilities::itos(pItem->equipLocation) + "]\n\r");
    ch->Send("binds:          [" + Utilities::itos(pItem->binds) + "]\n\r");
    ch->Send("type:           [" + Utilities::itos(pItem->type) + "]\n\r");
    ch->Send("quest item:     [" + Utilities::itos(pItem->quest) + "]\n\r");
    ch->Send("unique:         [" + Utilities::itos(pItem->unique) + "]\n\r");
    ch->Send("armor:          [" + Utilities::itos(pItem->armor) + "]\n\r");
    ch->Send("durability:     [" + Utilities::itos(pItem->durability) + "]\n\r");
    ch->Send("damage_low:     [" + Utilities::itos(pItem->damageLow) + "]\n\r");
    ch->Send("damage_high:    [" + Utilities::itos(pItem->damageHigh) + "]\n\r");
    ch->Send("speed:          [" + Utilities::dtos(pItem->speed, 2) + "]\n\r");
    if(pItem->speed != 0)
        ch->Send("(damage per second): " + Utilities::dtos(((((pItem->damageHigh - pItem->damageLow) / 2.0) + pItem->damageLow) / pItem->speed), 2) + "\n\r");
    ch->Send("value:          [" + Utilities::itos(pItem->value) + "]\n\r");
}

void itemEditCmd_name(Character * ch, string argument)
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

void itemEditCmd_item_level(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_char_level(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_quality(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_equip_location(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_binds(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_type(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_quest(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_unique(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_armor(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_durability(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_damageLow(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_damageHigh(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_speed(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void itemEditCmd_value(Character * ch, string argument)
{
    Item * pItem = (Item *)ch->editData;

    string arg1;
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

void questEditCmd_show(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

	if(!pQuest)
	{
        LogFile::Log("error", "questEditCmd_show : user->editData == NULL");
		return;
	}

	ch->Send("name:      [" + pQuest->name + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pQuest->id) + "]\n\r");
    ch->Send("level:     [" + Utilities::itos(pQuest->level) + "]\n\r");
    ch->Send("level_requirement: [" + Utilities::itos(pQuest->levelRequirement) + "]\n\r");
    ch->Send("quest_requirement: [" + Utilities::itos(pQuest->questRequirement) + "]\n\r");
    ch->Send("quest_restriction: [" + Utilities::itos(pQuest->questRestriction) + "]\n\r");
    ch->Send("start id:     [" + (pQuest->start == NULL ? "none" : Utilities::itos(pQuest->start->id)) + "]\n\r");
    ch->Send("end id:       [" + (pQuest->end == NULL ? "none" : Utilities::itos(pQuest->end->id)) + "]\n\r");
    ch->Send("exp_reward:   [" + Utilities::itos(pQuest->experienceReward) + "]\n\r");
    ch->Send("money_reward: [" + Utilities::itos(pQuest->moneyReward) + "]\n\r");
    ch->Send("shareable:    [" + Utilities::itos(pQuest->shareable) + "]\n\r");

    /*
    enum ObjectiveType { OBJECTIVE_ROOM, OBJECTIVE_VISITNPC, OBJECTIVE_KILLNPC, OBJECTIVE_ITEM };
    struct QuestObjective
    {
        ObjectiveType type;
        int count;
        void * objective; //visit a room, visit npc, kill npc, retrieve item
    };
    */
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
            case Quest::OBJECTIVE_VISITNPC: objid = ((Character*)((*objiter).objective))->id; break;
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

void questEditCmd_objective(Character * ch, string argument)
{
    Quest * pQuest = (Quest*)ch->editData;
    //objective add room|npc|item id count
    //          remove #

    string arg1,arg2,arg3,arg4;

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
        
        string description = argument;
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

void questEditCmd_name(Character * ch, string argument)
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

void questEditCmd_shortdesc(Character * ch, string argument)
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

void questEditCmd_longdesc(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pQuest->longDescription );
        pQuest->changed = true;
    }
}

void questEditCmd_progressmsg(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pQuest->progressMessage );
        pQuest->changed = true;
    }
}

void questEditCmd_completionmsg(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    if(ch->player)
    {
        StringEdit::string_append( ch->player->user, &pQuest->completionMessage );
        pQuest->changed = true;
    }
}

void questEditCmd_level(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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

void questEditCmd_levelrequirement(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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

void questEditCmd_questrequirement(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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
    if(q == NULL)
    {
        ch->Send("A quest with that id does not exist.\n\r");
        return;
    }
    pQuest->questRequirement = q->id;
    pQuest->changed = true;
    ch->Send("quest_requirement set.\n\r");
}

void questEditCmd_questrestriction(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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
    if(q == NULL)
    {
        ch->Send("A quest with that id does not exist.\n\r");
        return;
    }
    pQuest->questRestriction = q->id;
    pQuest->changed = true;
    ch->Send("quest_restriction set.\n\r");
}

void questEditCmd_start(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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
    Character * tch = Game::GetGame()->GetCharacterIndex(start);
    if(tch == NULL)
    {
        ch->Send("An npc with that id does not exist.\n\r");
        return;
    }
    //remove this quest from our current pQuest->start's questStart vector
    if(pQuest->start)
    {
        std::vector<Quest*>::iterator iter;
        for(iter = pQuest->start->questStart.begin(); iter != pQuest->start->questStart.end(); ++iter)
        {
            if((*iter)->id == pQuest->id)
            {
                pQuest->start->questStart.erase(iter);
                break;
            }
        }
    }
    tch->questStart.push_back(pQuest);
    pQuest->start = tch;
    pQuest->changed = true;
    ch->Send("start npc set.\n\r");
}

void questEditCmd_end(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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
    Character * tch = Game::GetGame()->GetCharacterIndex(end);
    if(tch == NULL)
    {
        ch->Send("An npc with that id does not exist.\n\r");
        return;
    }
    if(pQuest->end)
    {
        std::vector<Quest*>::iterator iter;
        for(iter = pQuest->end->questEnd.begin(); iter != pQuest->end->questEnd.end(); ++iter)
        {
            if((*iter)->id == pQuest->id)
            {
                pQuest->end->questEnd.erase(iter);
                break;
            }
        }
    }
    tch->questEnd.push_back(pQuest);
    pQuest->end = tch;
    pQuest->changed = true;
    ch->Send("end npc set.\n\r");
}

void questEditCmd_expreward(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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

void questEditCmd_moneyreward(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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

void questEditCmd_shareable(Character * ch, string argument)
{
    Quest * pQuest = (Quest *)ch->editData;

    string arg1;
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

void classEditCmd_show(Character * ch, string argument)
{
    Class * pClass = (Class *)ch->editData;

    ch->Send("Name:      [" + pClass->name + "]\n\r");
    ch->Send("ID:        [" + Utilities::itos(pClass->id) + "]\n\r");
    ch->Send("Color string: [|" + pClass->color + "]\n\r");

    ch->Send("Skills (ID, Level, Cost): ");
    std::list<Class::SkillData>::iterator iter;
    for(iter = pClass->classSkills.begin(); iter != pClass->classSkills.end(); ++iter)
    {
        ch->Send("{" + Utilities::itos((*iter).skill->id) + "," + Utilities::itos((*iter).level) + "} ");
                    //Utilities::itos((*iter).learnCost) + "} ");
    }
}

void helpEditCmd_show(Character * ch, string argument)
{
    Help * pHelp = (Help *)ch->editData;
	if(pHelp == NULL)
		return;

	ch->Send("ID:			[" + Utilities::itos(pHelp->id) + "]\n\r");
    ch->Send("Title:		[" + pHelp->title + "]\n\r");
	ch->Send("Search String:[" + pHelp->search_string + "]\n\r");
    ch->Send("Text:\n\r");
    ch->Send(pHelp->text + "\n\r");
}

void helpEditCmd_title(Character * ch, string argument)
{
	Help * pHelp = (Help *)ch->editData;
	if(pHelp == NULL)
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

void helpEditCmd_text(Character * ch, string argument)
{
    Help * pHelp = (Help *)ch->editData;

    if(argument.empty() && ch->player && ch->player->user)
    {
        StringEdit::string_append( ch->player->user, &pHelp->text );
        pHelp->changed = true;
        return;
    }

    ch->Send( "Syntax:  text\n\r" );
}

void helpEditCmd_delete(Character * ch, string argument)
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


void playerEditCmd_show(Character * ch, string argument)
{

}

void playerEditCmd_name(Character * ch, string argument)
{

}

//Sets level but NOT experience
void playerEditCmd_level(Character * ch, string argument)
{
    string * name = (string*)ch->editData;
    Character * vch = Game::GetGame()->GetCharacterByPCName(*name);

	if(!vch || !vch->player)
	{
        ch->Send("Error: Editing a null character. Target offline?\n\r");
		return;
	}

    string arg1;
    Utilities::one_argument(argument, arg1);
    if(!Utilities::IsNumber(arg1))
    {
        ch->Send("level <#arg>\n\r");
        return;
    }
    int newlevel = Utilities::atoi(arg1);
    if(vch->level == newlevel)
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

void playerEditCmd_exp(Character * ch, string argument)
{

}

void areaEditCmd_show(Character * ch, string argument)
{
    Area * pArea = (Area *)ch->editData;

	if(!pArea)
	{
        LogFile::Log("error", "areaEditCmd_show : user->editData == NULL");
		return;
	}

	ch->Send("Name:             [" + pArea->name + "]\n\r");
    ch->Send("ID:               [" + Utilities::itos(pArea->GetID()) + "]\n\r");
	ch->Send("pvp:              [" + Utilities::itos(pArea->pvp) + "]\n\r");
    ch->Send("level_range_low:  [" + Utilities::itos(pArea->level_range_low) + "]\n\r");
    ch->Send("level_range_high: [" + Utilities::itos(pArea->level_range_high) + "]\n\r");
}

void areaEditCmd_name(Character * ch, string argument)
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

void areaEditCmd_pvp(Character * ch, string argument)
{
    Area * pArea = (Area *)ch->editData;

    string arg1;
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

void areaEditCmd_levelRangeLow(Character * ch, string argument)
{

}

void areaEditCmd_levelRangeHigh(Character * ch, string argument)
{

}
