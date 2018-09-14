#include "stdafx.h"
#include "CCharacter.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CRoom.h"
#include "CGroup.h"
#include "CNPC.h"
#include "CGame.h"
#include "CItem.h"
#include "CServer.h"
#include "CSkill.h"
#include "CClass.h"
#include "CSpellAffect.h"
#include "CPlayer.h"
#include "CNPCIndex.h"
#include "CTrigger.h"
#include "CQuest.h"
#include "CArea.h"
#include "utils.h"
#include "CLogFile.h"
#include "mud.h"
#include "json.hpp"
// for convenience
using json = nlohmann::json;
#include <string>

class Subscriber;

using std::string;

//TODO: racial skills?
Character::RaceType Character::race_table[] = 
{
    { Character::Races::RACE_HUMAN, "Human"	},
    { Character::Races::RACE_ELF, "Elf"		},
	{ Character::Races::RACE_DWARF, "Dwarf"	},
	{ Character::Races::RACE_ORC, "Orc"		},
	{ Character::Races::RACE_GNOME, "Gnome"	},
	{ Character::Races::RACE_GOBLIN, "Goblin"	},
	{ Character::Races::RACE_UNDEAD, "Undead"	},
	{ Character::Races::RACE_MINOTAUR, "Minotaur" },
	{ Character::Races::RACE_TROLL, "Troll"	},
	{-1, ""			}
};

Character::Character()
{
	room = nullptr;
	movementSpeed = Character::NORMAL_MOVE_SPEED;
	lastMoveTime = 0;
	meleeActive = false;
	delay = 0;
	delay_active = false;
	delayFunction = nullptr;
	lastSpellCast = 0;
	target = nullptr;
	health = mana = energy = rage = 1;
	combat = false;
	isCorpse = false;
	deathTime = 0;
	remove = false;
}

Character::~Character()
{
    RemoveAllSpellAffects();
}

void Character::SendTargetSubscriberGMCP(std::string str)
{
	for (auto iter = this->subscribers_.begin(); iter != this->subscribers_.end(); ++iter)
	{
		Character * subscriber_as_char;
		if (subscriber_as_char = dynamic_cast<Character*>(iter->subscriber))
		{
			if (subscriber_as_char->GetTarget() == this)
			{
				subscriber_as_char->SendGMCP(str);
			}
		}
	}
}

void Character::SendTargetTargetSubscriberGMCP(std::string str)
{
	for (auto iter1 = this->subscribers_.begin(); iter1 != this->subscribers_.end(); ++iter1)
	{
		Character * subscriber_as_char;
		if (subscriber_as_char = dynamic_cast<Character*>(iter1->subscriber))
		{
			if (subscriber_as_char->GetTarget() == this)
			{
				for (auto iter2 = subscriber_as_char->subscribers_.begin(); iter2 != subscriber_as_char->subscribers_.end(); ++iter2)
				{
					Character * subofsub_as_char;
					if (subofsub_as_char = dynamic_cast<Character*>(iter2->subscriber))
					{
						if (subofsub_as_char->GetTarget() == subscriber_as_char)
						{
							subofsub_as_char->SendGMCP(str);
						}
					}
				}
			}
		}
	}
}

void Character::Message(const string & txt, MessageType msg_type, Character * vict)
{
	switch(msg_type)
	{
		case MSG_CHAR:
        {
            Send(txt + "\n\r");
			//mob trigger on msg here
			break;
        }

        case MSG_CHAR_VICT:
        {
            Send(txt + "\n\r");
            vict->Send(txt + "\n\r");
			//mob trigger on msg here
            break;
        }

		case MSG_ROOM:
        {
			if(!room)
				return;
			std::list<Character *>::iterator iter;
			for(iter = room->characters.begin(); iter != room->characters.end(); iter++)
			{
				if(!(*iter)->IsGhost())
					(*iter)->Send(txt + "\n\r");
			    //mob trigger on msg here
			}
			break;
        }

        case MSG_ROOM_NOTCHAR:
        {
            if(!room)
				return;
			std::list<Character *>::iterator iter;
			for(iter = room->characters.begin(); iter != room->characters.end(); iter++)
			{
                if((*iter) != this && !(*iter)->IsGhost())
                    (*iter)->Send(txt + "\n\r");
			    //mob trigger on msg here
			}
            break;
        }

        case MSG_ROOM_NOTVICT:
        {
            if(!room)
				return;
			std::list<Character *>::iterator iter;
			for(iter = room->characters.begin(); iter != room->characters.end(); iter++)
			{
                if((*iter) != vict && !(*iter)->IsGhost())
                    (*iter)->Send(txt + "\n\r");
			    //mob trigger on msg here
			}
            break;
        }

        case MSG_ROOM_NOTCHARVICT:
        {
            if(!room)
				return;
			std::list<Character *>::iterator iter;
			for(iter = room->characters.begin(); iter != room->characters.end(); iter++)
			{
                if((*iter) != vict && (*iter) != this && !(*iter)->IsGhost())
                    (*iter)->Send(txt + "\n\r");
			    //mob trigger on msg here
			}
            break;
        }

		case MSG_GROUP:
		{
			if (!HasGroup())
				return;
			for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
			{
				if (GetGroup()->members[i] != nullptr)
				{
					GetGroup()->members[i]->Send(txt + "\n\r");
				}
			}
			break;
		}

		case MSG_GROUP_NOTCHAR:
		{
			if (!HasGroup())
				return;
			for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
			{
				if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
				{
					GetGroup()->members[i]->Send(txt + "\n\r");
				}
			}
			break;
		}

		//case MSG_AREA: all characters in area (including mobs)?
		//	break;

		default:
        {
            LogFile::Log("error", "Character::Message(), bad msg_type: " + msg_type);
			break;
        }
	}
}

Item * Character::GetItemRoom(string name)
{
	if (name.empty() || GetRoom() == nullptr)
		return nullptr;

	return GetRoom()->GetItem(name);
}

bool Character::IsItemInRoom(Item * i)
{
	if (i == nullptr)
		return false;
	if (GetRoom())
		return GetRoom()->HasItem(i);
	return false;
}

Character * Character::GetCharacterRoom(string name)
{
	if (name.empty() || GetRoom() == nullptr)
		return nullptr;

	if (!Utilities::str_cmp(name, "self") || !Utilities::str_cmp(name, "me"))
		return this;

	return GetRoom()->GetCharacter(name);
}

Character * Character::GetCharacterRoom(Character * target)
{
	if (!target)
		return nullptr;
	if (target == this)
		return target;

	return GetRoom()->GetCharacter(target);
}

//Find a character in this room or any adjacent room. takes an optional direction argument
//todo: most of this should be a CRoom function
Character * Character::GetCharacterAdjacentRoom(string name, string direction)
{
    if(name.empty())
        return nullptr;

    int count = 0;
	string tempname = name;
    int number = Utilities::number_argument(tempname);

    if(!Utilities::str_cmp(tempname, "self") || !Utilities::str_cmp(tempname, "me"))
		return this;

	int exitNum = -1;
	if (!direction.empty())
	{
		for (int i = 0; i < Exit::DIR_LAST; ++i)
		{
			if (!Utilities::str_cmp(direction, Exit::exitNames[i]))
			{
				exitNum = i;
				break;
			}
		}
	}
	std::list<Character *>::iterator iter;
	if (exitNum != -1)
	{
		if (room->exits[exitNum] && room->exits[exitNum]->to)
		{
			for (iter = room->exits[exitNum]->to->characters.begin(); iter != room->exits[exitNum]->to->characters.end(); ++iter)
			{
				if (!Utilities::IsName(tempname, (*iter)->GetName()))
					continue;
				if (++count == number)
					return (*iter);
			}
		}
		return nullptr;
	}

    for(iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if(!Utilities::IsName(tempname, (*iter)->GetName()))
	        continue;
	    if(++count == number)
	        return (*iter);
    }
    for(int i = 0; i < Exit::DIR_LAST; i++)
    {
        if(room->exits[i] && room->exits[i]->to)
        {
            for(iter = room->exits[i]->to->characters.begin(); iter != room->exits[i]->to->characters.end(); ++iter)
            {
                if(!Utilities::IsName(tempname, (*iter)->GetName()))
		            continue;
		        if(++count == number)
		            return (*iter);
            }
        }
    }
    return nullptr;
}



