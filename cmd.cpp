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

extern class Command cmd_table[];

void cmd_attack(Character * ch, string argument)
{
    if(!ch)
        return;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    Character * target;
    if(arg1.empty())
    {
        if(ch->GetTarget())
        {
            target = ch->GetTarget();
			if (target->room != ch->room)
			{
				ch->Send("They aren't here.\n\r");
				return;
			}
        }
        else
        {
            ch->Send("Attack who?\n\r");
            return;
        }
    }
    else
    {
        target = ch->GetCharacterRoom(arg1);
        if(target == NULL)
        {
            ch->Send("They aren't here.\n\r");
            return;
        }
    }
    if(target == ch)
    {
        ch->Send("You can't attack yourself!\n\r");
        return;
    }
    if((target->IsNPC() && Utilities::FlagIsSet(target->flags, Character::FLAG_FRIENDLY))
		|| (!target->IsNPC() && ch->room->pvp == 0))
    {
        ch->Send("You can't attack that target.\n\r");
        return;
    }

    ch->SetTarget(target);
    ch->Send("You begin attacking " + target->name + "!\n\r");
    target->Send(ch->name + " begins attacking you!\n\r");

    ch->EnterCombat(target);
    target->EnterCombat(ch);

    //Do first attack here so attacker has a minor advantage
    ch->AutoAttack(target);
}

/*void cmd_autoattack(Character * ch, string argument)
{
    //TODO 'set' autoattack to turn on/off automatic retaliation?
}*/

void cmd_cancel(Character * ch, string argument)
{
    if(ch && ch->CancelCast())
    {
        ch->Send("Action Cancelled!\n\r");
    }
    else if(ch && ch->meleeActive)
    {
        ch->Send("Melee attack cancelled.\n\r");
        ch->meleeActive = false;
    }
    else
    {
        ch->Send("No action is in progress.\n\r");
    }
}

