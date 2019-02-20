#include "stdafx.h"
#include "mud.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CItem.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CGroup.h"
#include "CClass.h"
#include "CExit.h"
#include "CUser.h"
#include "CClient.h"
#include "CNPC.h"
#include "CArea.h"
#include "CCommand.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "utils.h"
#include "CHelp.h"
#include "CLogFile.h"
// for convenience
using json = nlohmann::json;

extern class Command cmd_table[];

void cmd_attack(Player * ch, std::string argument)
{
    if(!ch)
        return;

    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    Character * target;
    if(arg1.empty())
    {
        if(ch->GetTarget())
        {
            target = ch->GetTarget();
			if (target->room != ch->room)
			{
				ch->Send("They aren't here.\r\n");
				return;
			}
        }
        else
        {
            ch->Send("Attack who?\r\n");
            return;
        }
    }
    else
    {
        target = ch->GetCharacterRoom(arg1);
        if(target == nullptr)
        {
            ch->Send("They aren't here.\r\n");
            return;
        }
    }
	if(!target->IsAlive())
	{
		ch->Send("That target is already dead!\r\n");
		return;
	}
    if(target == ch)
    {
        ch->Send("You can't attack yourself!\r\n");
        return;
    }
	if (!ch->CanAttack(target))
	{
		ch->Send("You can't attack that target.\r\n");
		return;
	}

    if (ch->HasActiveDelay())
    {
        ch->CancelActiveDelay();
        ch->Send("Action interrupted!\r\n");
    }

    ch->SetTarget(target);
    ch->Send("You begin attacking " + target->GetName() + "!\r\n");

    ch->EnterCombat(target);
    target->EnterCombat(ch);

    //Do first attack here so attacker has a minor advantage
    ch->AutoAttack(target);
}

/*void cmd_autoattack(Player * ch, std::string argument)
{
    //TODO 'set' autoattack to turn on/off automatic retaliation?
}*/

void cmd_cancel(Player * ch, std::string argument)
{
    if(ch && ch->CancelActiveDelay())
    {
        ch->Send("Action Cancelled!\r\n");
    }
    else if(ch && ch->meleeActive)
    {
        ch->Send("Melee attack cancelled.\r\n");
        ch->CancelAutoAttack();
    }
    else
    {
        ch->Send("No action is in progress.\r\n");
    }
}