//Find a character in this room or any adjacent room
//todo: most of this should be a CRoom function
Character * Character::GetCharacterAdjacentRoom(Character * target)
{
    if(!target)
        return nullptr;
    if(target == this)
        return target;

    std::list<Character *>::iterator iter;
    for(iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if((*iter) == target)
            return (*iter);
    }
    for(int i = 0; i < Exit::DIR_LAST; i++)
    {
        if(room->exits[i] && room->exits[i]->to)
        {
            for(iter = room->exits[i]->to->characters.begin(); iter != room->exits[i]->to->characters.end(); ++iter)
            {
                if((*iter) == target)
                    return (*iter);
            }
        }
    }
    return nullptr;
}

void Character::Move(int direction)
{
    if(GetRoom() == nullptr)
    {
        LogFile::Log("error", "Character::Move, room == nullptr");
		return;
    }

    if(direction < 0 || direction >= Exit::DIR_LAST)
	{
		LogFile::Log("error", "Character::Move, bad direction");
		return;
	}
    
	Stand();

    if(delay_active)
    {
		CancelActiveDelay();
        Send("Action interrupted!\n\r");
    }
    
	Room * toroom;

    //Regular rooms
	if(room->exits[direction] == nullptr || room->exits[direction]->to == nullptr)
	{
		Send("You cannot move in that direction.\n\r");
		return;
	}

	if (IsAlive())
	{
		Message(GetName() + " leaves " + Exit::exitNames[direction] + ".", MSG_ROOM_NOTCHAR);
	}
	toroom = room->exits[direction]->to;

    ChangeRooms(toroom);

    //TODO: move the checks for movement triggers here
	if (IsAlive())
	{
		Message(GetName() + " has arrived from " + ((direction != Exit::DIR_UP && direction != Exit::DIR_DOWN) ? "the " : "") + Exit::reverseExitNames[direction] + ".", MSG_ROOM_NOTCHAR);
	}

	if(IsPlayer())					 //ew
		cmd_look((Player*)this, ""); //eww

    //check npc aggro
	if (IsAlive())
	{
		for (std::list<Character*>::iterator iter = room->characters.begin(); iter != room->characters.end(); ++iter)
		{
			if ((*iter)->IsNPC() && (*iter)->IsAlive() && (*iter)->FlagIsSet(NPCIndex::FLAG_AGGRESSIVE) && !(*iter)->InCombat() && !IsImmortal())
			{
				(*iter)->EnterCombat(this);
				EnterCombat(*iter);
				Send((*iter)->GetName() + " begins attacking you!\n\r");
			}
		}
	}
}

SpellAffect * Character::AddSpellAffect(int isDebuff, Character * caster, string name,
                               bool hidden, bool stackable, int ticks, double duration, int category, Skill * sk, string affect_description)
{
    if(!stackable) //if name and skill are the same as an existing affect, dont add it
    {
        std::list<SpellAffect*>::iterator iter, end;
        if(isDebuff)
        {
            iter = debuffs.begin(); end = debuffs.end();
        }
        else
        {
            iter = buffs.begin(); end = buffs.end();
        }
        for(; iter != end; ++iter)
        {
			if (!Utilities::str_cmp((*iter)->name, name) && (*iter)->skill == sk) 
			{
				//Found it, refresh the duration
				(*iter)->ticksRemaining = (*iter)->ticks;
				(*iter)->appliedTime = Game::currentTime;
				return nullptr;
			}
		}
    }

    SpellAffect * sa = new SpellAffect();
    sa->name = name;
    sa->hidden = hidden;
    sa->stackable = stackable;
    sa->ticks = ticks;
    sa->duration = duration;
	sa->affectDescription = affect_description;
    sa->skill = sk;
    sa->debuff = isDebuff;
    sa->appliedTime = Game::currentTime;
    sa->caster = caster;
    sa->affectCategory = category;
    if(sa->caster)
    {
        sa->caster->AddSubscriber(sa);
		//cout << "AddSpellAffect ADD" << endl;
        sa->casterName = caster->GetName();
    }
    sa->ticksRemaining = ticks;

    if(isDebuff)
    {
        sa->id = (int)debuffs.size() + 1;
        debuffs.push_front(sa);
    }
    else
    {
        sa->id = (int)buffs.size() + 1;
        buffs.push_front(sa);
    }

    if(sk != nullptr)
    {
        string func = sk->function_name + "_apply";
        try
        {
			sol::function lua_apply_func = Server::lua[func.c_str()];
			sol::protected_function_result result = lua_apply_func(caster, this, sa);
			if (!result.valid())
			{
				// Call failed
				sol::error err = result;
				std::string what = err.what();
				LogFile::Log("error", "_apply call failed, sol::error::what() is: " + what);
			}
        }
		catch (const std::exception & e)
		{
			LogFile::Log("error", e.what());
		}
    }
    return sa;
}

SpellAffect * Character::HasSpellAffect(string name)
{
    std::list<SpellAffect*>::iterator iter;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        if((*iter)->name == name)
        {
            return (*iter);
        }
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        if((*iter)->name == name)
        {
            return (*iter);
        }
    }
	return nullptr;
}

SpellAffect * Character::GetFirstSpellAffectWithAura(int aura_id)
{
	for (auto iter = buffs.begin(); iter != buffs.end(); ++iter)
	{
		if ((*iter)->HasAura(aura_id))
		{
			return (*iter);
		}
	}
	for (auto iter = debuffs.begin(); iter != debuffs.end(); ++iter)
	{
		if ((*iter)->HasAura(aura_id))
		{
			return (*iter);
		}
	}
	return nullptr;
}

int Character::CleanseSpellAffect(Character * cleanser, int category, int howMany)
{
    if(category < 0 || category >= SpellAffect::AFFECT_LAST)
    {
        LogFile::Log("error", "CleanseSpellAffect: bad category");
        return 0;
    }
    int removed_count = 0;
    std::list<SpellAffect*>::iterator iter = debuffs.begin();
    while(iter != debuffs.end())
    {
        std::list<SpellAffect*>::iterator thisiter = iter;
        iter++;
        if((*thisiter)->affectCategory == category)
        {
            Send("Your '" + (*thisiter)->name + "' has been removed.\n\r");
            if(cleanser != this)
            {
                cleanser->Send("Removed '" + (*thisiter)->name + "'.\n\r");
            }
            removed_count++;
            (*thisiter)->auraAffects.clear();
            delete (*thisiter);
            debuffs.erase(thisiter);
            if(removed_count >= howMany)
                break;
        }
    }
    return removed_count;
}