void cmd_look(Character * ch, string argument)
{
	if (argument == "compasstest")
	{
		const std::string formattest = R"(
  |CNorthshire |X- |WOn the Steps of Northshire Abbey
|B(------------------------------------------------------------------------------------------) 
|W(NW)|GSisol, Snoogans, Chrissy,|X 99 more...   |W(N)           |GMinotorious, Avemor, |X99 more...|W(NE)
                              |GDeathsmasher, Kabouter, |X99 more...                              
|W(W) |GRajani, Abos, Prothis, |X99 more...   |Y<---X--->|G      |GTyrell, Brodie, Angoon, |X99 more...|W(E)
                           |GWolfeater, Lydda, Crysknife, |X99 more...       
|W(SW)|GCutlex, Stakker, Gnarls, |X99 more...|W    (S)          |GPuolituinen, Arcadin, |X99 more...|W(SE)
|B(------------------------------------------------------------------------------------------)
|Y[?] |GMarshal McBride is here.

|B<|X3635/3635|Bh|X 100/100|Bm|X 100/100|Be |X0/100|Br>

  |CNorthshire |X- |WOn the Steps of Northshire Abbey
|B(------------------------------------------------------------------------------------------) 
|W(NW)|GSisol, Snoogans                       |W(N)                                           |W(NE)
                              |GDeathsmasher, Kabouter, |X3 more...                              
|W(W) |GRajani, Abos, Prothis, |X1 more...   |Y<---X--->|G                                         |W(E)
                                       |GWolfeater       
|W(SW)                                      |W(S)         |GPuolituinen, Arcadin, |X10 more...  |W(SE)
|B(------------------------------------------------------------------------------------------)
|Y[?] |GMarshal McBride is here.

|B<|X3635/3635|Bh|X 100/100|Bm|X 100/100|Be |X0/100|Br>

  |CNorthshire |X- |WOn the Steps of Northshire Abbey
|B(------------------------------------------------------------------------------------------) 
|W |B--                                       |W(N)                                            |B--
                              
|W(W)                                    |Y<---X--->|G                                         |W(E)
       
|W |B--                                       |W(S)                                            |B--
|B(------------------------------------------------------------------------------------------)
|Y[?] |GMarshal McBride is here.

|B<|X3635/3635|Bh|X 100/100|Bm|X 100/100|Be |X0/100|Br>
		)";
		ch->Send(formattest + "\n\r");
		return;
	}
    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty()) //"look" with no argument
    {
        if(ch->room == NULL)
	    {
            LogFile::Log("error", "cmd_look: ch->room == NULL");
		    return;
	    }

	    Room * inroom = ch->room;
        bool exitstatus[Exit::DIR_LAST];
	    for(int i = 0; i < Exit::DIR_LAST; i++)
	    {
		    if(inroom->exits[i] && inroom->exits[i]->to)
			    exitstatus[i] = true;
		    else
			    exitstatus[i] = false;
	    }
    /*
     Northern Courtyard                                     --    N    --
    (-------------------------------------------------)     - <---X---> -
                                                            --    S    --
    */
        stringstream roomstream;
        roomstream << " |W" << left << setw(55) << inroom->name <<  
            setw(8) << (exitstatus[7] ? "|WNW" : "|B--") <<
            setw(7) << (exitstatus[0] ? "|WN" : "|B-") <<
            (exitstatus[1] ? "|WNE" : "|B--") << "|X\n\r";
        roomstream << left << setw(58) << "|B(-------------------------------------------------)" <<
            (exitstatus[6] ? "|WW" : "|B-") << "|Y <-" << (exitstatus[8] ? "|WU|Y" : "-") << "-X-" <<
            (exitstatus[9] ? "|WD|Y" : "-") << "-> " << (exitstatus[2] ? "|WE" : "|B-") << "|X\n\r";

        string areaname = "|X";
        Area * this_area = Game::GetGame()->GetArea(inroom->area);
        if(this_area != NULL)
        {
            //TODO add pvp room rules to Game somewhere...
            if(this_area->pvp == 0) //no pvp whatsoever
                areaname = "|C";
            else if(this_area->pvp == 1) //dueling and guild pvp
                areaname = "|G";
            else if(this_area->pvp == 2) //everyone attacks everyone
                areaname = "|Y";
            else if(this_area->pvp == 3) //everyone attacks everyone, lose all your equipment
                areaname = "|R";

            areaname += this_area->name;
        }

		roomstream << " " << left << setw(26) << areaname;
		std::string roomflags = "";
		if (Utilities::FlagIsSet(inroom->flags, Room::Flags::FLAG_RECALL))
			roomflags += "[Recall]";
		if (ch->player && ch->player->IMMORTAL())
			roomflags += "[Room ID: " + Utilities::itos(inroom->id) + "]";

		roomstream << "|Y" << right << setw(26) << roomflags;
		roomstream << right << setw(9) << (exitstatus[5] ? "|WSW" : "|B--") <<
					  setw(7) << (exitstatus[4] ? "|WS" : "|B-") <<
					  setw(8) << (exitstatus[3] ? "|WSE" : "|B--") << "|X\n\r\n\r";

        ch->Send(roomstream.str());

	    /*if(ch->in_room->area->terrainmap && ch->playerData)
	    {
		    string terrainbuf = "";
		    for(int i = ch->playerData->aty - 7; i <= ch->playerData->aty + 7; i++)
		    {
			    for(int j = ch->playerData->atx - 15; j <= ch->playerData->atx + 15; j++)
			    {
				    if(i == ch->playerData->aty && j == ch->playerData->atx)
				    {
					    terrainbuf += "|M@";
				    }
				    /*else if((int)ch->playerData->map[i][j].rgbtGreen == 1)
				    {
					    terrainbuf += "|YO";
				    }*/
				    /*else
				    {
					    terrainbuf += ch->in_room->area->terrainmap->terrainCodes[((int)ch->playerData->map[i][j].rgbtRed)];
				    }
			    }
			    terrainbuf += '\n';
			    ch->SendToChar(terrainbuf);
			    terrainbuf = "";
		    }
	    }*/
        if(!inroom->description.empty())
	        ch->Send("  " + inroom->description);

        std::list<Character *>::iterator i;
        for(i = inroom->characters.begin(); i != inroom->characters.end(); i++)
        {
            if(ch == (*i))
                continue;

            //Determine appropriate quest icon. Hierarchy |Y?, |Y!, |W?, |D!
            //Search quests this npc ends
            string questicon = "";
            bool foundyellow = false; bool foundwhite = false;
            if(ch->player && !(*i)->questEnd.empty())
            {
                std::vector<Quest *>::iterator questiter;
                for(questiter = (*i)->questEnd.begin(); questiter != (*i)->questEnd.end(); ++questiter)
                {
                    if(ch->player->QuestObjectivesComplete(*questiter))
                    {
                        questicon = "|Y[?] ";
                        foundyellow = true;
                        break;
                    }
                    else if(ch->player->QuestActive(*questiter))
                    {
                        questicon = "|W[?] ";
                        foundwhite = true;
                    }
                }
            }
            //Search quests this npc starts
            if(!foundyellow && ch->player && !(*i)->questStart.empty())
            {
                std::vector<Quest *>::iterator questiter;
                for(questiter = (*i)->questStart.begin(); questiter != (*i)->questStart.end(); ++questiter)
                {
                    if(ch->player->QuestEligible(*questiter))
                    {
                        //TODO: white ! for soon to be available quests
                        if(!foundwhite && Quest::GetDifficulty(ch->level, (*questiter)->level) == 0)
                        {
                            questicon = "|Y[|D!|Y] ";
                        }
                        else
                        {
                            questicon = "|Y[!] ";
                            break;
                        }
                    }
                }
            }
		    string disconnected = "";
            string title = "";
            string fighting = ".";
			string level = "";
			string aggressionColor = "|G";

		    if((*i)->player && (*i)->player->user && !(*i)->player->user->IsConnected())
			    disconnected = "|Y[DISCONNECTED] |X";

            if((*i)->player)
                title = (*i)->title;
            else if(!(*i)->title.empty())
                title = " <" + (*i)->title + ">";

			level += "<" + 
				Game::LevelDifficultyColor(Game::LevelDifficulty(ch->level, (*i)->level))
				+ Utilities::itos((*i)->level) + "|X> ";

            if((*i)->InCombat())
            {
                if((*i)->IsFighting(ch))
                    fighting = ", fighting YOU!";
                else if((*i)->GetTarget() != NULL)
                    fighting = ", fighting " + (*i)->GetTarget()->name + ".";
            }
			aggressionColor = ch->AggressionColor((*i));

		    ch->Send(disconnected + level + questicon + aggressionColor + (*i)->name + title + " is here" + fighting + "|X\n\r");
	    }
    }
    else // "look argument" //TODO, look at things in the room with higher priority
    {
        //TODO definitely want to make a function out of this
        Item * inspect = ch->player->GetItemInventory(arg1);
        if(inspect == NULL)
        {
            inspect = ch->player->GetItemEquipped(arg1);
        }
        if(!inspect)
        {
            ch->Send("You don't see that here.\n\r");
            return;
        }
        ch->Send(Item::quality_strings[inspect->quality] + inspect->name + "|X\n\r");
        if(inspect->binds != Item::BIND_NONE)
        {
            ch->Send((string)Item::bind_strings[inspect->binds] + "\n\r");
        }
        if(inspect->quest)
        {
            ch->Send("Quest Item\n\r");
        }
        if(inspect->unique)
        {
            ch->Send("Unique\n\r");
        }  

        if(inspect->equipLocation != Item::EQUIP_NONE)
        {
            ch->Send(Item::equip_strings[inspect->equipLocation]);
            if(inspect->type != Item::TYPE_MISC)
            {
                ch->Send("     " + (string)Item::type_strings[inspect->type] + "\n\r");
            }
            else
            {
                ch->Send("\n\r");
            }
        }
        else if(inspect->equipLocation == Item::EQUIP_NONE && inspect->type != Item::TYPE_MISC)
        {
            ch->Send((string)Item::type_strings[inspect->type] + "\n\r");
        }
        
        if(inspect->armor > 0)
        {
            ch->Send("Armor " + Utilities::itos(inspect->armor) + "\n\r");
        }
        if(inspect->damageHigh > 0 && inspect->speed > 0)
        {
            ch->Send(Utilities::itos(inspect->damageLow) + " - " + Utilities::itos(inspect->damageHigh) + " Damage");
            ch->Send("    Speed " + Utilities::dtos(inspect->speed, 2) + "\n\r");  
            double dps = ((inspect->damageLow + inspect->damageHigh) / 2.0) / inspect->speed;
            ch->Send("(" + Utilities::dtos(dps, 2) + " damage per second)\n\r");
        }

        if(inspect->durability)
        {
            ch->Send("Durability " + Utilities::itos(inspect->durability) + "\n\r");
        }
        if(inspect->charLevel > 0)
            ch->Send("Requires Level " + Utilities::itos(inspect->charLevel) + "\n\r");
        if(inspect->itemLevel > 0)
            ch->Send("Item Level " + Utilities::itos(inspect->itemLevel) + "\n\r");
        if(inspect->value > 0)
            ch->Send("Sell Price: " + Utilities::itos(inspect->value) + "\n\r");
    }
}