void cmd_look(Player * ch, std::string argument)
{
    std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(arg1.empty()) //"look" with no argument
    {
        if(ch->room == nullptr)
	    {
            LogFile::Log("error", "cmd_look: ch->room == nullptr");
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
        std::stringstream roomstream;
        roomstream << " |W" << std::left << std::setw(55) << inroom->name <<
			std::setw(8) << (exitstatus[7] ? "|WNW" : "|B--") <<
			std::setw(7) << (exitstatus[0] ? "|WN" : "|B-") <<
            (exitstatus[1] ? "|WNE" : "|B--") << "|X\r\n";
        roomstream << std::left << std::setw(58) << "|B(-------------------------------------------------)" <<
            (exitstatus[6] ? "|WW" : "|B-") << "|Y <-" << (exitstatus[8] ? "|WU|Y" : "-") << "-X-" <<
            (exitstatus[9] ? "|WD|Y" : "-") << "-> " << (exitstatus[2] ? "|WE" : "|B-") << "|X\r\n";

        std::string areaname = "|X";
        Area * this_area = Game::GetGame()->GetArea(inroom->area);
        if(this_area != nullptr)
        {
            //TODO add pvp room rules to Game somewhere...
            if(this_area->pvp == 0) //no pvp whatsoever
                areaname = "|C";
            else if(this_area->pvp == 1) //dueling and guild pvp
                areaname = "|G";
            else if(this_area->pvp == 2) //everyone attacks everyone
                areaname = "|Y";
            else if(this_area->pvp == 3) //everyone attacks everyone, lose all your equipment?
                areaname = "|R";

            areaname += this_area->name;
        }

		roomstream << " " << std::left << std::setw(26) << areaname;
		std::string roomflags = "";
		if (Utilities::FlagIsSet(inroom->flags, Room::Flags::FLAG_RECALL))
			roomflags += "[Recall]";
		if (ch && ch->IsImmortal())
			roomflags += "[Room ID: " + Utilities::itos(inroom->id) + "]";

		roomstream << "|Y" << std::right << std::setw(26) << roomflags;
		roomstream << std::right << std::setw(9) << (exitstatus[5] ? "|WSW" : "|B--") <<
			std::setw(7) << (exitstatus[4] ? "|WS" : "|B-") <<
			std::setw(8) << (exitstatus[3] ? "|WSE" : "|B--") << "|X\r\n\r\n";

        ch->Send(roomstream.str());

        if(!inroom->description.empty())
	        ch->Send("  " + inroom->description);

		cmd_scan(ch, "");

		if (!ch->IsGhost() || (ch->IsGhost() && (FindDistance(inroom, Game::GetGame()->GetRoom(ch->corpse_room), 1) != -1 || inroom->id == ch->graveyard_room)))
		{
            if(!inroom->items.empty() || inroom->characters.size() > 1)
                ch->Send("\r\n");

			for (auto itemiter = inroom->items.begin(); itemiter != inroom->items.end(); itemiter++)
			{
				if(!itemiter->first->inroom_name.empty())
					ch->Send(itemiter->first->inroom_name + "\r\n");
				else
					ch->Send(itemiter->first->GetName() + "\r\n");
			}

			std::list<Character *>::iterator i;
			for (i = inroom->characters.begin(); i != inroom->characters.end(); i++)
			{
				if (ch == (*i))
					continue;

				if (!ch->IsImmortal() && (*i)->IsGhost())
				{
					continue;
				}

				//Determine appropriate quest icon. Hierarchy |Y?, |Y!, |W?, |D!
				//Search quests this npc ends
				std::string questicon = "";
				if ((*i)->IsNPC())
				{
					NPC * inroom_npc = (NPC*)(*i);
					bool foundyellow = false; bool foundwhite = false;
					if (!inroom_npc->GetNPCIndex()->questEnd.empty())
					{
						std::vector<Quest *>::iterator questiter;
						for (questiter = inroom_npc->GetNPCIndex()->questEnd.begin(); questiter != inroom_npc->GetNPCIndex()->questEnd.end(); ++questiter)
						{
							if (ch->QuestObjectivesComplete(*questiter))
							{
								questicon = "|Y[?] ";
								foundyellow = true;
								break;
							}
							else if (ch->QuestActive(*questiter))
							{
								questicon = "|W[?] ";
								foundwhite = true;
							}
						}
					}
					//Search quests this npc starts
					if (!foundyellow && !inroom_npc->GetNPCIndex()->questStart.empty())
					{
						std::vector<Quest *>::iterator questiter;
						for (questiter = inroom_npc->GetNPCIndex()->questStart.begin(); questiter != inroom_npc->GetNPCIndex()->questStart.end(); ++questiter)
						{
							if (ch->QuestEligible(*questiter))
							{
								//TODO: white ! for soon to be available quests
								if (!foundwhite && Quest::GetDifficulty(ch->GetLevel(), (*questiter)->level) == 0)
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
				}
				std::string disconnected = "";
				std::string title = "";
				std::string corpse = "";
				std::string targeting = ".";
				std::string level = "";
				std::string aggressionColor = "|G";
                std::string crowd_control = "";
                std::string stunned = "";
				std::string tapped = "";
                std::string in_combat = "";

				Player * inroom_player = nullptr;
				if ((*i)->IsPlayer())
					inroom_player = (Player*)(*i);

				if ((*i)->IsPlayer() && inroom_player->user && !inroom_player->user->IsConnected())
					disconnected = "|Y[DISCONNECTED] |X";

				if ((*i)->IsPlayer())
					title = (*i)->GetTitle();
				else if (!(*i)->GetTitle().empty())
					title = " <" + (*i)->GetTitle() + ">";

				level += "<" +
					Game::LevelDifficultyColor(Game::LevelDifficulty(ch->GetLevel(), (*i)->GetLevel()))
					+ Utilities::itos((*i)->GetLevel()) + "|X> ";

				if ((*i)->IsCorpse())
				{
					corpse = "|DThe corpse of ";
				}
				if (ch->IsImmortal() && (*i)->IsGhost())
				{
					corpse = "The ghost of ";
				}
				if ((*i)->InCombat())
				{
                    in_combat = "|R(X)|X ";
                    if((*i)->GetTarget() && (*i)->GetTarget() == ch)
                        targeting = ", targeting YOU!";
                    else if ((*i)->GetTarget())
                        targeting = ", targeting " + (*i)->GetTarget()->GetName() + ".";
					/*
                    if ((*i)->IsFighting(ch))
						fighting = ", fighting YOU!";
					else if ((*i)->GetTarget() != nullptr && (*i)->IsFighting((*i)->GetTarget()))
						fighting = ", fighting " + (*i)->GetTarget()->GetName() + ".";
                    */
				}
				aggressionColor = ch->AggressionColor((*i));
                SpellAffect * cc = (*i)->GetFirstSpellAffectWithAura(SpellAffect::AURA_INCAPACITATE);
                if (cc != nullptr)
                {
                    crowd_control = cc->GetDataString("cmd_look_cc");
                }
                cc = (*i)->GetFirstSpellAffectWithAura(SpellAffect::AURA_STUN);
                if (cc != nullptr)
                {
                    stunned = "|C*";
                }
				Character * tappedBy = (*i)->GetTap();
				if (tappedBy)
				{
                    if (tappedBy->InSameGroup(ch))
                        tapped += "|G";
                    else
                        tapped += "|D";
                    tapped += "(tapped by ";
                    if (tappedBy == ch)
                        tapped += "YOU!)";
                    else
                        tapped += tappedBy->GetName() + ")";
				}
				ch->Send(disconnected + level + questicon + in_combat + stunned + aggressionColor + corpse + (*i)->GetName() + title + " is here" + targeting + crowd_control + tapped + "|X\r\n");
			}
			//ch->Send("\r\n");	
		}
    }
    else // "look argument"
    {
        //look <direction>
        for (int i = 0; i < Exit::DIR_LAST; i++)
        {
            if (!Utilities::str_cmp(arg1, Exit::exitNames[i]))
            {
                if (!ch->room->exits[i] || !ch->room->exits[i]->to)
                {
                    ch->Send("There is no exit in that direction.\r\n");
                    return;
                }
                Room * inroom = ch->room->exits[i]->to;
                bool exitstatus[Exit::DIR_LAST];
                for (int i = 0; i < Exit::DIR_LAST; i++)
                {
                    if (inroom->exits[i] && inroom->exits[i]->to)
                        exitstatus[i] = true;
                    else
                        exitstatus[i] = false;
                }
                std::stringstream roomstream;
                roomstream << " |W" << std::left << std::setw(55) << inroom->name <<
                    std::setw(8) << (exitstatus[7] ? "|WNW" : "|B--") <<
                    std::setw(7) << (exitstatus[0] ? "|WN" : "|B-") <<
                    (exitstatus[1] ? "|WNE" : "|B--") << "|X\r\n";
                roomstream << std::left << std::setw(58) << "|B(-------------------------------------------------)" <<
                    (exitstatus[6] ? "|WW" : "|B-") << "|Y <-" << (exitstatus[8] ? "|WU|Y" : "-") << "-X-" <<
                    (exitstatus[9] ? "|WD|Y" : "-") << "-> " << (exitstatus[2] ? "|WE" : "|B-") << "|X\r\n";

                std::string areaname = "|X";
                Area * this_area = Game::GetGame()->GetArea(inroom->area);
                if (this_area != nullptr)
                {
                    //TODO add pvp room rules to Game somewhere...
                    if (this_area->pvp == 0) //no pvp whatsoever
                        areaname = "|C";
                    else if (this_area->pvp == 1) //dueling and guild pvp
                        areaname = "|G";
                    else if (this_area->pvp == 2) //everyone attacks everyone
                        areaname = "|Y";
                    else if (this_area->pvp == 3) //everyone attacks everyone, lose all your equipment?
                        areaname = "|R";

                    areaname += this_area->name;
                }

                roomstream << " " << std::left << std::setw(26) << areaname;
                std::string roomflags = "";
                if (Utilities::FlagIsSet(inroom->flags, Room::Flags::FLAG_RECALL))
                    roomflags += "[Recall]";
                if (ch && ch->IsImmortal())
                    roomflags += "[Room ID: " + Utilities::itos(inroom->id) + "]";

                roomstream << "|Y" << std::right << std::setw(26) << roomflags;
                roomstream << std::right << std::setw(9) << (exitstatus[5] ? "|WSW" : "|B--") <<
                    std::setw(7) << (exitstatus[4] ? "|WS" : "|B-") <<
                    std::setw(8) << (exitstatus[3] ? "|WSE" : "|B--") << "|X\r\n\r\n";

                ch->Send(roomstream.str());

                if (!inroom->description.empty())
                    ch->Send("  " + inroom->description);

                if (!ch->IsGhost() || (ch->IsGhost() && (inroom->id == ch->corpse_room || inroom->id == ch->graveyard_room)))
                {
                    for (auto itemiter = inroom->items.begin(); itemiter != inroom->items.end(); itemiter++)
                    {
                        if (!itemiter->first->inroom_name.empty())
                            ch->Send(itemiter->first->inroom_name + "\r\n");
                        else
                            ch->Send(itemiter->first->GetName() + "\r\n");
                    }

                    std::list<Character *>::iterator i;
                    for (i = inroom->characters.begin(); i != inroom->characters.end(); i++)
                    {
                        if (ch == (*i))
                            continue;

                        if (!ch->IsImmortal() && (*i)->IsGhost())
                        {
                            continue;
                        }

                        //Determine appropriate quest icon. Hierarchy |Y?, |Y!, |W?, |D!
                        //Search quests this npc ends
                        std::string questicon = "";
                        if ((*i)->IsNPC())
                        {
                            NPC * inroom_npc = (NPC*)(*i);
                            bool foundyellow = false; bool foundwhite = false;
                            if (!inroom_npc->GetNPCIndex()->questEnd.empty())
                            {
                                std::vector<Quest *>::iterator questiter;
                                for (questiter = inroom_npc->GetNPCIndex()->questEnd.begin(); questiter != inroom_npc->GetNPCIndex()->questEnd.end(); ++questiter)
                                {
                                    if (ch->QuestObjectivesComplete(*questiter))
                                    {
                                        questicon = "|Y[?] ";
                                        foundyellow = true;
                                        break;
                                    }
                                    else if (ch->QuestActive(*questiter))
                                    {
                                        questicon = "|W[?] ";
                                        foundwhite = true;
                                    }
                                }
                            }
                            //Search quests this npc starts
                            if (!foundyellow && !inroom_npc->GetNPCIndex()->questStart.empty())
                            {
                                std::vector<Quest *>::iterator questiter;
                                for (questiter = inroom_npc->GetNPCIndex()->questStart.begin(); questiter != inroom_npc->GetNPCIndex()->questStart.end(); ++questiter)
                                {
                                    if (ch->QuestEligible(*questiter))
                                    {
                                        //TODO: white ! for soon to be available quests
                                        if (!foundwhite && Quest::GetDifficulty(ch->GetLevel(), (*questiter)->level) == 0)
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
                        }
                        std::string disconnected = "";
                        std::string title = "";
                        std::string corpse = "";
                        std::string targeting = ".";
                        std::string level = "";
                        std::string aggressionColor = "|G";
                        std::string crowd_control = "";
                        std::string stunned = "";
                        std::string tapped = "";
                        std::string in_combat = "";

                        Player * inroom_player = nullptr;
                        if ((*i)->IsPlayer())
                            inroom_player = (Player*)(*i);

                        if ((*i)->IsPlayer() && inroom_player->user && !inroom_player->user->IsConnected())
                            disconnected = "|Y[DISCONNECTED] |X";

                        if ((*i)->IsPlayer())
                            title = (*i)->GetTitle();
                        else if (!(*i)->GetTitle().empty())
                            title = " <" + (*i)->GetTitle() + ">";

                        level += "<" +
                            Game::LevelDifficultyColor(Game::LevelDifficulty(ch->GetLevel(), (*i)->GetLevel()))
                            + Utilities::itos((*i)->GetLevel()) + "|X> ";

                        if ((*i)->IsCorpse())
                        {
                            corpse = "|DThe corpse of ";
                        }
                        if (ch->IsImmortal() && (*i)->IsGhost())
                        {
                            corpse = "The ghost of ";
                        }
                        if ((*i)->InCombat())
                        {
                            in_combat = "|R(X)|X ";
                            if ((*i)->GetTarget() && (*i)->GetTarget() == ch)
                                targeting = ", targeting YOU!";
                            else if ((*i)->GetTarget())
                                targeting = ", targeting " + (*i)->GetTarget()->GetName() + ".";
                            /*
                            if ((*i)->IsFighting(ch))
                            fighting = ", fighting YOU!";
                            else if ((*i)->GetTarget() != nullptr && (*i)->IsFighting((*i)->GetTarget()))
                            fighting = ", fighting " + (*i)->GetTarget()->GetName() + ".";
                            */
                        }
                        aggressionColor = ch->AggressionColor((*i));
                        SpellAffect * cc = (*i)->GetFirstSpellAffectWithAura(SpellAffect::AURA_INCAPACITATE);
                        if (cc != nullptr)
                        {
                            crowd_control = cc->GetDataString("cmd_look_cc");
                        }
                        cc = (*i)->GetFirstSpellAffectWithAura(SpellAffect::AURA_STUN);
                        if (cc != nullptr)
                        {
                            stunned = "|C*";
                        }
                        Character * tappedBy = (*i)->GetTap();
                        if (tappedBy)
                        {
                            if (tappedBy->InSameGroup(ch))
                                tapped += "|G";
                            else
                                tapped += "|D";
                            tapped += "(tapped by ";
                            if(tappedBy == ch)
                                tapped += "YOU!)";
                            else
                                tapped += tappedBy->GetName() + ")";
                        }
                        ch->Send(disconnected + level + questicon + in_combat + stunned + aggressionColor + corpse + (*i)->GetName() + title + " is here" + targeting + crowd_control + tapped + "|X\r\n");
                    }
                }
                return;
            }
        }

        //Search for a player to look at
		Character * ch_inspect = ch->GetCharacterRoom(arg1);
		if (ch_inspect != nullptr && ch_inspect->IsPlayer())
		{
			ch->Send(ch_inspect->GetName() + " is currently wearing:\r\n");
			ch->Send(((Player*)ch_inspect)->FormatEquipment().str());
			return;
		}

        //Search for an inventory/equipped item to look at
        Item * inspect = ch->GetItemInventory(arg1);
        if(inspect == nullptr)
        {
            inspect = ch->GetItemEquipped(arg1);
        }
        if(!inspect)
        {
            ch->Send("You don't see that here.\r\n");
            return;
        }
		ch->Send(inspect->FormatItemInfo(ch));
    }
}

// adds the target's health/mana/energy/rage to the players prompt
// spells/skills done to a character automatically target that character
// allow targeting in same room and adjacent rooms
void cmd_target(Player * ch, std::string argument)
{
    if(argument.empty())
	{
		ch->Send("Target cleared.\r\n");
		ch->ClearTarget();
        ch->CancelAutoAttack(); //Any target change stops auto attack
		return;
	}

	std::string arg1;
	std::string direction;
    argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, direction);

	Character *vch;
	if((vch = ch->GetCharacterAdjacentRoom(arg1, direction)) != nullptr ||
        (ch->IsImmortal() 
         && (vch = Game::GetGame()->GetPlayerWorld(ch, arg1)) != nullptr))
	{
        if(ch->GetTarget() != vch)
            ch->CancelAutoAttack(); //Any target change stops auto attack
		ch->SetTarget(vch);
		ch->Send("Targeting " + vch->GetName() + "\r\n");
	}
    else
    {
        ch->Send("Target not found.\r\n");
    }
}

void cmd_commands(Player * ch, std::string argument)
{
	Command * whichTable = Command::GetCurrentCmdTable(ch);

    int newline = 1;
    for(int cmd = 0; whichTable[cmd].name.length() > 0; cmd++)
	{
		if (whichTable[cmd].level >= 0 || ch->IsImmortal())
		{
			ch->Send(whichTable[cmd].name + "  ");
			newline++;
		}
		if(newline % 5 == 0)
			ch->Send("\r\n");
		
	}
	ch->Send("\r\n");
}

void cmd_score(Player * ch, std::string argument)
{
	std::stringstream score;
	score.setf(std::ios_base::fixed);

	score << "|BPlayer Information for: |X" << ch->GetName() << " " << ch->GetTitle() << "|X\r\n";
	score << "|BTotal Level: |X" << std::setw(11) << std::left << ch->GetLevel();
	score << " |BClass Level: |X" << ch->GetClassLevel(ch->currentClass->GetID()) << "\r\n";
	if (ch->IsImmortal())
		score << "|BImmortal Level: |X" << ch->GetImmLevel() << "\r\n";
	score << "|BRace: |X" << std::setw(18) << std::left << Character::race_table[ch->race].name;
	score << " |BGender: |X" << (ch->GetGender() ? "male\r\n" : "female\r\n");
	score << "|BClass: |X" << std::setw(17) << std::left << ch->currentClass->name;
	score << " |BClass History: |X";
	

    bool found = false;
    std::list<Player::ClassData>::iterator iter;
    for(iter = ch->classList.begin(); iter != ch->classList.end(); ++iter)
    {
        Class * c = Game::GetGame()->GetClass((*iter).id);
		score << "[" << c->name << " " << iter->level << "] ";
        found = true;
    }
    if(found)
    {
		score << "\r\n";
    }
    else
    {
		score << "None\r\n";
    }

	score << "|BHealth: |X" << std::setw(5) << std::right << ch->GetHealth() << "|B/|X" << std::setw(11) << std::left << ch->GetMaxHealth();
	score << "|BMana: |X" << std::setw(5) << std::right << ch->GetMana() << "|B/|X" << ch->GetMaxMana() << "\r\n";
	score << "|BEnergy: |X" << std::setw(5) << std::right << ch->GetEnergy() << "|B/|X" << std::setw(11) << std::left << ch->GetMaxEnergy();
	score << "|BRage: |X" << std::setw(5) << std::right << ch->GetRage() << "|B/|X" << ch->GetMaxRage() << "\r\n";

	score << "|BAgility: |X" << std::setw(4) << std::left << ch->GetAgility() << "|G+" << std::setw(11) << ch->GetAgility() - ch->GetBaseAgility();
	score << "|BIntellect: |X" << ch->GetIntellect() << " |G+" << ch->GetIntellect() - ch->GetBaseIntellect() << "\r\n";
	score << "|BStrength: |X" << std::setw(4) << std::left << ch->GetStrength() << "|G+" << std::setw(10) << ch->GetStrength() - ch->GetBaseStrength();
	score << "|BStamina: |X" << ch->GetStamina() << " |G+" << ch->GetStamina() - ch->GetBaseStamina() << "\r\n";
	score << "|BWisdom: |X" << std::setw(4) << std::left << ch->GetWisdom() << "|G+" << std::setw(12) << ch->GetWisdom() - ch->GetBaseWisdom();
	score << "|BSpirit: |X" << ch->GetSpirit() << " |G+" << ch->GetSpirit() - ch->GetBaseSpirit() << "\r\n";

	score << "|BAttribute Points: |X" << ch->statPoints << "\r\n";

	score << "|BArmor: |X" << ch->GetArmor() << " |BPhysical damage reduction: |X" << std::setprecision(2) << ch->CalculateArmorMitigation()*100 << "%\r\n";

	score << "|BDodge chance: |X" << std::setw(5) << std::right << std::setprecision(2) << ch->GetDodge() << std::setw(6) << std::left << "%";
	score << "|BCritical Strike chance: |X" << std::setprecision(2) << ch->GetCrit() << "%\r\n";

	score << "|BYou can wear the following armor types: |X";

	if (ch->CanWearArmor(Item::TYPE_ARMOR_CLOTH))
		score << "Cloth ";
	if (ch->CanWearArmor(Item::TYPE_ARMOR_LEATHER))
		score << "Leather ";
	if (ch->CanWearArmor(Item::TYPE_ARMOR_MAIL))
		score << "Mail ";
	if (ch->CanWearArmor(Item::TYPE_ARMOR_PLATE))
		score << "Plate ";
	score << "\r\n";

    score << "|BYou have |X" << ch->experience << "|B experience and need |X"
		<< Game::ExperienceForLevel(ch->GetLevel()+1) << "|B experience to reach level |X"
		<< ch->GetLevel()+1 << "|B (|X" << (Game::ExperienceForLevel(ch->GetLevel()+1) - ch->experience) << "|B tnl)|X\r\n";

    double movespeed = ch->GetMoveSpeed();
    score << "|BCurrent movement speed: |X" << std::setprecision(2) << ch->movementSpeed * movespeed << "|B rooms per second (|X"
		<< std::setprecision(0) << movespeed*100 << "% |Bof normal)|X\r\n";

	if (!ch->IsAlive())
	{
		int res_at_graveyard = ch->death_timer - ch->TimeSinceDeath();
		int res_at_corpse = ch->death_timer_runback - ch->TimeSinceDeath();
		if (res_at_graveyard > 0)
			score << "|W" << res_at_graveyard << " seconds before you can resurrect at the graveyard.\r\n";
		if (res_at_corpse > 0)
			score << "|W" << res_at_corpse << " seconds before you can resurrect near your corpse.\r\n";
	}
	ch->Send(score.str());
}

void cmd_scan(Player * ch, std::string argument)
{
	if (!ch || !ch->room)
		return;

	if (!ch->IsAlive())
		return;

	std::list<Character *>::iterator iter;
	Room * scan_room;
	std::stringstream out;
	int depth = 3;
	//std::string depthcolors[3] = { "|R", "|Y", "|G" };
	std::string depthstring[3] = { "||  ", "|||| ", "||||||" };
	bool found = false;
	std::string level = "";
	std::string corpse = "";

	for (int i = 0; i < Exit::DIR_LAST; i++)
	{
		out << std::setw(11) << std::left << Exit::exitNames[i] + ": ";
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
			if(scan_room->HasNonGhostCharacters())
			{
				if (found == true)
				{
					out << std::setw(13) << std::left << "\r\n";
				}
				found = true;
				out << "|W[" << /*depthcolors[j]*/"|202" << depthstring[j] << "|W]:|X ";
				std::list<Character *>::iterator iter = scan_room->characters.begin();
				while (iter != scan_room->characters.end())
				{
					if (!(*iter)->IsAlive())
						corpse = "corpse of ";
					else
						corpse = "";

					//if (j == 0) //depth
					{
						level = "<" +
							Game::LevelDifficultyColor(Game::LevelDifficulty(ch->GetLevel(), (*iter)->GetLevel()))
							+ Utilities::itos((*iter)->GetLevel()) + "|X>";
						out << level << ch->AggressionColor(*iter) << corpse << (*iter)->GetName();
					}
					/*else
					{
						level = "<" +
							Game::LevelDifficultyLightColor(Game::LevelDifficulty(ch->level, (*iter)->level))
							+ Utilities::itos((*iter)->level) + "|X>";
						out << level << ch->AggressionLightColor(*iter) << corpse << (*iter)->name;
					}*/
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
			out << "|X\r\n";
			ch->Send(out.str());
			found = false;
		}
		out.str("");
		out.clear();
	}
}

void cmd_who(Player * ch, std::string argument)
{
	std::string arg1;
	argument = Utilities::one_argument(argument, arg1);

	if (arg1.empty()) //"who" with no arguments
	{
		ch->Send("|YPlayers online: |X\r\n");
		ch->Send("|B`-------------------------------------------------------------------------------'|X\r\n");
		int found = 0;
		Player * wplayer;
		std::list<User *>::iterator iter;
		std::map<int, Class *>::iterator classiter;
		Class * myclass = nullptr;
		std::stringstream sstr;
		for (iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
		{
			if ((*iter)->IsConnected() && (*iter)->connectedState == User::CONN_PLAYING && (*iter)->character)
			{
				wplayer = (*iter)->character;
				sstr.str("");
				sstr.clear();
				//name
				sstr << " " << std::left << std::setw(12) << (*iter)->character->GetName();
				//total level
				sstr << " |B[" << std::right;
				if (wplayer->IsImmortal())
				{
					sstr << "|Y" << std::setw(3) << wplayer->GetImmLevel();

				}
				else
				{
					sstr << "|W" << std::setw(3) << (*iter)->character->GetLevel();
				}
				sstr << "|B]";
				//gender
				sstr << " |B" << std::right << ((*iter)->character->GetGender() == 1 ? "M" : "F");
				//race
				sstr << " |B" << std::left << std::setw(8) << Character::race_table[(*iter)->character->race].name << " ";
				//in combat & ghost
				sstr << std::left << ((*iter)->character->InCombat() ? "|R<X>" : "   ") << ((*iter)->character->IsGhost() ? "|D<G>" : "   ");
				//area
				if ((*iter)->character->room)
				{
					Area * a = Game::GetGame()->GetArea((*iter)->character->room->area);
					if (a)
						sstr << " |B[|C" << a->name << "|B]";
				}
				sstr << "|X\r\n";

				ch->Send(sstr.str());
				found++;
			}
		}
		ch->Send("|B`-------------------------------------------------------------------------------'|X\r\n");
		ch->Send(Utilities::itos(found) + " players found.\r\n");
	}
	else //"who" with argument. For now just look up by character name
	{
		Player * wplayer;
		std::list<User *>::iterator iter;
		std::map<int, Class *>::iterator classiter;
		Class * myclass = nullptr;
		std::stringstream sstr;
		for (iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
		{
			if ((*iter)->IsConnected() && (*iter)->connectedState == User::CONN_PLAYING && (*iter)->character 
				&& !Utilities::str_cmp((*iter)->character->GetName(), arg1))
			{ 
				wplayer = (*iter)->character;
				ch->Send("|B`-------------------------------------------------------------------------------'|X\r\n");
				sstr.str("");
				sstr.clear();
				//name
				sstr << " " << std::left << std::setw(12) << (*iter)->character->GetName();
				//total level
				sstr << " |B[" << std::right;
				if (wplayer->IsImmortal())
				{
					sstr << "|Y" << std::setw(3) << wplayer->GetImmLevel();

				}
				else
				{
					sstr << "|W" << std::setw(3) << (*iter)->character->GetLevel();
				}
				sstr << "|B]";
				//gender
				sstr << " |B" << std::right << ((*iter)->character->GetGender() == 1 ? "M" : "F");
				//race
				sstr << " |B" << std::left << std::setw(8) << Character::race_table[(*iter)->character->race].name << " ";
				//in combat & ghost
				sstr << std::left << ((*iter)->character->InCombat() ? "|R<X>" : "   ") << ((*iter)->character->IsGhost() ? "|D<G>" : "   ");
				//area
				if ((*iter)->character->room)
				{
					Area * a = Game::GetGame()->GetArea((*iter)->character->room->area);
					if (a)
						sstr << " |B[|C" << a->name << "|B]";
				}
				//classes
				sstr << "\r\n              ";
				if (wplayer->IsImmortal())
				{
					sstr << "|B[|WImmortal|B]";
				}
				else
				{
					for (classiter = Game::GetGame()->classes.begin(); classiter != Game::GetGame()->classes.end(); classiter++)
					{
						myclass = (*classiter).second;
						sstr << "|B[" << myclass->color << myclass->name << " " << std::right << std::setw(3) << Utilities::itos(wplayer->GetClassLevel(myclass->GetID())) << "|B]";
					}
				}
				sstr << "|X\r\n";
				ch->Send(sstr.str());
				ch->Send("|B`-------------------------------------------------------------------------------'|X\r\n");
			}
		}
	}
}

void cmd_title(Player * ch, std::string argument)
{   
    if(argument.empty())
	{
		ch->Send("Title cleared.\r\n");
        ch->title.clear();
		return;
	}
	if(argument.length() > 45)
    {
        ch->Send("Title too long. Shortened to 45 characters.\r\n");
		argument = argument.substr(0, 45);
    }

	if(argument[0] != '.' && argument[0] != ',' && argument[0] != '!' && argument[0] != '?' )
	{
		argument = ' ' + argument;
	}
	ch->title = argument;
	ch->Send("Title set.\r\n");
}

void cmd_group(Player * ch, std::string argument)
{
	std::string arg1;
	std::string arg2;
	Player * vch;

	argument = Utilities::one_argument(argument, arg1);
	
	if (!Utilities::str_cmp(arg1, "invite"))
	{
		argument = Utilities::one_argument(argument, arg2);

		if (ch->HasGroup() && !ch->group->IsGroupLeader(ch))
		{
			ch->Send("Only the group leader can invite players.\r\n");
			return;
		}
		if (arg2.empty())
		{
			ch->Send("Invite who?\r\n");
			return;
		}
		int slot = -1;
		if (ch->HasGroup())
			slot = ch->group->FindNextEmptySlot();
		if (ch->HasGroup() && slot == -1)
		{
			ch->Send("Your group is full.\r\n");
			return;
		}
		if ((vch = Game::GetGame()->GetPlayerByName(arg2)) != nullptr)
		{
			if (vch == ch)
			{
				ch->Send("You can't invite yourself.\r\n");
				return;
			}
			if (vch->HasGroup())
			{
				ch->Send("That player already has a group.\r\n");
				return;
			}
			if (vch->HasQuery(cmd_groupQuery))
			{
				ch->Send("That player has a pending group invitation.\r\n");
				return;
			}
			ch->Send("You invite " + vch->GetName() + " to join your group.\r\n");
			vch->AddQuery(ch->GetName() + " has invited you to join " + ch->HisHer() + " group ('accept'/'decline') ", (void*)ch, cmd_groupQuery);
			ch->AddSubscriber(vch); //If the person asking us to join disappears before we answer it's a problem
			vch->Send("\r\n");
		}
		else
		{
			ch->Send("Player not found.\r\n");
			return;
		}
		return;
	}
	if (!Utilities::str_cmp(arg1, "remove"))
	{
		argument = Utilities::one_argument(argument, arg2);

		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		if (!ch->group->IsGroupLeader(ch))
		{
			ch->Send("Only the group leader can remove players.\r\n");
			return;
		}
		if (arg2.empty())
		{
			ch->Send("Who would you like to remove from your group?\r\n");
			return;
		}
		Player * remove_me = ch->group->FindByName(arg2);
		if (remove_me == nullptr)
		{
			ch->Send("A player with that name is not in your group.\r\n");
			return;
		}
		if (remove_me == ch)
		{
			ch->Send("You cannot remove yourself from the group. Use \"group leave\".\r\n");
			return;
		}

		remove_me->Send("You have been removed from the group.\r\n");
		ch->group->Remove(remove_me);
		ch->Message(remove_me->GetName() + " has been removed from the group.", Character::MSG_GROUP);

		if (ch->group->GetMemberCount() <= 1)
		{
			ch->Send("Your group has been disbanded.\r\n");
			delete ch->group;
			ch->group = nullptr;
		}
		return;
	}
	if (!Utilities::str_cmp(arg1, "leave"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		ch->Message(ch->GetName() + " has left the group.", Character::MSG_GROUP_NOTCHAR);
		ch->Send("You have left the group.\r\n");
		if (ch->group->GetMemberCount() <= 2)
		{
			ch->Message("Your group has been disbanded.", Character::MSG_GROUP_NOTCHAR);
			for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
			{
				if (ch->group->GetMember(i) != nullptr && ch->group->GetMember(i) != ch)
				{
					ch->group->GetMember(i)->group = nullptr;
				}
			}
			delete ch->group;
			ch->group = nullptr;
		}
		else
		{
			if (ch->group->IsGroupLeader(ch))
			{
				for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
				{
					if (ch->group->GetMember(i) != ch && ch->group->GetMember(i) != nullptr)
					{
						ch->group->leader = ch->group->GetMember(i);
						ch->group->leader->Message(ch->group->leader->GetName() + " is now the group leader.", Character::MSG_GROUP_NOTCHAR);
						ch->group->leader->Send("You are now the group leader.\r\n");
						break;
					}
				}
			}
			ch->group->Remove(ch);
		}
		return;
	}
	if (!Utilities::str_cmp(arg1, "raid"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		if (!ch->group->IsGroupLeader(ch))
		{
			ch->Send("You aren't the group leader.\r\n");
			return;
		}
		if (ch->group->IsRaidGroup())
		{
			ch->Send("You are already in a raid group.\r\n");
			return;
		}
		ch->group->MakeRaidGroup();
		ch->Message("You have joined a raid group.", Character::MSG_GROUP);
		return;
	}
	if (!Utilities::str_cmp(arg1, "leader"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		if (!ch->group->IsGroupLeader(ch))
		{
			ch->Send("You aren't the group leader.\r\n");
			return;
		}
		argument = Utilities::one_argument(argument, arg2);

		if (arg2.empty())
		{
			ch->Send("Who would you like to make the new group leader?\r\n");
			return;
		}
		Player * new_leader = ch->group->FindByName(arg2);
		if (new_leader == nullptr)
		{
			ch->Send("A player with that name is not in your group.\r\n");
			return;
		}
		if (new_leader == ch)
		{
			ch->Send("You are already the group leader.\r\n");
			return;
		}
		ch->group->leader = new_leader;
		new_leader->Message(new_leader->GetName() + " is now the group leader.", Character::MSG_GROUP_NOTCHAR);
		new_leader->Send("You are now the group leader.\r\n");
		return;
	}
	if (!Utilities::str_cmp(arg1, "talk"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		if (argument.empty())
		{
			ch->Send("What would you like to say?\r\n");
			return;
		}
		ch->Send("|CYou say to the group: '" + argument + "|C'|X\r\n");
		ch->Message("|C" + ch->GetName() + " says to the group, '" + argument + "|C'|X", Character::MSG_GROUP_NOTCHAR);
		return;
	}
	if (!Utilities::str_cmp(arg1, "move"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		if (!ch->group->IsGroupLeader(ch))
		{
			ch->Send("You aren't the group leader.\r\n");
			return;
		}
		if (!ch->group->IsRaidGroup())
		{
			ch->Send("You aren't in a raid group.\r\n");
			return;
		}
		std::string player_arg;
		std::string slot_arg;
		argument = Utilities::one_argument(argument, player_arg);
		argument = Utilities::one_argument(argument, slot_arg);

		if (player_arg.empty() || slot_arg.empty() || !Utilities::IsNumber(slot_arg))
		{
			ch->Send("Usage: group move <player> <slot #>\r\n");
			return;
		}
		Player * group_member = ch->group->FindByName(player_arg);
		if (!group_member)
		{
			ch->Send("That player is not in your group.\r\n");
			return;
		}
		int slot_num = Utilities::atoi(slot_arg);
		if (slot_num <= 0 || slot_num > Group::MAX_RAID_SIZE)
		{
			ch->Send("Slot number out of range.\r\n");
			return;
		}
		ch->group->Move(group_member, slot_num - 1);
		for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
		{
			if (ch->group->GetMember(i) != nullptr)
			{
				json groupstatus = { { "move", "playermoved" } };
				ch->group->GetMember(i)->SendGMCP("group.status " + groupstatus.dump());
			}
		}
		return;
	}
	if (!Utilities::str_cmp(arg1, "show"))
	{
		if (!ch->HasGroup())
		{
			ch->Send("You aren't in a group.\r\n");
			return;
		}
		std::stringstream group_format;

		//Display non-raid group
		if (!ch->group->IsRaidGroup())
		{
			group_format << "Your group (" << ch->group->GetMemberCount() << "/" << Group::MAX_GROUP_SIZE << "):\r\n";
			for (int i = 0; i < Group::MAX_GROUP_SIZE; i++)
			{
				Character * current_member = ch->group->GetMember(i);
				if (current_member != nullptr)
				{
					current_member->InCombat() ? group_format << "|R(X)|X" : group_format << "   ";
					current_member == ch->group->leader ? group_format << "|Y*|X" : group_format << " ";

					group_format << "|C" << std::left << std::setw(12) << current_member->GetName() << "|X";

					//Health
					int percent;
					std::string statColor;
					if (current_member->GetHealth() > 0 && current_member->GetMaxHealth() > 0)
						percent = (current_member->GetHealth() * 100) / current_member->GetMaxHealth();
					else
						percent = 0;

					if (percent >= 75)
						statColor = "|x";
					else if (percent >= 50)
						statColor = "|G";
					else if (percent >= 25)
						statColor = "|Y";
					else
						statColor = "|R";

					group_format << statColor << std::right << std::setw(4) << current_member->GetHealth() << "|X/" << std::left << std::setw(4) << current_member->GetMaxHealth() << "  ";
				}
			}
			group_format << "\r\n";
		}
		//Display raid group
		else
		{
			group_format << "Your raid group (" << ch->group->GetMemberCount() << "/" << Group::MAX_RAID_SIZE << "):\r\n";
			for (int format_loop = 0; format_loop < (Group::MAX_RAID_SIZE / Group::MAX_GROUP_SIZE) / 2; format_loop++)
			{
				group_format << "           Group " << (format_loop * 2) + 1 << "                  ";
				group_format << "Group " << (format_loop * 2) + 2 << "\r\n";
				for (int i = 0; i < Group::MAX_GROUP_SIZE; i++)
				{
					for (int j = 0; j < 2; j++)
					{
						Character * current_member = ch->group->GetMember(i + (format_loop * 8) + (j * 4));
						if (current_member != nullptr)
						{
							current_member->InCombat() ? group_format << "|R(X)|X" : group_format << "   ";
							current_member == ch->group->leader ? group_format << "|Y*|X" : group_format << " ";

							group_format << "|C" << std::left << std::setw(12) << current_member->GetName() << "|X";

							//Health
							int percent;
							std::string statColor;
							if (current_member->GetHealth() > 0 && current_member->GetMaxHealth() > 0)
								percent = (current_member->GetHealth() * 100) / current_member->GetMaxHealth();
							else
								percent = 0;

							if (percent >= 75)
								statColor = "|x";
							else if (percent >= 50)
								statColor = "|G";
							else if (percent >= 25)
								statColor = "|Y";
							else
								statColor = "|R";

							group_format << statColor << std::right << std::setw(4) << current_member->GetHealth() << "|X/" << std::left << std::setw(4) << current_member->GetMaxHealth();

						}
						else
						{
							group_format << "    Empty                ";
						}
					}
					group_format << "\r\n";
				}
				group_format << "\r\n";
			}
		}
		ch->Send(group_format.str());
		return;
	}
	ch->Send("group invite <player>\r\n");
	ch->Send("group show\r\n");
	ch->Send("group remove <player>\r\n");
	ch->Send("group leave\r\n");
	ch->Send("group talk <message>\r\n");
	ch->Send("group raid\r\n");
	ch->Send("group leader <player>\r\n");
	ch->Send("group move <player> <slot #>\r\n");
}

bool cmd_groupQuery(Player *ch, std::string argument)
{
	Player * vch = (Player *)ch->GetQueryDataPtr(cmd_groupQuery);

	if (!Utilities::str_cmp(argument, "accept"))
	{
		vch->RemoveSubscriber(ch);
		ch->QueryClear(cmd_groupQuery);

		if (ch->HasGroup())
		{
			ch->Send("You already have your own group.\r\n");
			return true;
		}

		if (!vch->HasGroup()) //start a new group
		{
			vch->group = new Group(vch);
			vch->group->Add(ch);
			vch->Send(ch->GetName() + " has joined your group.\r\n");
			ch->Send("You have joined " + vch->GetName() + "'s group.\r\n");
		}
		else //joining an existing group
		{
			int slot = vch->group->FindNextEmptySlot();
			if ((!vch->group->IsRaidGroup() && slot >= Group::MAX_GROUP_SIZE) || slot == -1)
			{
				ch->Send("That group is full.\r\n");
			}
			else
			{
				if (vch->group->Add(ch))
				{
					//vch->Send(ch->name + " has joined your group.\r\n");
					ch->Send("You have joined " + vch->GetName() + "'s group.\r\n");
					ch->Message(ch->GetName() + " has joined the group.", Character::MSG_GROUP_NOTCHAR);
				}
			}
		}
		return true;
	}
	else if (!Utilities::str_cmp(argument, "decline"))
	{
		vch->Send(ch->GetName() + " declines your group invitation.\r\n");
		ch->QueryClear(cmd_groupQuery);
		vch->RemoveSubscriber(ch);
		return true;
	}
	return false;
}

void cmd_class(Player * ch, std::string argument)
{
    std::string arg1;
    std::string arg2;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    Class * currentClass = ch->currentClass;

    if(!Utilities::str_cmp(arg1, "info"))
    {
        ch->Send("Current class: [" + currentClass->name + " " 
                   + Utilities::itos(ch->GetClassLevel(currentClass->GetID())) + "]\r\n");
        ch->Send("Class history: ");
        bool found = false;
        std::list<Player::ClassData>::iterator iter;
        for(iter = ch->classList.begin(); iter != ch->classList.end(); ++iter)
        {
            Class * c = Game::GetGame()->GetClass((*iter).id);
            ch->Send("[" + c->name + " " + Utilities::itos((*iter).level) + "] ");
            found = true;
        }
        if(found)
        {
            ch->Send("\r\n");
        }
        else
        {
            ch->Send("None\r\n");
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
			ch->Send("\r\n");
            return;
        }
        Class * newclass = Game::GetGame()->GetClassByName(arg2);
        if(newclass == nullptr)
        {
			ch->Send("Valid classes are: ");
			std::map<int, Class *>::iterator iter;
			for (iter = Game::GetGame()->classes.begin(); iter != Game::GetGame()->classes.end(); ++iter)
			{
				ch->Send(" " + (*iter).second->name);
			}
			ch->Send("\r\n");
            return;
        }
        ch->currentClass = newclass;
        ch->Send("Class changed to " + newclass->name + ".\r\n");
        return;
    }
    else
    {
        ch->Send("Usage: class info\r\n");
        ch->Send("       class newclass <classname>\r\n");
        return;
    }
}

void cmd_levels(Player * ch, std::string argument)
{
    int prev = 0, next = 0;
    for(int i = 2; i <= Game::MAX_LEVEL; i++)
    {
        next = Game::ExperienceForLevel(i);
        std::stringstream sstr;
        sstr << "|B[|XLevel: " << std::setw(3) << i;
        sstr << "|B]|X Experience Required: " << std::setw(9) << next << "  Difference: " << next - prev << "\r\n";
        ch->Send(sstr.str());
        prev = next;
    }
}

void cmd_help(Player * ch, std::string argument)
{
	std::string arg1;
    argument = Utilities::one_argument(argument, arg1);

	if(arg1.empty())
	{
		ch->Send("HELP is available on the following topics.\r\nType 'help <topic>' for more information.\r\nType 'commands' for a full list of commands.\r\n");
		ch->Send("MOVEMENT    QUESTS    COMBAT    COMMUNICATION    SPELLS\r\n");
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
				ch->Send("|M[|X" + Utilities::itos(found[i]->id) + "|M]|X " + Utilities::ToUpper(found[i]->title) + "\r\n");
			}
		}
		else
		{
			ch->Send("No matching help items found.\r\n");
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

void cmd_alias(Player * ch, std::string argument)
{
    if (ch->IsNPC())
        return;

    std::string new_alias;
    argument = Utilities::one_argument(argument, new_alias);

    if (new_alias.empty())
    {
        if (ch->alias.empty())
        {
            ch->Send("You have no aliases.\r\n");
            ch->Send("Use: alias <word>\r\n     alias <word> <substitution>\r\n");
        }
        else
        {
            ch->Send("Existing aliases:\r\n");
            for (auto iter = ch->alias.begin(); iter != ch->alias.end(); iter++)
            {
                ch->Send("'" + iter->first + "' : '" + iter->second + "'\r\n");
            }
        }
        return;
    }

    if (!Utilities::str_cmp(new_alias, "alias") || !Utilities::str_cmp(new_alias, "unalias"))
    {
        ch->Send("That word is reserved.\r\n");
        return;
    }

    if (ch->alias.size() > Player::ALIAS_MAX)
    {
        ch->Send("Maximum number of aliases reached.\r\n");
        return;
    }

    auto alias_iter = ch->alias.find(new_alias);
    if (alias_iter != ch->alias.end())
    {
        if (!argument.empty())
        {
            ch->Send("That alias already exists.\r\n");
        }
        ch->Send(new_alias + ": " + alias_iter->second + "\r\n");
        return;
    }
    if (argument.empty())
    {
        ch->Send("Alias '" + new_alias + "' to what?\r\n");
        return;
    }
    if (argument.length() > MAX_COMMAND_LENGTH)
    {
        ch->Send("That alias substitution is too long. Maximum length is " + Utilities::itos(MAX_COMMAND_LENGTH) + " characters.\r\n");
        return;
    }
    if (new_alias.length() > 25)
    {
        ch->Send("That alias keyword is too long. Maximum length is 25 characters.\r\n");
        return;
    }
    ch->alias[new_alias] = argument;
    ch->Send("Alias created: '" + new_alias + "': " + argument + "\r\n");
}

void cmd_unalias(Player * ch, std::string argument)
{
    if (ch->IsNPC())
        return;

    if (ch->alias.empty())
    {
        ch->Send("You have no aliases.\r\n");
        ch->Send("Use: unalias <word>\r\n");
        return;
    }

    std::string remove_alias;
    argument = Utilities::one_argument(argument, remove_alias);

    if (remove_alias.empty())
    {
        ch->Send("Use: unalias <word>\r\n");
        ch->Send("Existing aliases:\r\n");
        for (auto iter = ch->alias.begin(); iter != ch->alias.end(); iter++)
        {
            ch->Send("'" + iter->first + "' : '" + iter->second + "'\r\n");
        }
        return;
    }

    auto iter = ch->alias.find(remove_alias);
    if (iter == ch->alias.end())
    {
        ch->Send("You don't have an alias with that name.\r\n");
        return;
    }

    ch->alias.erase(remove_alias);
    ch->Send("Alias '" + remove_alias + "' deleted.\r\n");
}

void cmd_quest(Player * ch, std::string argument)
{
    if(argument.empty())
    {
        ch->Send("Quest: log, list, info #, accept #, drop #, complete #, progress #, share #\r\n");
        return;
    }
    //log, list, info #, accept #, drop #, complete #, progress #, share #
    std::string arg1;
    std::string arg2;
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    if(!Utilities::str_cmp(arg1, "log"))
    {
        ch->Send("Active quests:\r\n");
        if(ch->questLog.empty())
        {
            ch->Send("None.\r\n");
            return;
        }
        std::vector<Quest *>::iterator iter;
        int ctr = 1;
        for(iter = ch->questLog.begin(); iter != ch->questLog.end(); ++iter)
        {
            Quest * q = (*iter);
            std::string complete = "";
            if(ch->QuestObjectivesComplete(q))
            {
                complete = " |G(Complete)";
            }
            
			//print current objective progress
			std::string objectives = "";
			std::vector<Quest::QuestObjective>::iterator objiter;
			int i = 0;
			for (objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
			{
				int currentCount = ch->questObjectives[ctr - 1][i++];
				//type count objective description
				if (currentCount < (*objiter).count)
				{
					objectives += "|y" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")";
				}
				else
				{
					objectives += "|g" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")";
				}
			}
			ch->Send(Utilities::itos(ctr) + ". " + Quest::GetDifficultyColor(ch->GetLevel(), q->level)
				+ q->name + " " + complete + " |X[" + objectives + "|X]\r\n");
			++ctr;
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "list"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to list quests.\r\n");
            return;
        }
		if (!ch->GetTarget()->IsNPC())
		{
			ch->Send("Your target has no quests to offer.\r\n");
			return;
		}
		NPCIndex * quest_target = ch->GetTarget()->GetNPCIndex();
        bool found = false;
        for(auto questiter = quest_target->questEnd.begin(); questiter != quest_target->questEnd.end(); ++questiter)
        {
            if(ch->QuestActive(*questiter))
            {
				if (!found && !quest_target->speechText.empty())
					ch->Send(quest_target->speechText + "\r\n");
                //TODO color ? by complete status
                found = true;
                ch->Send("|Y? " + Quest::GetDifficultyColor(ch->GetLevel(), (*questiter)->level) + (*questiter)->name + " (" + Utilities::itos((*questiter)->level) + ")|X");
				if (!(*questiter)->progressMessage.empty())
				{
					ch->Send(" : " + (*questiter)->progressMessage + "\r\n");
				}
				else
				{
					ch->Send("\r\n");
				}
            }
        }
        int ctr = 1;
        for(auto questiter = quest_target->questStart.begin(); questiter != quest_target->questStart.end(); ++questiter)
        {
            if(ch->QuestEligible(*questiter))
            {
				if (!found && !quest_target->speechText.empty())
					ch->Send(quest_target->speechText + "\r\n");
                found = true;
                ch->Send(Utilities::itos(ctr) + ". |Y! " + Quest::GetDifficultyColor(ch->GetLevel(), (*questiter)->level) 
                           + (*questiter)->name + " (" + Utilities::itos((*questiter)->level) + ")|X\r\n");
                ctr++;
            }
        }
        if(!found)
        {
            ch->Send("Your target has no quests to offer.\r\n");
            return;
        }
        return;
    }
    else if(!Utilities::str_cmp(arg1, "info"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to get quest information.\r\n");
            return;
        }
		if (!ch->GetTarget()->IsNPC())
		{
			ch->Send("Your target has no quests to offer.\r\n");
			return;
		}
		NPCIndex * quest_target = ch->GetTarget()->GetNPCIndex();
        if(quest_target->questStart.empty())
        {
            ch->Send("Your target has no quests to offer.\r\n");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Get information for which quest?\r\n");
            return;
        }

        std::vector<Quest *>::iterator questiter;
        int ctr = 1;
        for(questiter = quest_target->questStart.begin(); questiter != quest_target->questStart.end(); ++questiter)
        {
            if(ch->QuestEligible(*questiter))
            {
                if(qnum == ctr)
                {
                    Quest * q = (*questiter);
                    ch->Send(Quest::GetDifficultyColor(ch->GetLevel(), q->level) + q->name + " (" 
                               + Utilities::itos(q->level) + ")|X\r\n\r\n");
                    ch->Send(q->longDescription + "\r\n\r\n");
                    ch->Send(q->shortDescription + "\r\n\r\n");
					ch->Send("You will receive |Y" + Utilities::itos(q->experienceReward) + " experience|X");
					if (q->moneyReward > 0)
						ch->Send(" and |Y" + Utilities::itos(q->moneyReward) + " gold|X");
					ch->Send("\r\n");

					if (!q->itemRewards.empty())
					{
						ch->Send("|YYou will be able to choose one of these rewards:|X\r\n");
						std::string combinedRewards;
						for (auto itemiter = std::begin(q->itemRewards); itemiter != std::end(q->itemRewards); ++itemiter)
						{
							std::string itemreward = Game::GetGame()->GetItem(*itemiter)->FormatItemInfo(ch);
							combinedRewards = Utilities::SideBySideString(combinedRewards, itemreward);
						}
						ch->Send(combinedRewards);
					}
                    return;
                }
                ctr++;
            }
        }
        ch->Send("Quest " + arg2 + " not found.\r\n");
        return;
    }
    else if(!Utilities::str_cmp(arg1, "accept"))
    {
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to accept a quest.\r\n");
            return;
        }
		if (!ch->GetTarget()->IsNPC())
		{
			ch->Send("Your target has no quests to offer.\r\n");
			return;
		}
		NPCIndex * quest_target = ch->GetTarget()->GetNPCIndex();
        if(quest_target->questStart.empty())
        {
            ch->Send("Your target has no quests to offer.\r\n");
            return;
        }
        if(ch->questLog.size() >= Player::QUESTLOG_MAX_SIZE)
        {
            ch->Send("Your quest log is full.\r\n");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Accept which quest?\r\n");
            return;
        }
        std::vector<Quest *>::iterator questiter;
        int ctr = 1;
        //Need to loop on QuestEligible quests only
        for(questiter = quest_target->questStart.begin(); questiter != quest_target->questStart.end(); ++questiter)
        {
            if(ch->QuestEligible(*questiter))
            {
                if(qnum == ctr)
                {
                    //accept this quest
                    Quest * q = (*questiter);
                    std::vector<int> playerObjectives;
                    playerObjectives.resize(q->objectives.size(), 0);
                    ch->questLog.push_back(q);
                    ch->questObjectives.push_back(playerObjectives);
                    
                    ch->Send("Accepted quest: " + q->name + "\r\n");

                    //TODO: check for already completed OBJECTIVE_ITEM
                    int objindex = 0;
                    std::vector<Quest::QuestObjective>::iterator iter;
                    for(iter = q->objectives.begin(); iter != q->objectives.end(); ++iter)
                    {
                        if((*iter).type == Quest::OBJECTIVE_ITEM)
                        {
                            for(auto inviter = ch->inventory.begin(); inviter != ch->inventory.end(); ++inviter)
                            {
                                if(inviter->first->GetID() == ((Item*)((*iter).objective))->GetID())
                                {
                                    ch->questObjectives[ch->questLog.size()-1][objindex]++;
                                }
                            }
                            if(ch->questObjectives[ch->questLog.size()-1][objindex] > 0)
                            {
                                ch->Send("|W" + q->name + ": ");
                                ch->Send("|Y" + (*iter).description + " (" 
                                    + Utilities::itos(ch->questObjectives[ch->questLog.size()-1][objindex]) 
                                    + "/" + Utilities::itos((*iter).count) + ")|X\r\n\r\n");
                            }
                        }
                        objindex++;
                    }
                    return;
                }
                ctr++;
            }
        }
        ch->Send("Quest " + arg2 + " not found.\r\n");

        return;
    }
    else if(!Utilities::str_cmp(arg1, "drop"))
    {
        if(ch->questLog.empty())
        {
            ch->Send("Your quest log is empty.\r\n");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Drop which quest?\r\n");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->questLog.size()) + "\r\n");
            return;
        }
        ch->Send("You have dropped " + ch->questLog[qnum-1]->name + ".\r\n");
        ch->questLog.erase(ch->questLog.begin() + qnum - 1);
        ch->questObjectives.erase(ch->questObjectives.begin() + qnum - 1);
        return;
    }
    else if(!Utilities::str_cmp(arg1, "complete"))
    {
        if(ch->questLog.empty())
        {
            ch->Send("Your quest log is empty.\r\n");
            return;
        }
        if(!ch->GetTarget())
        {
            ch->Send("You need a target to complete a quest.\r\n");
            return;
        }
		if (!ch->GetTarget()->IsNPC())
		{
			ch->Send("Your target cannot complete your quest.\r\n");
			return;
		}
		NPCIndex * quest_target = ch->GetTarget()->GetNPCIndex();
        if(quest_target->questEnd.empty())
        {
            ch->Send("Your target cannot complete your quest.\r\n");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Complete which quest?\r\n");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->questLog.size()) + "\r\n");
            return;
        }
        Quest * complete = ch->questLog[qnum-1];
        std::vector<Quest*>::iterator qiter;
        bool found = false;
        for(qiter = quest_target->questEnd.begin(); qiter != quest_target->questEnd.end(); ++qiter)
        {
            if((*qiter)->id == complete->id)
            {
                found = true;
                break;
            }
        }
        if(!found)
        {
            ch->Send("Your target cannot complete your quest.\r\n");
            return;
        }
        if(!ch->QuestObjectivesComplete(complete))
        {
            ch->Send("Not all objectives of that quest are complete.\r\n");
            return;
        }


		if (!complete->itemRewards.empty())
		{
			std::string rewardQueryString = "Choose your reward: (";
			std::string combinedRewards;
			int ctr = 1;
			for (auto itemiter = std::begin(complete->itemRewards); itemiter != std::end(complete->itemRewards); ++itemiter)
			{
				rewardQueryString += Utilities::itos(ctr++);
				if (ctr <= complete->itemRewards.size())
					rewardQueryString += ", ";
				std::string itemreward = Game::GetGame()->GetItem(*itemiter)->FormatItemInfo(ch);
				combinedRewards = Utilities::SideBySideString(combinedRewards, itemreward);
			}
			rewardQueryString += "): ";
			ch->Send(combinedRewards);
			ch->AddQuery(rewardQueryString, qnum, questCompleteQuery);
		}
		else
		{
			//Remove items from inventory for OBJECTIVE_ITEM
			std::vector<Quest::QuestObjective>::iterator iter;
			for (iter = complete->objectives.begin(); iter != complete->objectives.end(); ++iter)
			{
				if ((*iter).type == Quest::OBJECTIVE_ITEM)
				{
					int found = 0;
					while (ch->RemoveItemInventory((Item*)((*iter).objective)))
					{
						found++;
						if (found >= (*iter).count)
							break;
					}
				}
			}

			qiter = ch->questLog.begin() + qnum - 1;
			std::vector< std::vector<int> >::iterator objiter = ch->questObjectives.begin() + qnum - 1;
			ch->questLog.erase(qiter);
			ch->questObjectives.erase(objiter);
			ch->completedQuests.insert(complete->id);

			ch->Send(complete->name + " completed!\r\n");
			ch->Send(complete->completionMessage + "\r\n");
            if (complete->experienceReward > 0)
            {
                ch->Send("|BYou have gained |Y" + Utilities::itos(complete->experienceReward) + "|B experience.|X\r\n");
                ch->ApplyExperience(complete->experienceReward);
            }
		}
        return;
    }
    else if(!Utilities::str_cmp(arg1, "progress"))
    {
        if(ch->questLog.empty())
        {
            ch->Send("Your quest log is empty.\r\n");
            return;
        }
        int qnum = Utilities::atoi(arg2);
        if(qnum == 0)
        {
            ch->Send("Get progress information for which quest?\r\n");
            return;
        }
        if(qnum < 1 || qnum > (int)ch->questLog.size())
        {
            ch->Send("Choose a quest from your quest log, 1-" + Utilities::itos((int)ch->questLog.size()) + "\r\n");
            return;
        }
        Quest * progress = ch->questLog[qnum-1];
        ch->Send("Quest information for: " + Quest::GetDifficultyColor(ch->GetLevel(), progress->level) 
                   + progress->name + " (" + Utilities::itos(progress->level) + ")|X\r\n");
        ch->Send(progress->longDescription + "\r\n\r\n");
        ch->Send(progress->shortDescription + "\r\n\r\n");

        std::vector<Quest::QuestObjective>::iterator objiter;
        int i = 0;
        for(objiter = progress->objectives.begin(); objiter != progress->objectives.end(); ++objiter)
        {
            int currentCount = ch->questObjectives[qnum-1][i++];
            //type count objective description
            if(currentCount < (*objiter).count)
            {
                ch->Send("|Y" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")|X\r\n");
            }
            else
            {
                ch->Send("|G" + (*objiter).description + " (" + Utilities::itos(currentCount) + "/" + Utilities::itos((*objiter).count) + ")|X\r\n");
            }
        }

		ch->Send("You will receive |Y" + Utilities::itos(progress->experienceReward) + " experience|X");
		if (progress->moneyReward > 0)
			ch->Send(" and |Y" + Utilities::itos(progress->moneyReward) + " gold|X");
		ch->Send("\r\n");

		if (!progress->itemRewards.empty())
		{
			ch->Send("|YYou will be able to choose one of these rewards:|X\r\n");
			std::string combinedRewards;
			for (auto itemiter = std::begin(progress->itemRewards); itemiter != std::end(progress->itemRewards); ++itemiter)
			{
				std::string itemreward = Game::GetGame()->GetItem(*itemiter)->FormatItemInfo(ch);
				combinedRewards = Utilities::SideBySideString(combinedRewards, itemreward);
			}
			ch->Send(combinedRewards);
		}
        return;
    }
    else if(!Utilities::str_cmp(arg1, "share"))
    {
        return;
    }
    ch->Send("Quest: log, list, info #, accept #, drop #, complete #, progress #, share #\r\n");
}


bool questCompleteQuery(Player * ch, std::string argument)
{
	int qnum = ch->GetQueryDataInt(questCompleteQuery);
	Quest * quest = ch->questLog[qnum - 1];
	ch->QueryClear(questCompleteQuery);

	std::string arg1;
	Utilities::one_argument(argument, arg1);
	if (!Utilities::IsNumber(arg1))
	{
		ch->Send("Invalid choice, quest not completed.\r\n");
		return true;
	}
	int choice = Utilities::atoi(arg1);
	if (choice <= 0 || choice > quest->itemRewards.size())
	{
		ch->Send("Invalid choice, quest not completed.\r\n");
		return true;
	}

	//Remove items from inventory for OBJECTIVE_ITEM
	std::vector<Quest::QuestObjective>::iterator iter;
	for (iter = quest->objectives.begin(); iter != quest->objectives.end(); ++iter)
	{
		if ((*iter).type == Quest::OBJECTIVE_ITEM)
		{
			int found = 0;
			while (ch->RemoveItemInventory((Item*)((*iter).objective)))
			{
				found++;
				if (found >= (*iter).count)
					break;
			}
		}
	}

	std::vector<Quest*>::iterator qiter = ch->questLog.begin() + qnum - 1;
	std::vector< std::vector<int> >::iterator objiter = ch->questObjectives.begin() + qnum - 1;
	ch->questLog.erase(qiter);
	ch->questObjectives.erase(objiter);
	ch->completedQuests.insert(quest->id);

	ch->Send(quest->name + " completed!\r\n");
	ch->Send(quest->completionMessage + "\r\n");

	Item * myreward = Game::GetGame()->GetItem(quest->itemRewards[choice - 1]);
	if(myreward)
	{
		ch->Send("|WYou receive loot: " + myreward->GetColoredName() + "|X\r\n");
		ch->AddItemInventory(myreward);
	}

    if (quest->experienceReward > 0)
    {
        ch->Send("|BYou have gained |Y" + Utilities::itos(quest->experienceReward) + "|B experience.|X\r\n");
        ch->ApplyExperience(quest->experienceReward);
    }
	return true;
}

void cmd_quit(Player * ch, std::string argument)
{
    /*if(ch->HasQuery())
    {
        ch->Send("Answer your current question first.\r\n");
        return;
    }*/
    //Do a few checks here, but just query the player with a quit-function callback.
    if(ch->InCombat())
    {
        ch->Send("You can't do that while in combat!\r\n");
        return;
    }
    if(ch->HasActiveDelay())
    {
        ch->Send("You can't do that while casting.\r\n");
        return;
    }
	
	std::string qp = "Quit? (y/n) ";
	if (ch->GetLevel() == 1)
	{
		qp += "|R(Level one characters will not be saved)|X: ";
	}
	ch->AddQuery(qp, nullptr, cmd_quit_Query);
}

bool cmd_quit_Query(Player * ch, std::string argument)
{
    ch->QueryClear(cmd_quit_Query);
    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
	{
        if(ch && ch->InCombat())
        {
            ch->Send("You can't do that while in combat!\r\n");
            return true;
        }
        if(ch->HasActiveDelay())
        {
            ch->Send("You can't do that while casting.\r\n");
            return true;
        }

		ch->Send("Bye\r\n");
		LogFile::Log("status", ch->GetName() + " \"quit\"");

        if(ch->user)
        {
			ch->user->SetDisconnect();
        }
	}
	return true;
}

bool releaseSpiritQuery(Player * ch, std::string argument)
{
    if(!Utilities::str_cmp(argument, "release"))
    {
		ch->corpse_room = ch->room->id;
		ch->SetGhost();
        ch->QueryClear(releaseSpiritQuery);
        ch->QueryClear(acceptPlayerRes);
		Area * this_area = Game::GetGame()->GetArea(ch->room->area);
		if (!this_area)
		{
			LogFile::Log("error", "releaseSpiritQuery, bad ch->room->area");
			return true;
		}
		Room * graveyard = Game::GetGame()->GetRoom(this_area->death_room);
		if (!graveyard)
		{
			LogFile::Log("error", "releaseSpiritQuery, no area death_room");
			return true;
		}
		ch->Message("|W" + ch->GetName() + "'s spirit separates from " + ch->HisHer() + " corpse, and disappates into nothingness.|X", Character::MessageType::MSG_ROOM_NOTCHAR);
		ch->MakeCorpse();
		ch->ChangeRooms(graveyard);
		ch->graveyard_room = graveyard->id;
		ch->Look("");

		int res_at_graveyard = ch->death_timer - ch->TimeSinceDeath();
		int res_at_corpse = ch->death_timer_runback - ch->TimeSinceDeath();
		if (res_at_graveyard > 0)
			ch->Send("|W" + Utilities::itos(res_at_graveyard) + " seconds before you can resurrect at the graveyard.\r\n");
		if (res_at_corpse > 0)
			ch->Send("|W" + Utilities::itos(res_at_corpse) + " seconds before you can resurrect near your corpse.\r\n");
        return true;
    }
    return false;
}

bool acceptPlayerRes(Player * ch, std::string argument)
{
    if (!Utilities::str_cmp(argument, "res"))
    {
        Room * resroom = (Room*)ch->GetQueryDataPtr(acceptPlayerRes);
        if (resroom == nullptr)
        {
            LogFile::Log("error", "acceptResOrReleaseQuery, null res room");
            ch->QueryClear(acceptPlayerRes);
            return true;
        }
        ch->SetHealth(ch->GetMaxHealth() / 4);
        ch->SetMana(ch->GetMaxMana() / 4);
        ch->SetEnergy(0);
        ch->SetRage(0);

        ch->ChangeRooms(resroom);
        ch->RemoveCorpse();

        ch->Message("|W" + ch->GetName() + " appears in a shimmering silver mist.|X", Character::MessageType::MSG_ROOM_NOTCHAR);
        ch->SetAlive();
        ch->QueryClear(acceptPlayerRes);
        ch->QueryClear(acceptResQuery);
        ch->QueryClear(returnToGYQuery);
        ch->QueryClear(releaseSpiritQuery);
        ch->Look("");
        return true;
    }
    return false;
}

bool acceptResQuery(Player * ch, std::string argument)
{
	if (!Utilities::str_cmp(argument, "res"))
	{
		ch->SetHealth(ch->GetMaxHealth() / 2);
		ch->SetMana(ch->GetMaxMana() / 2);
		ch->SetEnergy(ch->GetMaxEnergy());
		ch->SetRage(0);

		ch->RemoveCorpse();
		
		ch->Message("|W" + ch->GetName() + " appears in a shimmering silver mist.|X", Character::MessageType::MSG_ROOM_NOTCHAR);
		ch->SetAlive();
        ch->QueryClear(acceptPlayerRes);
        ch->QueryClear(acceptResQuery);
        ch->QueryClear(returnToGYQuery);
        ch->QueryClear(releaseSpiritQuery);
        ch->Look("");
		return true;
	}
	return false;
}

bool returnToGYQuery(Player * ch, std::string argument)
{
	if (!Utilities::str_cmp(argument, "return"))
	{
        ch->QueryClear(acceptResQuery);
        ch->QueryClear(returnToGYQuery);
		ch->ChangeRooms(Game::GetGame()->GetRoom(ch->graveyard_room));
        ch->Look("");
		return true;
	}
	return false;
}