bool Character::RemoveSpellAffectsByAura(int isDebuff, int auraid)
{
	std::list<SpellAffect*>::iterator iter;
	bool removed = false;
	if (isDebuff)
	{
		iter = debuffs.begin();
		while ( iter != debuffs.end())
		{
			std::list<SpellAffect::AuraAffect>::iterator findme;
			findme = std::find_if((*iter)->auraAffects.begin(), (*iter)->auraAffects.end(), SpellAffect::CompareAuraByID(auraid));
			if (findme != (*iter)->auraAffects.end())
			{
				removed = true;
				(*iter)->auraAffects.clear();
				delete (*iter);
				iter = debuffs.erase(iter);
				break;
			}
			else
			{
				iter++;
			}
		}
	}
	else
	{
		iter = buffs.begin();
		while (iter != buffs.end())
		{
			std::list<SpellAffect::AuraAffect>::iterator findme;
			findme = std::find_if((*iter)->auraAffects.begin(), (*iter)->auraAffects.end(), SpellAffect::CompareAuraByID(auraid));
			if (findme != (*iter)->auraAffects.end())
			{
				removed = true;
				(*iter)->auraAffects.clear();
				delete (*iter);
				iter = buffs.erase(iter);
				break;
			}
			else
			{
				iter++;
			}
		}
	}
	return removed;
}

void Character::RemoveSpellAffect(int isDebuff, int id)
{
    std::list<SpellAffect*>::iterator iter;
    if(isDebuff)
    {
        for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
        {
            if((*iter)->id == id)
            {
                (*iter)->auraAffects.clear();
                delete (*iter);
                debuffs.erase(iter);
                break;
            }
        }
    }
    else
    {
        for(iter = buffs.begin(); iter != buffs.end(); ++iter)
        {
            if((*iter)->id == id)
            {
                (*iter)->auraAffects.clear();
                delete (*iter);
                buffs.erase(iter);
                break;
            }
        }
    }
}

void Character::RemoveSpellAffect(int isDebuff, string name)
{
    std::list<SpellAffect*>::iterator iter;
    if(isDebuff)
    {
        for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
        {
            if((*iter)->name == name)
            {
                (*iter)->auraAffects.clear();
                delete (*iter);
                debuffs.erase(iter);
                break;
            }
        }
    }
    else
    {
        for(iter = buffs.begin(); iter != buffs.end(); ++iter)
        {
            if((*iter)->name == name)
            {
                (*iter)->auraAffects.clear();
                delete (*iter);
                buffs.erase(iter);
                break;
            }
        }
    }
}

void Character::RemoveAllSpellAffects()
{
    while(!debuffs.empty())
    {
        delete debuffs.front();
        debuffs.pop_front();
    }
    debuffs.clear();
    while(!buffs.empty())
    {
        delete buffs.front();
        buffs.pop_front();
    }
    buffs.clear();
}



int Character::GetAuraModifier(int aura_id, int whatModifier)
{
    //whatModifier: 1 total, 2 largest, 3 smallest, 4 largest positive, 5 smallest negative
    int result = 0;

    std::list<SpellAffect*>::iterator iter;
    std::list<struct SpellAffect::AuraAffect>::iterator iter2;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id)
                {
                    switch(whatModifier)
                    {
                        case 1:
                            result += (*iter2).modifier;
                            break;
                        case 2:
                            if(result < (*iter2).modifier || result == 0)
                                result = (*iter2).modifier;
                            break;
                        case 3:
                            if(result > (*iter2).modifier || result == 0)
                                result = (*iter2).modifier;
                            break;
                        case 4:
                            if(result < (*iter2).modifier)
                                result = (*iter2).modifier;
                            break;
                        case 5:
                            if(result > (*iter2).modifier)
                                result = (*iter2).modifier;
                            break;
                    }
                }
            }
        }
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id)
                {
                    switch(whatModifier)
                    {
                        case 1:
                            result += (*iter2).modifier;
                            break;
                        case 2:
                            if(result < (*iter2).modifier || result == 0)
                                result = (*iter2).modifier;
                            break;
                        case 3:
                            if(result > (*iter2).modifier || result == 0)
                                result = (*iter2).modifier;
                            break;
                        case 4:
                            if(result < (*iter2).modifier)
                                result = (*iter2).modifier;
                            break;
                        case 5:
                            if(result > (*iter2).modifier)
                                result = (*iter2).modifier;
                            break;
                    }

                }
            }
        }
    }
    return result;
}

int Character::GetTotalAuraModifier(int aura_id)
{
    int total = 0;

    std::list<SpellAffect*>::iterator iter;
    std::list<struct SpellAffect::AuraAffect>::iterator iter2;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id)
                    total += (*iter2).modifier;
            }
        }
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id)
                    total += (*iter2).modifier;
            }
        }
    }
    return total;
}

int Character::GetLargestAuraModifier(int aura_id)
{
    //TODO: largest will return the largest negative modifier, if there's no positive
    // change it to getlargestpositiveauramodifier, getsmallestnegativeauramodifier?
    int largest = 0;

    std::list<SpellAffect*>::iterator iter;
    std::list<struct SpellAffect::AuraAffect>::iterator iter2;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id && (largest < (*iter2).modifier || largest == 0))
                    largest = (*iter2).modifier;
            }
        }
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id && (largest < (*iter2).modifier || largest == 0))
                    largest = (*iter2).modifier;
            }
        }
    }
    return largest;
}

int Character::GetSmallestAuraModifier(int aura_id)
{
    int smallest = 0;

    std::list<SpellAffect*>::iterator iter;
    std::list<struct SpellAffect::AuraAffect>::iterator iter2;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id && (smallest > (*iter2).modifier || smallest == 0))
                    smallest = (*iter2).modifier;
            }
        }
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        if(!(*iter)->auraAffects.empty())
        {
            for(iter2 = (*iter)->auraAffects.begin(); iter2 != (*iter)->auraAffects.end(); ++iter2)
            {
                if((*iter2).auraID == aura_id && (smallest < (*iter2).modifier || smallest == 0))
                    smallest = (*iter2).modifier;
            }
        }
    }
    return smallest;
}

void Character::EnterCombat(Character * victim)
{
	if (!IsAlive() || !victim->IsAlive())
	{
		LogFile::Log("error", "EnterCombat called on !IsAlive ch or vict");
		return;
	}

    if(target == nullptr || target == victim)
    {
        SetTarget(victim);
        meleeActive = true;
    }
    //if(victim->target == nullptr || victim->target == this)
    if((victim->IsNPC() && victim->meleeActive == false) || victim->target == nullptr || victim->target == this)
    {
        victim->SetTarget(this);
        victim->meleeActive = true;
    }
    combat = true;
    victim->combat = true;

    if(IsPlayer())
        ((Player*)(this))->lastCombatAction = Game::currentTime;
    if(victim->IsPlayer())
		((Player*)(victim))->lastCombatAction = Game::currentTime;

	if (IsNPC() || victim->IsNPC()) //Keep track of threat unless BOTH are players
	{
        victim->UpdateThreat(this, 0, Character::Threat::THREAT_DAMAGE); 
		UpdateThreat(victim, 0, Character::Threat::THREAT_DAMAGE);
    }

    movementSpeed = Character::COMBAT_MOVE_SPEED;
    victim->movementSpeed = Character::COMBAT_MOVE_SPEED;
	json combat = { { "combat", 1 } };
	SendGMCP("char.vitals " + combat.dump());
	victim->SendGMCP("char.vitals " + combat.dump());

	SendTargetSubscriberGMCP("target.vitals " + combat.dump());
	victim->SendTargetSubscriberGMCP("target.vitals " + combat.dump());

	if (HasGroup())
	{
		combat = { { "name", GetName() },{ "combat", 1 } };
		int first_slot = GetGroup()->FindFirstSlotInSubgroup((Player*)this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
			{
				GetGroup()->members[i]->SendGMCP("group.vitals " + combat.dump());
			}
		}
	}
}