// adds the target's health/mana/energy/rage to the players prompt
// spells/skills done to a character automatically target that character
// allow targeting in same room and adjacent rooms
void cmd_target(Character * ch, string argument)
{
    if(argument.empty())
	{
		ch->Send("Target cleared.\n\r");
		ch->ClearTarget();
        ch->meleeActive = false; //Any target change stops auto attack
		return;
	}

	string arg1;
    argument = Utilities::one_argument(argument, arg1);

	Character *vch;
	if((vch = ch->GetCharacterAdjacentRoom(arg1)) != NULL ||
        (ch->player && ch->player->IMMORTAL() 
         && (vch = Game::GetGame()->GetPlayerWorld(ch, arg1)) != NULL))
	{
        if(ch->GetTarget() != vch)
            ch->meleeActive = false; //Any target change stops auto attack
		ch->SetTarget(vch);
		ch->Send("Targeting " + vch->name + "\n\r");
	}
    else
    {
        ch->Send("Target not found.\n\r");
    }
}

void cmd_commands(Character * ch, string argument)
{
	Command * whichTable = Command::GetCurrentCmdTable(ch);

    int newline = 1;
    for(int cmd = 0; whichTable[cmd].name.length() > 0; cmd++)
	{
		if (whichTable[cmd].level >= 0 || ch->player->IMMORTAL())
		{
			ch->Send(whichTable[cmd].name + "  ");
			newline++;
		}
		if(newline % 5 == 0)
			ch->Send("\n\r");
		
	}
	ch->Send("\n\r");
}

void cmd_score(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

    ch->Send("Player information for " + ch->name + ":\n\r");
    ch->Send("Level: " + Utilities::itos(ch->level) + "\n\r");
    ch->Send("Class: " + ch->player->currentClass->name + "\n\r");
    ch->Send("Class Level: " + Utilities::itos(ch->player->GetClassLevel(ch->player->currentClass->id)) + "\n\r");
    ch->Send("Class History: ");

    bool found = false;
    std::list<Player::ClassData>::iterator iter;
    for(iter = ch->player->classList.begin(); iter != ch->player->classList.end(); ++iter)
    {
        Class * c = Game::GetGame()->GetClass((*iter).id);
        ch->Send("[" + c->name + " " + Utilities::itos((*iter).level) + "] ");
        found = true;
    }
    if(found)
    {
        ch->Send("\n\r");
    }
    else
    {
        ch->Send("None\n\r");
    }
    if(ch->player->IMMORTAL())
        ch->Send("Immortal Level: " + Utilities::itos(ch->player->immlevel) + "\n\r");
    ch->Send("Health: " + Utilities::itos(ch->health) + "/" + Utilities::itos(ch->maxHealth));
    ch->Send("  Mana: " + Utilities::itos(ch->mana) + "/" + Utilities::itos(ch->maxMana));
    ch->Send("  Energy: " + Utilities::itos(ch->energy) + "/" + Utilities::itos(ch->maxEnergy) + "\n\r");
	ch->Send("  Rage: " + Utilities::itos(ch->rage) + "/" + Utilities::itos(ch->maxRage) + "\n\r");
    ch->Send("Agility: " + Utilities::itos(ch->agility) + " Intellect: " + Utilities::itos(ch->intellect)
        + " Strength: " + Utilities::itos(ch->strength) + " Stamina: " + Utilities::itos(ch->stamina) + " Wisdom: "
        + Utilities::itos(ch->wisdom) + "\n\r");
	ch->Send("Attribute Points available: " + Utilities::itos(ch->player->statPoints) + "\n\r");
    ch->Send("Experience: " + Utilities::itos(ch->player->experience) + "\n\r");
    ch->Send("You have " + Utilities::itos(ch->player->experience) + " experience and need " 
                    + Utilities::itos(Game::ExperienceForLevel(ch->level+1)) + " experience to reach level " 
                    + Utilities::itos(ch->level+1) + " (" + Utilities::itos((Game::ExperienceForLevel(ch->level+1) - ch->player->experience))
                    + " tnl)\n\r");
    double movespeed = ch->GetMoveSpeed();
    ch->Send("Current movement speed: " + Utilities::dtos(ch->movementSpeed * movespeed, 2) + " rooms per second ("
        + Utilities::dtos(movespeed*100, 0) + "% of normal)\n\r");
}

void cmd_scan(Character * ch, string argument)
{
	if (!ch || !ch->room)
		return;

	std::list<Character *>::iterator iter;
	Room * scan_room;
	stringstream out;
	int depth = 3;
	string depthcolors[3] = { "|r", "|m", "|y" };
	bool found = false;

	for (int i = 0; i < Exit::DIR_LAST; i++)
	{
		out << setw(11) << Exit::exitNames[i] + ": ";
		scan_room = ch->room;
		for(int j = 0; j < depth; j++)
		{
			if (scan_room->exits[i] && scan_room->exits[i]->to)
			{
				scan_room = scan_room->exits[i]->to;
			}
			else
			{
				break;
			}
			if(!scan_room->characters.empty())
			{
				found = true;
				out << depthcolors[j] << "[" << Utilities::itos(j + 1) << "]: ";
				std::list<Character *>::iterator iter = scan_room->characters.begin();
				while (iter != scan_room->characters.end())
				{
					out << ch->AggressionColor(*iter) << (*iter)->name;
					iter++;
					if (iter != scan_room->characters.end())
					{
						out << "|X, ";
					}
				}
				out << "|X ";
			}
		}
		if (found)
		{
			out << "|X\n\r";
			ch->Send(out.str());
			found = false;
		}
		out.str("");
		out.clear();
	}
}

void cmd_who(Character * ch, string argument)
{
    ch->Send("|YPlayers online: |X\n\r");
    ch->Send("|B`-------------------------------------------------------------------------------'|X\n\r");
    int found = 0;
    Player * wplayer;
    std::list<User *>::iterator iter;
	std::map<int, Class *>::iterator classiter;
	Class * myclass = nullptr;
	std::stringstream sstr;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
    {
        if((*iter)->IsConnected() && (*iter)->connectedState == User::CONN_PLAYING && (*iter)->character && (*iter)->character->player)
	    {
		    wplayer = (*iter)->character->player;
			sstr.str("");
			sstr.clear();
			//name
            sstr << " " << left << setw(12) << (*iter)->character->name;
			//total level
            sstr << " |B[|W" << right << setw(3) << (wplayer->IMMORTAL() ? wplayer->immlevel : (*iter)->character->level) << "|B]";
			//gender
			sstr << " |B" << right << ((*iter)->character->gender == 1 ? "M" : "F");
			//race
			sstr << " |B" << left << setw(8) << Character::race_table[(*iter)->character->race].name << " ";
			//classes
			if (wplayer->IMMORTAL())
			{
				sstr << "|B[|WImmortal|B]";
			}
			else
			{
				for (classiter = Game::GetGame()->classes.begin(); classiter != Game::GetGame()->classes.end(); classiter++)
				{
					myclass = (*classiter).second;
					sstr << "|B[" << myclass->color << myclass->name << " " << right << setw(3) << Utilities::itos(wplayer->GetClassLevel(myclass->id)) << "|B]";
				}
			}
            sstr << left << ((*iter)->character->combat ? "|R<X>" : "") << ((*iter)->character->IsGhost() ? "|D<G>" : "");
            sstr << "|X\n\r";

		    ch->Send(sstr.str());
		    found++;
	    }
    }
    ch->Send("|B`-------------------------------------------------------------------------------'|X\n\r");
    ch->Send(Utilities::itos(found) + " players found.\n\r");
}