void Character::EnterCombatAssist(Character * friendly)
{
	if (this == friendly)
		return;

	if (!IsAlive() || !friendly->IsAlive())
	{
		LogFile::Log("error", "EnterCombatAssist called on !IsAlive ch or vict");
		return;
	}

	if (!friendly->combat)
	{
		return;
	}

	combat = true;
    if(IsPlayer())
		((Player*)(this))->lastCombatAction = Game::currentTime;

	//go through friendly's threat list, add "zero" threat to everybody there
	for (auto threatiter = friendly->threatList.begin(); threatiter != friendly->threatList.end(); threatiter++)
	{
		Threat * threat = &(*threatiter);
		if (IsNPC() || threat->ch->IsNPC())
		{
			threat->ch->UpdateThreat(this, 0, Character::Threat::THREAT_DAMAGE);
			UpdateThreat(threat->ch, 0, Character::Threat::THREAT_DAMAGE);
		}
	}

    movementSpeed = Character::COMBAT_MOVE_SPEED;
	json combat = { { "combat", 1 } };
	SendGMCP("char.vitals " + combat.dump());

	SendTargetSubscriberGMCP("target.vitals " + combat.dump());

	if (HasGroup())
	{
		combat = { { "name", GetName() },{ "combat", 1 } };
		int first_slot = GetGroup()->FindFirstSlotInSubgroup((Player*)this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
			{
				GetGroup()->members[i]->SendGMCP("group.vitals " + combat.dump());
			}
		}
	}
}

void Character::ExitCombat()
{
    combat = false;
    meleeActive = false;
    RemoveThreat(nullptr, true);
    movementSpeed = Character::NORMAL_MOVE_SPEED;
	json combat = { { "combat", 0 } };
	SendGMCP("char.vitals " + combat.dump());

	SendTargetSubscriberGMCP("target.vitals " + combat.dump());

	if (HasGroup())
	{
		combat = { { "name", GetName() },{ "combat", 0 } };
		int first_slot = GetGroup()->FindFirstSlotInSubgroup((Player*)this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
			{
				GetGroup()->members[i]->SendGMCP("group.vitals " + combat.dump());
			}
		}
	}
}

bool Character::InCombat()
{
    if(combat)// && meleeActive)
        return true;
    return false;
}

void Character::AutoAttack(Character * victim)
{
    if(victim == nullptr)
        return;

    //unarmed
    double weaponSpeed_main = 2.0; 
    double weaponSpeed_off = 2.0;
    int damage_main = 1;
    int damage_off = 1;
    bool attack_mh = true;
    bool attack_oh = false; //weapon required for offhand attack (no unarmed)

	//NPC autoattack
    if(IsNPC() && lastAutoAttack_main + ((NPC*)(this))->GetNPCIndex()->npcAttackSpeed <= Game::currentTime)
    {
		NPC * thisnpc = (NPC*)this;
        if(victim->target == nullptr) //Force a target on our victim
        {
            victim->SetTarget(this);
            //Have the victim retaliate when attacked with no target set
            victim->meleeActive = true;
        }
        if(victim->IsPlayer())
            ((Player*)(victim))->lastCombatAction = Game::currentTime;
        lastAutoAttack_main = Game::currentTime;

		double percent = (double)(Server::rand() % 100);
		double range_low = 0;
		double range_high = BASE_MISS_CHANCE;
		bool crit = false;
		if (percent <= range_high)
		{
			//miss
			victim->Send("|Y" + GetName() + "'s attack misses you.|X\n\r");
			return;
		}
		range_low = range_high;
		range_high += victim->GetDodge();
		if (percent > range_low && percent <= range_high)
		{
			//dodge
			victim->Send("|YYou dodge " + GetName() + "'s attack.|X\n\r");
			return;
		}
		range_low = range_high;
		range_high += GetCrit();
		if (percent > range_low && percent <= range_high)
		{
			//crit
			crit = true;
		}

        int damage = thisnpc->GetNPCIndex()->npcDamageLow;
        if(thisnpc->GetNPCIndex()->npcDamageHigh != thisnpc->GetNPCIndex()->npcDamageLow)
            damage = (Server::rand() % (thisnpc->GetNPCIndex()->npcDamageHigh - thisnpc->GetNPCIndex()->npcDamageLow)) + thisnpc->GetNPCIndex()->npcDamageLow;
		if (victim->IsPlayer())
			((Player*)(victim))->GenerateRageOnTakeDamage(damage);

		string hitcrit = "hits";
		if (crit)
		{
			damage = (int)(damage * 1.5);
			hitcrit = "CRITS";
		}
		victim->Send("|Y" + GetName() + "'s attack " + hitcrit + " you for " + Utilities::itos(damage) + " damage.|X\n\r");
		//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

        OneHit(victim, damage);
        //victim may be invalid here if it was killed!
    }
    else if(IsPlayer()) //Player autoattack
    {
		Player * thisplayer = (Player*)this;
		weaponSpeed_main = thisplayer->GetMainhandWeaponSpeed();
		damage_main = thisplayer->GetMainhandDamageRandomHit();
		if(damage_main == 0)
			attack_mh = false; //no mainhand attack if we're holding a non weapon

		weaponSpeed_off = thisplayer->GetOffhandWeaponSpeed();
		damage_off = thisplayer->GetOffhandDamageRandomHit();
		if(damage_off == 0)
			attack_oh = false;

		string tapcolor = "|G";
		Character * thetap = victim->GetTap();
		if (thetap != nullptr && thetap != this && !thetap->InSameGroup(this))
		{
			tapcolor = "|D";
		}

        if(attack_mh && thisplayer->lastAutoAttack_main + weaponSpeed_main <= Game::currentTime)
        {
			damage_main += (int)ceil((thisplayer->GetStrength() * Player::STRENGTH_DAMAGE_MODIFIER) / weaponSpeed_main);
            if(victim->target == nullptr) //Force a target on our victim
            {     
                victim->SetTarget(this);
                //Have the victim retaliate when attacked with no target set
                victim->meleeActive = true;
            }
			thisplayer->lastCombatAction = Game::currentTime;
            if(victim->IsPlayer())
				((Player*)(victim))->lastCombatAction = Game::currentTime;
            lastAutoAttack_main = Game::currentTime;

			double percent = (double)(Server::rand() % 100);
			double range_low = 0;
			double range_high = BASE_MISS_CHANCE;
			bool crit = false;
			if (percent <= range_high)
			{
				//miss
				Send(tapcolor + "Your attack misses " + victim->GetName() + "|X\n\r");
				victim->Send("|Y" + GetName() + "'s attack misses you.|X\n\r");
				return;
			}
			range_low = range_high;
			range_high += victim->GetDodge();
			if (percent > range_low && percent <= range_high)
			{
				//dodge
				Send(tapcolor + victim->GetName() + " dodges your attack.|X\n\r");
				victim->Send("|YYou dodge " + GetName() + "'s attack.|X\n\r");
				return;
			}
			range_low = range_high;
			range_high += GetCrit();
			if (percent > range_low && percent <= range_high)
			{
				//crit
				crit = true;
			}

			string hitcrit = "hit";
			string hitcrits = "hits";
			if (crit)
			{
				damage_main = (int)(damage_main * 1.5);
				hitcrit = "CRIT";
				hitcrits = "CRITS";
			}

			GenerateRageOnAttack(damage_main, weaponSpeed_main, true, false);

			Send(tapcolor + "You " + hitcrit + " " + victim->GetName() + " for " + Utilities::itos(damage_main) + " damage.|X\n\r");
			victim->Send("|Y" + GetName() + " " + hitcrits + " you for " + Utilities::itos(damage_main) + " damage.|X\n\r");
			//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage_main) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

            OneHit(victim, damage_main);
            //victim may be invalid if it was killed!
        }
        if(attack_oh && thisplayer->lastAutoAttack_off + weaponSpeed_off <= Game::currentTime)
        {
			damage_off += (int)ceil((thisplayer->strength * Player::STRENGTH_DAMAGE_MODIFIER) / weaponSpeed_off);
            if(victim->target == nullptr) //Force a target on our victim
            {     
                victim->SetTarget(this);
                //Have the victim retaliate when attacked with no target set
                victim->meleeActive = true;
            }
			thisplayer->lastCombatAction = Game::currentTime;
            if(victim->IsPlayer())
                ((Player*)victim)->lastCombatAction = Game::currentTime;
			thisplayer->lastAutoAttack_off = Game::currentTime;

			double percent = (double)(Server::rand() % 100);
			double range_low = 0;
			double range_high = BASE_MISS_CHANCE;
			bool crit = false;
			if (percent <= range_high)
			{
				//miss
				Send(tapcolor + "Your attack misses " + victim->GetName() + "|X\n\r");
				victim->Send("|Y" + GetName() + "'s attack misses you.|X\n\r");
				return;
			}
			range_low = range_high;
			range_high += victim->GetDodge();
			if (percent > range_low && percent <= range_high)
			{
				//dodge
				Send(tapcolor + victim->GetName() + " dodges your attack.|X\n\r");
				victim->Send("|YYou dodge " + GetName() + "'s attack.|X\n\r");
				return;
			}
			range_low = range_high;
			range_high += GetCrit();
			if (percent > range_low && percent <= range_high)
			{
				//crit
				crit = true;
			}

			string hitcrit = "hit";
			string hitcrits = "hits";
			if (crit)
			{
				damage_off = (int)(damage_off * 1.5);
				hitcrit = "CRIT";
				hitcrits = "CRITS";
			}

			GenerateRageOnAttack(damage_off, weaponSpeed_off, false, false);

			Send(tapcolor + "You " + hitcrit + " " + victim->GetName() + " for " + Utilities::itos(damage_off) + " damage.|X\n\r");
			victim->Send("|Y" + GetName() + " " + hitcrits + " you for " + Utilities::itos(damage_off) + " damage.|X\n\r");
			//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage_off) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

            OneHit(victim, damage_off);
            //victim may be invalid if it was killed!
        }
    }
}