void cmd_title(Character * ch, string argument)
{   
    if(!ch || !ch->player)
        return;

    if(argument.empty())
	{
		ch->Send("Title cleared.\n\r");
        ch->title.clear();
		return;
	}
	if(argument.length() > 45)
    {
        ch->Send("Title too long. Shortened to 45 characters.\n\r");
		argument = argument.substr(0, 45);
    }

	if(ch->player)
	{
		if(argument[0] != '.' && argument[0] != ',' && argument[0] != '!' && argument[0] != '?' )
		{
			argument = ' ' + argument;
		}
		ch->title = argument;
		ch->Send("Title set.\n\r");
	}
}

void cmd_group(Character * ch, string argument)
{
    ch->Send("cmd_group\n\r");
}

void cmd_class(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

    string arg1;
    string arg2;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    Class * currentClass = ch->player->currentClass;

    if(!Utilities::str_cmp(arg1, "info"))
    {
        ch->Send("Current class: [" + currentClass->name + " " 
                   + Utilities::itos(ch->player->GetClassLevel(currentClass->id)) + "]\n\r");
        ch->Send("Class history: ");
        bool found = false;
        std::list<Player::ClassData>::iterator iter;
        for(iter = ch->player->classList.begin(); iter != ch->player->classList.end(); ++iter)
        {
            Class * c = Game::GetGame()->GetClass((*iter).id);
            ch->Send("[" + c->name + " " + Utilities::itos((*iter).level) + "] ");
            found = true;
        }
        if(found)
        {
            ch->Send("\n\r");
        }
        else
        {
            ch->Send("None\n\r");
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "newclass"))
    {
        if(arg2.empty())
        {
			ch->Send("Valid classes are: ");
			std::map<int, Class *>::iterator iter;
			for (iter = Game::GetGame()->classes.begin(); iter != Game::GetGame()->classes.end(); ++iter)
			{
				ch->Send(" " + (*iter).second->name);
			}
			ch->Send("\n\r");
            return;
        }
        Class * newclass = Game::GetGame()->GetClassByName(arg2);
        if(newclass == NULL)
        {
			ch->Send("Valid classes are: ");
			std::map<int, Class *>::iterator iter;
			for (iter = Game::GetGame()->classes.begin(); iter != Game::GetGame()->classes.end(); ++iter)
			{
				ch->Send(" " + (*iter).second->name);
			}
			ch->Send("\n\r");
            return;
        }
        ch->player->currentClass = newclass;
        ch->Send("Class changed to " + newclass->name + ".\n\r");
        return;
    }
    else
    {
        ch->Send("Usage: class info\n\r");
        ch->Send("       class newclass <classname>\n\r");
        return;
    }
}

void cmd_levels(Character * ch, string argument)
{
    int prev = 0, next = 0;
    for(int i = 1; i <= Game::MAX_LEVEL; i++)
    {
        next = Game::ExperienceForLevel(i);
        std::stringstream sstr;
        sstr << "|B[|XLevel: " << setw(3) << i;
        sstr << "|B]|X Experience Required: " << setw(9) << next << "  Difference: " << next - prev << "\n\r";
        ch->Send(sstr.str());
        prev = next;
    }
}