//returns the outcome of the attack, miss dodge parry block crit resist absorb? (need new enum)
//todo move stuff from AutoAttack in here
int Character::RunAttackTable(Character * victim, int school)
{
	if (school == Game::School::SCHOOL_PHYSICAL)
	{

	}
	return 0;
}

double Character::CalculateArmorMitigation(Character * victim)
{
	double percent_reduction = victim->GetArmor() / ((22 * GetLevel()) + victim->GetArmor() + 400);
	if(percent_reduction > ARMOR_MITIGATION_MAX)
		return ARMOR_MITIGATION_MAX;
	return percent_reduction;
	//%Reduction = (Armor / ([85 * Enemy_Level] + Armor + 400)) * 100
}

void Character::OneHit(Character * victim, int damage) 
{
    if(victim == nullptr)
        return;
    if(victim->remove)
    {
        //Victim is already toast
        return;
    }

    if((IsNPC() || victim->IsNPC()) && victim->InCombat())
		//Keep track of threat unless BOTH are players or victim is (somehow, "peace", leash with dots) not in combat
    {
        victim->UpdateThreat(this, damage, Threat::Type::THREAT_DAMAGE);
        //Send("My threat on " + victim->name + " is " + Utilities::itos(victim->GetThreat(this)) + "\n\r");
    }
	if (damage > 0 && victim->CancelCastOnHit())
		victim->Send("Action Interrupted!\n\r");

    victim->AdjustHealth(this, -damage);
}

void Character::OneHeal(Character * victim, int heal)
{
	if (victim == nullptr)
		return;
	if (victim->remove)
	{
		//victim is already toast
		return;
	}

	EnterCombatAssist(victim);

	for (auto threatiter = threatList.begin(); threatiter != threatList.end(); threatiter++)
	{
		Threat * threat = &(*threatiter);
		threat->ch->UpdateThreat(this, (double)heal / threatList.size(), Threat::Type::THREAT_HEALING);
	}
	victim->AdjustHealth(this, heal);
}


bool Character::IsFighting(Character * target)
{
    if(this == target)
        return false; //never fighting ourself
    if(this->target == target && meleeActive)
        return true;
    return false;
}

void Character::GenerateRageOnAttack(int damage, double weapon_speed, bool mainhand, bool wascrit)
{
	//Rage generation stuff, inspired by wow, for now
	double conversionvalue = (.008 * GetLevel() * GetLevel()) + 4;
	double hitfactor = 1.75;
	if (mainhand)
		hitfactor *= 2;
	if (wascrit)
		hitfactor *= 2;
	int limit = (int)ceil((15 * damage) / conversionvalue);
	int ragegen = (int)ceil(((15 * damage) / (4 * conversionvalue)) + ((hitfactor * weapon_speed) / 2));
	if (ragegen > limit)
		AdjustRage(this, limit);
	else
		AdjustRage(this, ragegen);
}

void Character::GenerateRageOnTakeDamage(int damage)
{
	//Rage generation stuff, inspired by wow, for now
	double conversionvalue = (.008 * GetLevel() * GetLevel()) + 4;
	AdjustRage(this, (int)ceil((5 * damage) / (2 * conversionvalue)));
}

void Character::OnDeath()
{
	std::list<Character::Threat>::iterator threatiter;
	for (threatiter = threatList.begin(); threatiter != threatList.end(); threatiter++)
	{
		Threat * threat = &(*threatiter);
		if (!threat->ch) //if this is possible we have bigger problems
			continue;

		if (threat->ch->GetTarget() && threat->ch->GetTarget() == this)
		{
			threat->ch->meleeActive = false;
		}
		if (threat->ch->HasComboPointTarget() && threat->ch->GetComboPointTarget() == this)
		{
			threat->ch->ClearComboPointTarget();
		}
		threat->ch->RemoveThreat(this, false);
	}
	movementQueue.clear();
	CancelActiveDelay();
	RemoveAllSpellAffects();
	ClearTarget();
	ClearComboPointTarget();
	
	if (!IsNPC()) //player killed... doesn't matter by who, no rewards yet for doing so
	{
		Player * thisplayer = (Player *)this;
		ExitCombat();			//Removes our threat list
		thisplayer->SetQuery("Release spirit? ('release') ", nullptr, releaseSpiritQuery);
	}
	else if (IsNPC()) //NPC killed, figure out by who...
	{
		NPC * thisnpc = (NPC*)this;
		Character * tap = nullptr;
		Character * highdamage = nullptr;
		double group_damage = 0;
		double other_damage = 0;
		double highest_damage = 0;
		//find out who has the tap
		std::list<Threat>::iterator iter;
		for (iter = threatList.begin(); iter != threatList.end(); ++iter)
		{
			if ((*iter).tapped)
			{
				tap = (*iter).ch;
			}
			if ((*iter).damage >= highest_damage) //also find the individual with the highest damage for the (bug?) case where no one has the tap
			{
				highest_damage = (*iter).damage;
				highdamage = (*iter).ch;
			}
		}

		if (!highdamage)
		{
			LogFile::Log("error", "OnDeath(): npc killed with null highdamage");
			return;
		}
		if (!tap)
		{
			LogFile::Log("error", "OnDeath(): npc killed with null tap");
			tap = highdamage;
		}

		//now that we know who has the tap, go through the list again to total 
		// all the damage done by their group, and damage done outside of group
		for (iter = threatList.begin(); iter != threatList.end(); ++iter)
		{
			if (iter->ch == tap || iter->ch->InSameGroup(tap))
			{
				group_damage += iter->damage;
			}
			else
			{
				other_damage += iter->damage;
			}
		}

		if(group_damage >= other_damage && tap->IsPlayer()) //The tap's group did most of the damage, give rewards
		{
			Player * tap_player = (Player *)tap;
			if (tap_player->HasGroup())
			{
				for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
				{
					Player * group_member = tap_player->GetGroup()->members[i];
					if (group_member != nullptr && FindDistance(this->room, group_member->room, Player::GROUP_LOOT_DISTANCE) != -1)
					{
						group_member->HandleNPCKillRewards(this);
					}
				}
			}
			else if (FindDistance(this->room, tap_player->room, Player::GROUP_LOOT_DISTANCE) != -1)
			{
				tap_player->HandleNPCKillRewards(this);
			}

			int loot_id_ctr = 1;
			for (auto dropiter = thisnpc->GetNPCIndex()->drops.begin(); dropiter != thisnpc->GetNPCIndex()->drops.end(); ++dropiter)
			{
				if (Server::rand() % 100 <= (*dropiter).percent && (*dropiter).id.size() > 0)
				{
					int which = Server::rand() % ((int)(*dropiter).id.size());
					Item * drop = Game::GetGame()->GetItem((*dropiter).id[which]);

					NPC::OneLoot one_loot;
					one_loot.item = drop;
					one_loot.id = loot_id_ctr++;
					one_loot.roll_timer = 0;

					if (tap->HasGroup())
					{
						for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
						{
							Player * group_member = tap->GetGroup()->members[i];
							if (group_member != nullptr && (!drop->quest || group_member->ShouldDropQuestItem(drop))
								&& FindDistance(this->room, group_member->room, Player::GROUP_LOOT_DISTANCE) != -1)
							{
								one_loot.looters.push_back(NPC::Looter(group_member));
								group_member->AddSubscriber(this);

								group_member->Send(GetName() + " drops loot: " + (string)Item::quality_strings[drop->quality] + drop->GetName() + "|X");
								if (drop->quality >= Item::QUALITY_UNCOMMON)
								{
									int my_id = group_member->AddLootRoll(one_loot.id, thisnpc);
									one_loot.roll_timer = Game::currentTime + 60;
									group_member->Send(" |Y(60 seconds to roll: #" + Utilities::itos(my_id) + ")|X");
								}
								group_member->Send("\n\r");
							}
						}
					}
					else if((!drop->quest || tap_player->ShouldDropQuestItem(drop)) && FindDistance(this->room, tap->room, Player::GROUP_LOOT_DISTANCE) != -1)
					{
						one_loot.looters.push_back(NPC::Looter(tap_player));
						tap->AddSubscriber(this);

						tap->Send(GetName() + " drops loot: " + (string)Item::quality_strings[drop->quality] + drop->GetName() + "|X\n\r");
					}
					if(!one_loot.looters.empty()) //Don't actually drop this loot if no one can loot it
						thisnpc->loot.push_back(one_loot);
				}
			}
		}
		ExitCombat(); //Removes NPC's threat list
	}
	SetCorpse();
}

//To be used in spell _cost function. Checks AURA_RESOURCE_COST
bool Character::HasResource(int which, int amount)
{
    int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);

    double increase = amount * (resource_cost / 100.0);
    amount += (int)increase;

    switch(which)
    {
        case RESOURCE_HEALTH:
            if(health >= amount)
                return true;
            break;
        case RESOURCE_MANA:
            if(mana >= amount)
                return true;
            break;
        case RESOURCE_ENERGY:
            if(energy >= amount)
                return true;
            break;
		case RESOURCE_RAGE:
			if (rage >= amount)
				return true;
			break;
		case RESOURCE_COMBO:
			if (GetComboPoints() >= amount)
				return true;
			break;
        default:
            LogFile::Log("error", "Character::HasResource, bad resource id");
            return false;
    }
    return false;
}

void Character::AdjustHealth(Character * source, int amount)
{
	if (!IsAlive())
	{
		LogFile::Log("error", "AdjustHealth() called on !IsAlive() character");
		return;
	}
	if (source == nullptr)
	{
		//a possibility
		source = this; //self damage!?
	}
	SetHealth(GetHealth() + amount);

	if (GetHealth() == 0)
	{
		Message("|R" + GetName() + " has been slain!|X", Character::MSG_ROOM_NOTCHARVICT, source);
		source->Send("|R" + GetName() + " has been slain!|X\n\r");
		Send("|RYou have been slain!|X\n\r");

		OnDeath(); //The source of damage shouldn't matter except where cases of "killing blow" matters (none so far?)
	}
}