void cmd_help(Character * ch, string argument)
{
	string arg1;
    argument = Utilities::one_argument(argument, arg1);

	if(arg1.empty())
	{
		ch->Send("HELP is available on the following topics.\n\rType 'help <topic>' for more information.\n\rType 'commands' for a full list of commands.\n\r");
		ch->Send("MOVEMENT    QUESTS    COMBAT    COMMUNICATION    SPELLS\n\r");
		return;
	}

	if(Utilities::IsNumber(arg1))
	{
		int helpnum = Utilities::atoi(arg1);
		std::map<int,Help*>::iterator found = Game::GetGame()->helpIndex.find(helpnum);
		if(found != Game::GetGame()->helpIndex.end())
		{
			ch->Send((*found).second->text);
		}
	}
	else
	{
		std::map<int, Help*>::iterator iter;
		std::vector<Help*> found;

		for(iter = Game::GetGame()->helpIndex.begin(); iter != Game::GetGame()->helpIndex.end(); ++iter)
		{
			if(!Utilities::str_prefix(arg1, (*iter).second->title))
			{
				found.push_back((*iter).second);
			}
		}
		if(found.size() == 1)
		{
			ch->Send(found[0]->text);
		}
		else if(found.size() > 1)
		{
			for(int i = 0; i < (int)found.size(); i++)
			{
				ch->Send("|M[|X" + Utilities::itos(found[i]->id) + "|M]|X " + Utilities::ToUpper(found[i]->title) + "\n\r");
			}
		}
		else
		{
			ch->Send("No matching help items found.\n\r");
		}
	}
}
	 /*
	 GENERAL        COMMUNICATION
	 OBJECTS        MOVEMENT           FORMATION
	 COMBAT         TRAVEL
	 SPELLS
	 INFORMATION
	 */

/*
Movement is allowed in the following directions:
north       south       east        west        up
northwest   northeast   southwest   southeast   down

    Directions can be abbreviated, for example 'n' is the same as 'north', 
'ne' is the same as 'northeast'. Movement consists of moving your character from location to 
location. Locations in the game are usually referred to as \"rooms\", 
even if they are outside.

Movement is allowed at a rate of three rooms per second, which can be altered by spell affects or the use of a mount.
*/