void Character::SetHealth(int amount)
{
	if (amount <= 0)
	{
		if (IsImmortal())
			health = 1;
		else
			health = 0;
	}
	else if (amount > GetMaxHealth())
	{
		health = GetMaxHealth();
	}
	else
	{
		health = amount;
	}

	json vitals = { { "hp", health } };
	SendGMCP("char.vitals " + vitals.dump());
	int percent = 0;
	if (GetMaxHealth() > 0)
		percent = (health * 100) / GetMaxHealth();
	vitals = { { "hppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	SendTargetTargetSubscriberGMCP("targettarget.vitals " + vitals.dump());

	if (HasGroup())
	{
		int first_slot = GetGroup()->FindFirstSlotInSubgroup((Player *)this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
			{
				vitals = { { "name", GetName() }, { "hp", GetHealth() } };
				GetGroup()->members[i]->SendGMCP("group.vitals " + vitals.dump());
			}
		}
	}
}

//Mana adjusting function to be used by spells. Invokes 5 second rule and checks for AURA_RESOURCE_COST
//A negative amount would indicate a mana gain
void Character::ConsumeMana(int amount)
{
    int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);
    double increase = amount * (resource_cost / 100.0);
    amount += (int)increase;

	//start 5 second rule if we lost mana
	if (amount > 0)
	{
		this->lastSpellCast = Game::currentTime;
	}

	SetMana(GetMana() - amount);
}

//Does NOT reset mana regen or check for AURA_RESOURCE_COST
void Character::AdjustMana(Character * source, int amount)
{
	SetMana(mana + amount);
}

void Character::SetMana(int amount)
{
	if (amount < 0)
	{
		mana = 0;
	}
	else if (amount > GetMaxMana())
	{
		mana = GetMaxMana();
	}
	else
	{
		mana = amount;
	}

	json vitals = { { "mp", mana } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (GetMaxMana() > 0)
		percent = (mana * 100) / GetMaxMana();
	vitals = { { "mppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());

	if (HasGroup())
	{
		int first_slot = GetGroup()->FindFirstSlotInSubgroup((Player *)this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (GetGroup()->members[i] != nullptr && GetGroup()->members[i] != this)
			{
				vitals = { { "name", GetName() },{ "mp", GetMana() } };
				GetGroup()->members[i]->SendGMCP("group.vitals " + vitals.dump());
			}
		}
	}
}

//Energy adjusting function to be used by spells. Checks for AURA_RESOURCE_COST
//A negative amount would indicate an energy gain
void Character::ConsumeEnergy(int amount)
{
	int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);
	double increase = amount * (resource_cost / 100.0);
	amount += (int)increase;

	SetEnergy(GetEnergy() - amount);
}

void Character::AdjustEnergy(Character * source, int amount)
{
	SetEnergy(GetEnergy() + amount);
}

void Character::SetEnergy(int amount)
{
	if (amount < 0)
	{
		energy = 0;
	}
	else if (amount > GetMaxEnergy())
	{
		energy = GetMaxEnergy();
	}
	else
	{
		energy = amount;
	}
	json vitals = { { "en", energy } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (GetMaxEnergy() > 0)
		percent = (energy * 100) / GetMaxEnergy();
	vitals = { { "enpercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}

//Rage adjusting function to be used by spells. Checks for AURA_RESOURCE_COST
//A negative amount would indicate an rage gain
void Character::ConsumeRage(int amount)
{
	int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);
	double increase = amount * (resource_cost / 100.0);
	amount += (int)increase;

	SetRage(GetRage() - amount);
}

void Character::AdjustRage(Character * source, int amount)
{
	SetRage(GetRage() + amount);
}

void Character::SetRage(int amount)
{
	if (amount < 0)
	{
		rage = 0;
	}
	else if (amount > GetMaxRage())
	{
		rage = GetMaxRage();
	}
	else
	{
		rage = amount;
	}
	json vitals = { { "rage", rage } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (GetMaxRage() > 0)
		percent = (rage * 100) / GetMaxRage();
	vitals = { { "ragepercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}



void Character::UpdateThreat(Character * ch, double value, int type)
{
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
			switch (type)
			{
				case Threat::Type::THREAT_DAMAGE:
					if (IsNPC() && GetTap() == nullptr && value > 0)
					{
						//No one has the tap yet and THREAT_DAMAGE > 0, they get the tap
						(*iter).tapped = true;
					}
					(*iter).threat += value;
					(*iter).damage += value;
					break;
				case Threat::Type::THREAT_HEALING:
					(*iter).threat += value/2;
					(*iter).healing += value;
					break;
				case Threat::Type::THREAT_OTHER:
					(*iter).threat += value;
					break;
			}
            return;
        }
    }
	ch->AddSubscriber(this);
	//cout << "UpdateThreat ADD" << endl;
	Threat tt = { ch, 0, 0, 0, false };
	switch (type)
	{
	case Threat::Type::THREAT_DAMAGE:
		tt.threat = value;
		tt.damage = value;
		break;
	case Threat::Type::THREAT_HEALING:
		tt.threat = value/2;
		tt.healing = value;
		break;
	case Threat::Type::THREAT_OTHER:
		tt.threat = value;
		break;
	}
	if (IsNPC() && GetTap() == nullptr && value > 0 && type == Threat::Type::THREAT_DAMAGE)
	{
		//No one has the tap yet and THREAT_DAMAGE > 0, they get the tap
		tt.tapped = true;
	}
	threatList.push_front(tt);
}

Character * Character::GetTopThreat()
{
	if (threatList.empty())
		return nullptr;

	double max = 0;
	Character * maxch = nullptr;
	for (std::list<Threat>::iterator iter = threatList.begin(); iter != threatList.end(); iter++)
	{
		if (iter->threat >= max)
		{
			max = iter->threat;
			maxch = iter->ch;
		}
	}

    return maxch;
}

void Character::RemoveThreat(Character * ch, bool removeall)
{
    if(removeall)
    {
        std::list<Threat>::iterator iter;
        for(iter = threatList.begin(); iter != threatList.end(); ++iter)
        {
            (*iter).ch->RemoveSubscriber(this);
			//cout << "RemoveThreat REMOVE" << endl;
        }
        threatList.clear();
        return;
    }
    
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
            (*iter).ch->RemoveSubscriber(this);
			//cout << "RemoveThreat REMOVE" << endl;
            threatList.erase(iter);
            return;
        }
    }
}

bool Character::HasTap(Character * target)
{
	std::list<Threat>::iterator iter;
	for (iter = target->threatList.begin(); iter != target->threatList.end(); ++iter)
	{
		if (iter->ch == this && iter->tapped == true)
		{
			return true;
		}
	}
	return false;
}

Character * Character::GetTap()
{
	std::list<Threat>::iterator iter;
	for (iter = threatList.begin(); iter != threatList.end(); ++iter)
	{
		if (iter->tapped == true)
		{
			return iter->ch;
		}
	}
	return nullptr;
}

double Character::GetThreat(Character * ch)
{
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
            return (*iter).threat;
        }
    }
    return 0;
}

bool Character::HasThreat(Character * ch)
{
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
            return true;
        }
    }
    return false;
}

//Go through our threat list and see if anyone on it (NPCs) is still in combat. Used to determine player combat status
bool Character::CheckThreatCombat()
{
	std::list<Threat>::iterator iter;
	for (iter = threatList.begin(); iter != threatList.end(); ++iter)
	{
		if ((*iter).ch->InCombat())
		{
			return true;
		}
	}
	return false;
}

bool Character::CancelActiveDelay()
{
	if (delay_active)
	{
		if (delayData.charTarget && delayData.caster && delayData.charTarget != delayData.caster)
		{
			delayData.charTarget->RemoveSubscriber(delayData.caster);
		}
		if (GetGlobalCooldown() > Game::currentTime) //If we're canceling a cast and gcd is active, reset it
		{
			SetGlobalCooldown(Game::currentTime);
		}
		json casttime = { { "time", 0 } };
		SendGMCP("char.casttime " + casttime.dump());
		delay_active = false;
		return true;
	}
	return false;
}

bool Character::CancelCastOnHit()
{
	if(delay_active && (!delayData.sk || delayData.sk->interruptFlags.test(Skill::Interrupt::INTERRUPT_HIT)))
	{
		CancelActiveDelay();
		return true;
	}
	return false;
}

std::string Character::AggressionColor(Character * target)
{
	if (!target->IsAlive())
	{
		return "|D";
	}

	if (!target->IsNPC())
	{
		return "|C";
	}
	else if (!target->IsAlive())
	{
		return "|D";
	}
	else if (target->FlagIsSet(NPCIndex::FLAG_FRIENDLY))
	{
		return "|G";
	}
	else if (target->FlagIsSet(NPCIndex::FLAG_NEUTRAL))
	{
		return "|Y";
	}
	else// if (target->FlagIsSet(NPCIndex::FLAG_AGGRESSIVE))
	{
		return "|R";
	}
}

std::string Character::AggressionLightColor(Character * target)
{
	if (!target->IsAlive())
	{
		return "|D";
	}

	if (!target->IsNPC())
	{
		return "|c";
	}
	else if (!target->IsAlive())
	{
		return "|D";
	}
	else if (target->FlagIsSet(NPCIndex::FLAG_FRIENDLY))
	{
		return "|g";
	}
	else if (target->FlagIsSet(NPCIndex::FLAG_NEUTRAL))
	{
		return "|y";
	}
	else// if (target->FlagIsSet(NPCIndex::FLAG_AGGRESSIVE))
	{
		return "|r";
	}
}

std::string Character::HisHer()
{
	return (GetGender() == 1 ? "his" : "her");
}

std::string Character::HimHer()
{
	return (GetGender() == 1 ? "him" : "her");
}

std::string Character::HisHers()
{
	return (GetGender() == 1 ? "his" : "hers");
}

bool Character::CanAttack(Character * victim)
{
	if ((victim->IsNPC() && victim->FlagIsSet(NPCIndex::FLAG_FRIENDLY))
		|| (!victim->IsNPC() && room->pvp == 0)
		|| (!victim->IsAlive()))
	{
		return false;
	}
	return true;
}

bool Character::CanMove()
{
    if(IsImmortal())
        return true;

	double movespeedPercent = GetMoveSpeed();

    if(movespeedPercent <= 0)
        return false;

    return (Game::currentTime > lastMoveTime + (1.0/(movementSpeed * movespeedPercent)));
}

double Character::GetMoveSpeed()
{
    int low = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 5);
    int high = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 4);
 
	double newspeed;
	if (IsNPC())
		newspeed = 110 + high + low;	//%110 base movement speed for NPCS
	else
		newspeed = 100 + high + low;

    if(newspeed <= 0) 
        return 0;

    return newspeed / 100;
}



void Character::SetCooldown(Skill * sk, double length) //USE LENGTH -1 TO USE SKILL->COOLDOWN
{
    if(length == 0)
        return;

    if(sk != nullptr)
    {
        if(length < 0) //if length default argument == -1, use sk->cooldown
            length = sk->cooldown;
        cooldowns[sk->id] = Game::currentTime + length;
    }
}

double Character::GetCooldownRemaining(Skill * sk)
{
    if(sk == nullptr)
        return 0;

    if(IsImmortal())
        return 0;

	//Global cooldown in progress
	double remaining_global = 0;
	if (!Utilities::FlagIsSet(sk->flags, Skill::FLAG_GCDIMMUNE) && this->GetGlobalCooldown() > Game::currentTime)
	{
		remaining_global = this->GetGlobalCooldown() - Game::currentTime;
	}

	double remaining_cd = 0;
    auto iter = cooldowns.find(sk->id);
	if (iter != cooldowns.end() && (*iter).second > Game::currentTime)
		remaining_cd = (*iter).second - Game::currentTime;

	if(remaining_global > remaining_cd)
		return remaining_global;
	return remaining_cd;
}

//room == nullptr to remove from room only
bool Character::ChangeRooms(Room * toroom)
{
    if(room != nullptr)
        room->characters.remove(this);
    if(toroom != nullptr)
    {
		if (IsPlayer())
		{
			//Check if this room is a quest objective
			((Player*)this)->QuestCompleteObjective(Quest::OBJECTIVE_ROOM, (void*)toroom);
			//Send GMCP room.info
			json roominfo;
			roominfo["name"] = toroom->name;
			roominfo["num"] = toroom->id;
			if(toroom->area != 0 && Game::GetGame()->GetArea(toroom->area) != nullptr)
			{
				roominfo["zone"] = Game::GetGame()->GetArea(toroom->area)->name;
			}
			else
			{
				roominfo["zone"] = "None";
			}

			for (int i = 0; i < Exit::DIR_LAST; i++)
			{
				if (toroom->exits[i] && toroom->exits[i]->to)
					roominfo["exits"][Exit::exitNamesShort[i]] = toroom->exits[i]->to->id;
			}
			SendGMCP("room.info " + roominfo.dump());
		}
		
        room = toroom;
        toroom->characters.push_front(this);

        //Check for room movement triggers
        Trigger * trig = nullptr;
        int ctr = 0;
        Trigger::TriggerType tt;

        //ENTER_NPC, ENTER_PC
        if(IsPlayer())
            tt = Trigger::ENTER_PC;
        else
            tt = Trigger::ENTER_NPC;

        while((trig = toroom->GetTrigger(ctr, tt)) != nullptr)
        {
            ctr++;
            string func = trig->GetFunction();
            try
            {
                //TODO: dont load the script every time?
                //LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for room " + Utilities::itos(toroom->id));
				Server::lua.script(trig->GetScript().c_str());
				sol::function lua_trig_func = Server::lua[func.c_str()];
				sol::protected_function_result result = lua_trig_func(this, toroom);
				if (!result.valid())
				{
					// Call failed
					sol::error err = result;
					std::string what = err.what();
					LogFile::Log("error", "room ENTER_PC/ENTER_NPC trigger call failed, sol::error::what() is: " + what);
				}
			
            }
            catch(const std::exception & e)
			{
				LogFile::Log("error", e.what());
			}
        }

        //ENTER_CHAR
        tt = Trigger::ENTER_CHAR;
        while((trig = toroom->GetTrigger(ctr, tt)) != nullptr)
        {
            ctr++;
            string func = trig->GetFunction();
			try
			{
				//TODO: dont load the script every time?
				//LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for room " + Utilities::itos(toroom->id));
				Server::lua.script(trig->GetScript().c_str());
				sol::function lua_trig_func = Server::lua[func.c_str()];
				sol::protected_function_result result = lua_trig_func(this, toroom);
				if (!result.valid())
				{
					// Call failed
					sol::error err = result;
					std::string what = err.what();
					LogFile::Log("error", "room ENTER_CHAR trigger call failed, sol::error::what() is: " + what);
				}

			}
			catch (const std::exception & e)
			{
				LogFile::Log("error", e.what());
			}
        }
    }

    return true;
}

bool Character::ChangeRoomsID(int roomid)
{
    if(roomid != 0)
    {
        std::map<int, Room*>::iterator iter;
        if((iter = Game::GetGame()->rooms.find(roomid)) == Game::GetGame()->rooms.end())
            return false;
        return ChangeRooms(iter->second);
    }
    return ChangeRooms(nullptr);
}

void Character::SetCorpse()
{
	isCorpse = true;
	deathTime = Utilities::GetTime();
}

void Character::SetAlive()
{
	isCorpse = false;
}

bool Character::IsCorpse()
{
	return isCorpse;
}

int Character::TimeSinceDeath()
{
	return (int)(Game::currentTime - deathTime);
}

void Character::SetTarget(Character * t)
{
    if(target == t)
        return;
    if(target != nullptr)
    {
        ClearTarget();
    }
    t->AddSubscriber(this);
	//cout << "SetTarget ADD" << endl;
    target = t;
}

void Character::ClearTarget()
{
    if(target)
    {
		//if (target->GetTarget())
		//{
			json targettargetvitals = { { "name", "" },{ "level", 0 },{ "hppercent", 0 } };
			SendGMCP("targettarget.vitals " + targettargetvitals.dump());
		//}
        target->RemoveSubscriber(this);
		//cout << "ClearTarget REMOVE" << endl;
        target = nullptr;
		json vitals = { { "name", "" },{ "level", 0 },{ "hppercent", 0 },{ "mppercent", 0 }, { "enpercent", 0 },{ "ragepercent", 0 } };
		SendGMCP("target.vitals " + vitals.dump());
    }
}

Character * Character::GetTarget()
{
    return target;
}

void Character::Notify(SubscriberManager * lm)
{
    //with the parameter we can check if the thing that just notified us is our target, 
    //  or is our spell cast target (delaydata.charTarget), or ...
    if(target && lm == target)
    {
		//ClearTarget();
        target->RemoveSubscriber(this);
		//cout << "Character::Notify target REMOVE" << endl;
        target = nullptr;
    }

    if(delay_active && delayData.charTarget == lm)
    {
        delayData.charTarget->RemoveSubscriber(this);
		//cout << "Character::Notify delaydata REMOVE" << endl;
        delayData.charTarget = nullptr;
		delay_active = false;
    }

    if(HasThreat((Character*)lm))
    {
        RemoveThreat((Character*)lm, false);
    }

	if (delay_active && delayData.itemTarget == lm)
	{
		delayData.itemTarget->RemoveSubscriber(this);
		delayData.itemTarget = nullptr;
		delay_active = false;
	}
}



bool Character::InSameGroup(Character * ch)
{
	if (ch == this)
		return true;
	if (HasGroup() && ch->HasGroup() && GetGroup() == ch->GetGroup())
		return true;
	return false;
}