void cmd_quest(Character * ch, string argument)
{
    if(!ch || !ch->player)
    {
        return;
    }
    if(argument.empty())
    {
        ch->Send("Quest: log, list, info #, accept #, drop #, complete #, progress #, share #\n\r");
        return;
    }
    //log, list, info #, accept #, drop #, complete #, progress #, share #
    string arg1;
    string arg2;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    if(!Utilities::str_cmp(arg1, "log"))
    {
        ch->Send("Active quests:\n\r");
        if(ch->player->questLog.empty())
        {
            ch->Send("None.\n\r");
            return;
        }
        std::vector<Quest *>::iterator iter;
        int ctr = 1;
        for(iter = ch->player->questLog.begin(); iter != ch->player->questLog.end(); ++iter)
        {
            Quest * q = (*iter);
            string complete = "";
            if(ch->player->QuestObjectivesComplete(q))
            {
                complete = " |G(Complete)";
            }
            ch->Send(Utilities::itos(ctr++) + ". " + Quest::GetDifficultyColor(ch->level, q->level) 
                       + q->name + " (" + Utilities::itos(q->level) + ")" + complete + "|X\n\r");
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "list"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to list quests.\n\r");
            return;
        }
        if(ch->GetTarget()->questStart.empty() && ch->GetTarget()->questEnd.empty())
        {
            ch->Send("Your target has no quests to offer.\n\r");
            return;
        }
        bool found = false;
        std::vector<Quest *>::iterator questiter;
        for(questiter = ch->GetTarget()->questEnd.begin(); questiter != ch->GetTarget()->questEnd.end(); ++questiter)
        {
            if(ch->player->QuestActive(*questiter))
            {
				if (!found && !ch->GetTarget()->speechText.empty())
					ch->Send(ch->GetTarget()->speechText + "\n\r");
                //TODO color ? by complete status
                found = true;
                ch->Send("|Y? " + Quest::GetDifficultyColor(ch->level, (*questiter)->level) + (*questiter)->name + " (" + Utilities::itos((*questiter)->level) + ")|X");
				if (!(*questiter)->progressMessage.empty())
				{
					ch->Send(" : " + (*questiter)->progressMessage + "\n\r");
				}
				else
				{
					ch->Send("\n\r");
				}
            }
        }
        int ctr = 1;
        for(questiter = ch->GetTarget()->questStart.begin(); questiter != ch->GetTarget()->questStart.end(); ++questiter)
        {
            if(ch->player->QuestEligible(*questiter))
            {
				if (!found && !ch->GetTarget()->speechText.empty())
					ch->Send(ch->GetTarget()->speechText + "\n\r");
                found = true;
                ch->Send(Utilities::itos(ctr) + ". |Y! " + Quest::GetDifficultyColor(ch->level, (*questiter)->level) 
                           + (*questiter)->name + " (" + Utilities::itos((*questiter)->level) + ")|X\n\r");
                ctr++;
            }
        }
        if(!found)
        {
            ch->Send("Your target has no quests to offer.\n\r");
            return;
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "info"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to get quest information.\n\r");
            return;
        }
        if(ch->GetTarget()->questStart.empty())
        {
            ch->Send("Your target has no quests to offer.\n\r");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Get information for which quest?\n\r");
            return;
        }

        std::vector<Quest *>::iterator questiter;
        int ctr = 1;
        for(questiter = ch->GetTarget()->questStart.begin(); questiter != ch->GetTarget()->questStart.end(); ++questiter)
        {
            if(ch->player->QuestEligible(*questiter))
            {
                if(qnum == ctr)
                {
                    Quest * q = (*questiter);
                    ch->Send(Quest::GetDifficultyColor(ch->level, q->level) + q->name + " (" 
                               + Utilities::itos(q->level) + ")|X\n\r\n\r");
                    ch->Send(q->longDescription + "\n\r\n\r");
                    ch->Send(q->shortDescription + "\n\r\n\r");
                    ch->Send("You will receive |Y" + Utilities::itos(q->experienceReward) + " experience|X and |Y" 
                               + Utilities::itos(q->moneyReward) + " gold|X.\n\r");
                    return;
                }
                ctr++;
            }
        }
        ch->Send("Quest " + arg2 + " not found.\n\r");
        return;
    }
    else if(!Utilities::str_cmp(arg1, "accept"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to accept a quest.\n\r");
            return;
        }
        if(ch->GetTarget()->questStart.empty())
        {
            ch->Send("Your target has no quests to offer.\n\r");
            return;
        }
        if(ch->player->questLog.size() >= Player::QUESTLOG_MAX_SIZE)
        {
            ch->Send("Your quest log is full.\n\r");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Accept which quest?\n\r");
            return;
        }
        std::vector<Quest *>::iterator questiter;
        int ctr = 1;
        //Need to loop on QuestEligible quests only
        for(questiter = ch->GetTarget()->questStart.begin(); questiter != ch->GetTarget()->questStart.end(); ++questiter)
        {
            if(ch->player->QuestEligible(*questiter))
            {
                if(qnum == ctr)
                {
                    //accept this quest
                    Quest * q = (*questiter);
                    std::vector<int> playerObjectives;
                    playerObjectives.resize(q->objectives.size(), 0);
                    ch->player->questLog.push_back(q);
                    ch->player->questObjectives.push_back(playerObjectives);
                    
                    ch->Send("Accepted quest: " + q->name + "\n\r");

                    //TODO: check for already completed OBJECTIVE_ITEM
                    int objindex = 0;
                    std::vector<Quest::QuestObjective>::iterator iter;
                    for(iter = q->objectives.begin(); iter != q->objectives.end(); ++iter)
                    {
                        if((*iter).type == Quest::OBJECTIVE_ITEM)
                        {
                            std::list<Item*>::iterator inviter;
                            for(inviter = ch->player->inventory.begin(); inviter != ch->player->inventory.end(); ++inviter)
                            {
                                if((*inviter)->id == ((Item*)((*iter).objective))->id)
                                {
                                    ch->player->questObjectives[ch->player->questLog.size()-1][objindex]++;
                                }
                            }
                            if(ch->player->questObjectives[ch->player->questLog.size()-1][objindex] > 0)
                            {
                                ch->Send("|W" + q->name + ": ");
                                ch->Send("|Y" + (*iter).description + " (" 
                                    + Utilities::itos(ch->player->questObjectives[ch->player->questLog.size()-1][objindex]) 
                                    + "/" + Utilities::itos((*iter).count) + ")|X\n\r\n\r");
                            }
                        }
                        objindex++;
                    }
                    return;
                }
                ctr++;
            }
        }
        ch->Send("Quest " + arg2 + " not found.\n\r");

        return;
    }
    else if(!Utilities::str_cmp(arg1, "drop"))
    {
        if(ch->player->questLog.empty())
        {
            ch->Send("Your quest log is empty.\n\r");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Drop which quest?\n\r");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->player->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->player->questLog.size()) + "\n\r");
            return;
        }
        ch->Send("You have dropped " + ch->player->questLog[qnum-1]->name + ".\n\r");
        ch->player->questLog.erase(ch->player->questLog.begin() + qnum - 1);
        ch->player->questObjectives.erase(ch->player->questObjectives.begin() + qnum - 1);
        return;
    }
    else if(!Utilities::str_cmp(arg1, "complete"))
    {
        if(ch->player->questLog.empty())
        {
            ch->Send("Your quest log is empty.\n\r");
            return;
        }
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to complete a quest.\n\r");
            return;
        }
        if(ch->GetTarget()->questEnd.empty())
        {
            ch->Send("Your target cannot complete your quest.\n\r");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Complete which quest?\n\r");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->player->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->player->questLog.size()) + "\n\r");
            return;
        }
        Quest * complete = ch->player->questLog[qnum-1];
        std::vector<Quest*>::iterator qiter;
        bool found = false;
        for(qiter = ch->GetTarget()->questEnd.begin(); qiter != ch->GetTarget()->questEnd.end(); ++qiter)
        {
            if((*qiter)->id == complete->id)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            ch->Send("Your target cannot complete your quest.\n\r");
            return;
        }
        if(!ch->player->QuestObjectivesComplete(complete))
        {
            ch->Send("Not all objectives of that quest are complete.\n\r");
            return;
        }

        //Remove items from inventory for OBJECTIVE_ITEM
        std::vector<Quest::QuestObjective>::iterator iter;
        for(iter = complete->objectives.begin(); iter != complete->objectives.end(); ++iter)
        {
            if((*iter).type == Quest::OBJECTIVE_ITEM)
            {
                int found = 0;
                std::list<Item*>::iterator inviter;
                for(inviter = ch->player->inventory.begin(); inviter != ch->player->inventory.end();)
                {
                    if((*inviter)->id == ((Item*)((*iter).objective))->id)
                    {
                        if(found < (*iter).count)
                        {
                            //remove from inventory
                            inviter = ch->player->inventory.erase(inviter);
                            ch->player->inventorySize--;
                        }
                        else
                        {
                            break;
                        }
                        found++;
                    }
                    else
                    {
                         ++inviter;
                    }
                }
            }
        }

        qiter = ch->player->questLog.begin() + qnum - 1;
        std::vector< std::vector<int> >::iterator objiter = ch->player->questObjectives.begin() + qnum - 1;
        ch->player->questLog.erase(qiter);
        ch->player->questObjectives.erase(objiter);
        ch->player->completedQuests.insert(complete->id);

        ch->Send(complete->name + " completed!\n\r");
        ch->Send(complete->completionMessage + "\n\r");
        //todo, rewards
        ch->Send("|BYou have gained |Y" + Utilities::itos(complete->experienceReward) + "|B experience.|X\n\r");
        ch->ApplyExperience(complete->experienceReward);
        return;
    }
    else if(!Utilities::str_cmp(arg1, "progress"))
    {
        if(ch->player->questLog.empty())
        {
            ch->Send("Your quest log is empty.\n\r");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Get progress information for which quest?\n\r");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->player->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->player->questLog.size()) + "\n\r");
            return;
        }
        Quest * progress = ch->player->questLog[qnum-1];
        ch->Send("Quest information for: " + Quest::GetDifficultyColor(ch->level, progress->level) 
                   + progress->name + " (" + Utilities::itos(progress->level) + ")|X\n\r");
        ch->Send(progress->longDescription + "\n\r\n\r");
        ch->Send(progress->shortDescription + "\n\r\n\r");
        ch->Send("You will receive |Y" + Utilities::itos(progress->experienceReward) + " experience|X and |Y" 
                   + Utilities::itos(progress->moneyReward) + " gold|X.\n\r");

        std::vector<Quest::QuestObjective>::iterator objiter;
        int i = 0;
        for(objiter = progress->objectives.begin(); objiter != progress->objectives.end(); ++objiter)
        {
            int currentCount = ch->player->questObjectives[qnum-1][i++];
            //type count objective description
            if(currentCount < (*objiter).count)
            {
                ch->Send("|Y" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")|X\n\r");
            }
            else
            {
                ch->Send("|G" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")|X\n\r");
            }
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "share"))
    {
        return;
    }
    ch->Send("Quest: log, list, info #, accept #, drop #, complete #, progress #, share #\n\r");
}

void cmd_quit(Character * ch, string argument)
{
    if(ch->hasQuery)
    {
        ch->Send("Answer your current question first.\n\r");
        return;
    }
    //Do a few checks here, but just query the player with a quit-function callback.
    if(ch->combat)
    {
        ch->Send("You can't do that while in combat!\n\r");
        return;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\n\r");
        return;
    }
	ch->queryData = NULL;
	ch->hasQuery = true;
	ch->queryPrompt = "Quit? (y/n) ";
	if (ch->level == 1)
	{
		ch->queryPrompt += "|R(Level one characters will not be saved)|X: ";
	}
	ch->queryFunction = cmd_quit_Query;
}

bool cmd_quit_Query(Character * ch, string argument)
{
    ch->QueryClear();
    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
	{
        if(ch && ch->combat)
        {
            ch->Send("You can't do that while in combat!\n\r");
            return true;
        }
        if(ch->delay_active)
        {
            ch->Send("You can't do that while casting.\n\r");
            return true;
        }

		ch->Send("Bye\n\r");

		//do this stuff on remove in the gameloop (to support user idle timeout quits)
        //ch->Message(ch->name + " has left the game.", Character::MSG_ROOM_NOTCHAR);

        //If we have a target, that target has US in their listener list. Remove that by clearing our target first.
        //ch->ClearTarget(); 

        //ch->ChangeRooms(NULL);
	   
        if(ch->player && ch->player->user)
        {
            /*ch->SaveSpellAffects();
            ch->SaveCooldowns();*/
			//ch->player->user->Disconnect();
			//ch->player->user->remove = true;
			ch->player->user->SetDisconnect();
        }
	}
    //LogFile::Log("status", ch->name + " \"quit\"");
	return true;
}

bool releaseSpiritQuery(Character * ch, string argument)
{
    if(!ch || !ch->player)
    { 
        //ch->QueryClear();
        return true;
    }

    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
    {
        //ch->RemoveAllSpellAffects();
        //set dead flag
        //SpellAffect * sa = ch->AddSpellAffect(true, ch, "player_is_ghost", true, false, 0, 0, 0, NULL);
        ch->player->isCorpse = false;
        ch->player->isGhost = true;
        ch->QueryClear();
        return true;
    }
    return false;
}