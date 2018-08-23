#include "stdafx.h"


extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define DB_INVENTORY_EQUIPPED 0
#define DB_INVENTORY_INVENTORY 1
#define DB_INVENTORY_BANK 2

using namespace std;

//in rooms per second
//const double Character::NORMAL_MOVE_SPEED = 2.5;
//const double Character::COMBAT_MOVE_SPEED = 0.5;

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

Character::flag_type Character::flag_table[] = 
{
    { Character::FLAG_FRIENDLY, "friendly" },
    { Character::FLAG_NEUTRAL, "neutral" },
    { Character::FLAG_AGGRESSIVE, "aggressive" },
    { Character::FLAG_GUILD, "guild" },
    { Character::FLAG_VENDOR, "vendor" },
    { Character::FLAG_REPAIR, "repair" },
    { Character::FLAG_TRAINER, "trainer" },
    { -1, "" }
};

Character::Character()
{
    name = "NPC";
    id = 0;
    player = NULL;
    SetDefaults();
}

Character::Character(string name_, int id_)
{
    name = name_;
    id = id_;
    player = NULL;
    SetDefaults();
}

Character::Character(string name_, User * user_)
{
    name = name_;
    id = 0;
    player = new Player(user_);
    SetDefaults();
}

Character::Character(const Character & copy)
{
    SetDefaults();
    name = copy.name;
	keywords = copy.keywords;
    id = copy.id;
    title = copy.title;
    player = NULL;
    level = copy.level;
    gender = copy.gender;
	race = copy.race;
    agility = copy.agility;
    intellect = copy.intellect;
    strength = copy.strength;
    stamina = copy.stamina;
    wisdom = copy.wisdom;
	spirit = copy.spirit;
    health = maxHealth = copy.maxHealth;
    mana = maxMana = copy.maxMana;
	energy = maxEnergy = copy.maxEnergy;
	rage = 0;
	maxRage = copy.maxRage;
	speechText = copy.speechText;
    npcAttackSpeed = copy.npcAttackSpeed;
    npcDamageLow = copy.npcDamageLow;
    npcDamageHigh = copy.npcDamageHigh;
    delay_active = false;
    combat = false;
    meleeActive = false;
    movementSpeed = NORMAL_MOVE_SPEED;
	position = copy.position;
    lastMoveTime = 0;
	isCorpse = false;
    changed = false;

    std::map<std::string, Skill *>::const_iterator skilliter;
    for(skilliter = copy.knownSkills.begin(); skilliter != copy.knownSkills.end(); ++skilliter)
    {
        knownSkills[(*skilliter).second->name] = (*skilliter).second;
    }

    std::vector<int>::const_iterator flagiter;
    for(flagiter = copy.flags.begin(); flagiter != copy.flags.end(); ++flagiter)
    {
        flags.push_back((*flagiter));
    }

    std::vector<Quest *>::const_iterator questiter;
    for(questiter = copy.questStart.begin(); questiter != copy.questStart.end(); ++questiter)
    {
        questStart.push_back((*questiter));
    }
    for(questiter = copy.questEnd.begin(); questiter != copy.questEnd.end(); ++questiter)
    {
        questEnd.push_back((*questiter));
    }

    std::list<DropData>::const_iterator dropiter;
    for(dropiter = copy.drops.begin(); dropiter != copy.drops.end(); ++dropiter)
    {
        drops.push_back(*dropiter);
    }

    std::map<int, Trigger>::const_iterator triggeriter;
    for(triggeriter = copy.triggers.begin(); triggeriter != copy.triggers.end(); ++triggeriter)
    {
        triggers[(*triggeriter).second.id] = (*triggeriter).second;
    }
}

void Character::SetDefaults()
{
    remove = false;
    room = NULL;
    target = NULL;
    level = 1;
    gender = 1;
    agility = intellect = strength = stamina = wisdom = spirit = 5;
	energy = maxEnergy = 100;
	rage = 0;
	maxRage = 100;
	comboPoints = 0;
	maxComboPoints = 5;
    health = maxHealth = stamina * Character::HEALTH_FROM_STAMINA;
    mana = maxMana = wisdom * Character::MANA_FROM_WISDOM;
    npcAttackSpeed = 2.0;
    npcDamageLow = npcDamageHigh = 1;
    delay_active = false;
    combat = false;
    meleeActive = false;
    movementSpeed = NORMAL_MOVE_SPEED;
	position = Character::Position::POSITION_STANDING;
    lastMoveTime = 0;
    changed = false;
    reset = NULL;
    editState = ED_NONE;
    editData = NULL;
    hasQuery = false;
    queryData = NULL;
    queryFunction = NULL;
    buffs_invalid = false;
    debuffs_invalid = false;
    movementQueue.clear();

    stringTable["name"] = &name;
	stringTable["keywords"] = &keywords;
    intTable["id"] = &id;
    intTable["level"] = &level;
    intTable["gender"] = &gender;
	intTable["race"] = &race;
    intTable["agility"] = &agility;
    intTable["intellect"] = &intellect;
    intTable["strength"] = &strength;
    intTable["stamina"] = &stamina;
    intTable["wisdom"] = &wisdom;
	intTable["spirit"] = &spirit;
    intTable["health"] = &health;
    intTable["mana"] = &mana;
    doubleTable["attack_speed"] = &npcAttackSpeed;
    intTable["damage_low"] = &npcDamageLow;
    intTable["damage_high"] = &npcDamageHigh;
}

Character::~Character()
{
    if(player != NULL)
        delete player;

    player = NULL;
    room = NULL;
    threatList.clear();
    RemoveAllSpellAffects();
    knownSkills.clear();
    cooldowns.clear();
    triggers.clear();

    /*if(reset)
    {
        this->RemoveSubscriber(reset);
    }*/
}

void Character::SendBW(std::string str)
{
	if (player == NULL || player->user == NULL)
		return;

	player->user->SendBW(str);
}

void Character::Send(std::string str)
{
    if(player == NULL || player->user == NULL)
        return;

    player->user->Send(str);
}

void Character::Send(char * str)
{
    if(player == NULL || player->user == NULL)
        return;

    player->user->Send(str);
}

void Character::SendGMCP(std::string str)
{
    if(player == NULL || player->user == NULL || !player->user->gmcp)
        return;

    player->user->SendGMCP(str);
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

void Character::SendGMCP(char * str)
{
    if(player == NULL || player->user == NULL)
        return;

    player->user->SendGMCP(str);
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
			if (!group)
				return;
			for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
			{
				if (group->members[i] != nullptr)
				{
					group->members[i]->Send(txt + "\n\r");
				}
			}
			break;
		}

		case MSG_GROUP_NOTCHAR:
		{
			if (!group)
				return;
			for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
			{
				if (group->members[i] != nullptr && group->members[i] != this)
				{
					group->members[i]->Send(txt + "\n\r");
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

void Character::QueryClear()
{
	queryData = NULL;
	queryFunction = NULL;
	queryPrompt = "";
	hasQuery = false;
}

void Character::SetQuery(std::string prompt, void * data, bool(*func)(Character *, std::string))
{
	hasQuery = true;
	queryFunction = func;
	queryData = data;
	queryPrompt = prompt;
}

void * Character::GetQueryData()
{
	return queryData;
}

bool Character::HasQuery()
{
	return hasQuery;
}

bool (*Character::GetQueryFunc())(Character *, std::string)
{
	return queryFunction;
}

void Character::ResetMaxStats()
{
	//todo: check equipment bonuses
	SetMaxHealth(stamina * Character::HEALTH_FROM_STAMINA);
	SetMaxMana(wisdom * Character::MANA_FROM_WISDOM);
	//todo: these might be higher based on skills or talents?
	SetMaxEnergy(100);
	SetMaxRage(100);
	maxComboPoints = 5;
}

void Character::GeneratePrompt(double currentTime)
{
	string prompt = "\n\r";

	if(editState != ED_NONE)
	{
		switch(editState)
		{
            case ED_ROOM:
				prompt += "|G(Room)|X";
				break;
            case ED_SKILL:
                prompt += "|G(Skill)|X";
                break;
			case ED_PLAYER:
				prompt += "|G(Player)|X";
				break;
            case ED_NPC:
                prompt += "|G(NPC)|X";
                break;
            case ED_ITEM:
                prompt += "|G(Item)|X";
                break;
            case ED_QUEST:
                prompt += "|G(Quest)|X";
                break;
			case ED_HELP:
                prompt += "|G(Help)|X";
                break;
            case ED_AREA:
                prompt += "|G(Area)|X";
                break;
            case ED_CLASS:
                prompt += "|G(Class)|X";
                break;
			default:
				prompt += "|G(ED_?)|X";
				break;
		}
	}
    prompt += "|B<";

    int percent;
    string statColor;
    if(!IsGhost())
    {
        //Health
        if(health > 0 && maxHealth > 0)
            percent = (health * 100)/maxHealth;
        else
            percent = 0;

        if(percent >= 75)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        prompt += statColor + Utilities::itos(health) + "/|X" + Utilities::itos(maxHealth) + "|Bh ";

        //Mana
        if(mana > 0 && maxMana > 0)
            percent = (mana * 100)/maxMana;
        else
            percent = 0;

        if(percent >= 75)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        prompt += statColor + Utilities::itos(mana) + "/|X" + Utilities::itos(maxMana) + "|Bm ";

		//Energy
		prompt += "|X" + Utilities::itos(energy) + "/" + Utilities::itos(maxEnergy) + "|Be ";

		//Rage
		prompt += "|X" + Utilities::itos(rage) + "/|X" + Utilities::itos(maxRage) + "|Br";
    }
    else
    {
        prompt += "|xGhost";
    }
    prompt += "|B>|x";

    //Combat flag
    if(combat)
    {
        prompt += "|B<|RX|B>|X";
    }
    //Cast timer
    if(delay_active)
    {
        double timeleft = delay - currentTime;
        prompt += "|Y[" + Utilities::dtos(timeleft, 1) + "s]|X";
    }
    
    //Target
	if(GetTarget() != NULL)
	{
		int jsonvitals[5]; //level, health, mana, energy, rage as %'s
		string targetPrompt = "";

		//Combo points
		if (comboPoints > 0 && GetTarget() == comboPointTarget)
		{
			targetPrompt += "|R(" + Utilities::itos(comboPoints) + ")|X";
			json combos = { { "points", comboPoints } };
			SendGMCP("target.combo " + combos.dump());
		}
		else if (GetTarget() != comboPointTarget)
		{
			json combos = { { "points", 0 } };
			SendGMCP("target.combo " + combos.dump());
		}
		
		string targetLevel = Game::LevelDifficultyColor(Game::LevelDifficulty(level, GetTarget()->level));
		if (GetTarget()->IsNPC() && !Utilities::FlagIsSet(GetTarget()->flags, Character::FLAG_FRIENDLY)
			&& Game::LevelDifficulty(level, GetTarget()->level) == 5) //NPC >= 10 levels
		{
			targetLevel += "??";
			jsonvitals[0] = 0;
		}
		else
		{
			targetLevel += Utilities::itos(GetTarget()->level);
			jsonvitals[0] = GetTarget()->level;
		}

		targetPrompt += "|B<" + targetLevel + " ";
		if (GetTarget() == this || Utilities::FlagIsSet(GetTarget()->flags, FLAG_FRIENDLY))
			targetPrompt += "|G";
		else if (GetTarget()->IsPlayer())
			targetPrompt += "|C";
		else if (Utilities::FlagIsSet(GetTarget()->flags, FLAG_NEUTRAL))
			targetPrompt += "|Y";
		else
			targetPrompt += "|R";
		targetPrompt += GetTarget()->name + "|X ";

		if (!GetTarget()->IsCorpse())
		{
			//Health
			if (GetTarget()->health > 0 && GetTarget()->maxHealth > 0)
				percent = (GetTarget()->health * 100) / GetTarget()->maxHealth;
			else
				percent = 0;

			if (percent >= 75 || GetTarget()->maxHealth == 0)
				statColor = "|x";
			else if (percent >= 50)
				statColor = "|G";
			else if (percent >= 25)
				statColor = "|Y";
			else
				statColor = "|R";

			targetPrompt += statColor + Utilities::itos(percent) + "|B%h ";
			jsonvitals[1] = percent;

			//Mana
			if (GetTarget()->mana > 0 && GetTarget()->maxMana > 0)
				percent = (GetTarget()->mana * 100) / GetTarget()->maxMana;
			else
				percent = 0;

			if (percent >= 75 || GetTarget()->maxMana == 0)
				statColor = "|x";
			else if (percent >= 50)
				statColor = "|G";
			else if (percent >= 25)
				statColor = "|Y";
			else
				statColor = "|R";

			targetPrompt += statColor + Utilities::itos(percent) + "|B%m ";
			jsonvitals[2] = percent;

			//Energy
			if (GetTarget()->GetEnergy() > 0 && GetTarget()->maxEnergy > 0)
				percent = (GetTarget()->GetEnergy() * 100) / GetTarget()->maxEnergy;
			else
				percent = 0;
			targetPrompt += "|X" + Utilities::itos(percent) + "|B%e ";
			jsonvitals[3] = percent;

			//Rage
			if (GetTarget()->GetRage() > 0 && GetTarget()->GetMaxRage() > 0)
				percent = (GetTarget()->GetRage() * 100) / GetTarget()->GetMaxRage();
			else
				percent = 0;
			targetPrompt += "|X" + Utilities::itos(percent) + "|B%r>|X";
			jsonvitals[4] = percent;
		}
		else
		{
			targetPrompt += "Corpse|B>|X";
			jsonvitals[1] = 0;
			jsonvitals[2] = 0;
			jsonvitals[3] = 0;
			jsonvitals[4] = 0;
		}
		prompt += targetPrompt;
		json vitals = { { "name", GetTarget()->GetName() },{ "level", jsonvitals[0] },{ "hppercent", jsonvitals[1] },{ "mppercent", jsonvitals[2] },
		{ "enpercent", jsonvitals[3] },{ "ragepercent", jsonvitals[4] } };
		SendGMCP("target.vitals " + vitals.dump());
	}

    //Target of target (changed to display name, level, health only)
	if(GetTarget() != NULL && GetTarget()->GetTarget() != NULL)
	{
        Character * targettarget = GetTarget()->GetTarget();
		string gmcplevel = "";
		string targetLevel = Game::LevelDifficultyColor(Game::LevelDifficulty(level, targettarget->level));
        if(player == NULL && Game::LevelDifficulty(level, targettarget->level) == 5) //NPC >= 10 levels
        {
            targetLevel += "??";
			gmcplevel = "??";
        }
        else
        {
            targetLevel += Utilities::itos(targettarget->level);
			gmcplevel = Utilities::itos(targettarget->level);
        }

        //TODO: Target name coloring based on pvp/attack status
		string targetPrompt = "|B<" + targetLevel + " ";
        if(targettarget == this || Utilities::FlagIsSet(targettarget->flags, FLAG_FRIENDLY))
            targetPrompt += "|G";
		else if (targettarget->IsPlayer())
			targetPrompt += "|C";
        else if(Utilities::FlagIsSet(targettarget->flags, FLAG_NEUTRAL))
            targetPrompt += "|Y";
        else
            targetPrompt += "|R";
        targetPrompt += targettarget->name + "|X ";

        //Health
        if(targettarget->health > 0 && targettarget->maxHealth > 0)
            percent = (targettarget->health * 100)/targettarget->maxHealth;
        else
            percent = 0;

        if(percent >= 75 || targettarget->maxHealth == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%h>|X";

		prompt += targetPrompt;

		json targettargetvitals = { { "name", targettarget->GetName() },{ "level", gmcplevel },{ "hppercent", percent } };
		SendGMCP("targettarget.vitals " + targettargetvitals.dump());
	}
    
	if(HasGroup())
	{
		int firstingroup = group->FindFirstSlotInSubgroup(this);
		if(group->GetSubgroupCount(firstingroup) > 1)
		{
			prompt += "\n\r";
			int gmcpslot = 1;
			for(int i = 0; i < Group::MAX_GROUP_SIZE; i++)
			{
				Character * current_member = group->members[firstingroup + i];
				if(current_member && current_member != this)
				{
					prompt += "|B<";

					if(current_member->InCombat())
						prompt += "|R(X)|X";
					if(current_member == group->leader)
						prompt += "|Y*|X";

					prompt += "|C" + current_member->name + "|X ";

					//Health
					int percent;
					string statColor;
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

					prompt += statColor + Utilities::itos(current_member->GetHealth()) + "|X/" + Utilities::itos(current_member->GetMaxHealth());
					prompt += "|B>|X";

					json vitals = { { "name", current_member->GetName() }, { "combat", current_member->InCombat() ? 1 : 0 },
					                { "hp", current_member->GetHealth() }, { "hpmax", current_member->GetMaxHealth() },
					                { "mp", current_member->GetMana() },{ "mpmax", current_member->GetMaxMana() } };
					SendGMCP("group.vitals " + vitals.dump());
				}
			}
		}
	}
    
	//todo: We could block the entire function at the top, but then we lose a lot of GMCP data. We need to structure this differently
	if (player && player->prompt) 
	{
		prompt += "\n\r";
		Send(prompt);
	}

	if (hasQuery)
	{
		Send(queryPrompt + "\n\r");
	}
}

Character * Character::GetCharacterRoom(string name)
{
    if(name.empty())
        return NULL;

    int count = 0;
	string tempname = name;
    int number = Utilities::number_argument(tempname);

    if(!Utilities::str_cmp(tempname, "self") || !Utilities::str_cmp(tempname, "me"))
		return this;
    std::list<Character *>::iterator iter;
    for(iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if(!Utilities::IsName(tempname, (*iter)->name))
		    continue;
		if(++count == number)
		    return (*iter);
    }
    return NULL;
}

Item * Character::GetItemRoom(string name)
{
	if (name.empty())
		return NULL;

	int count = 0;
	string tempname = name;
	int number = Utilities::number_argument(tempname);

	for (auto iter = room->items.begin(); iter != room->items.end(); ++iter)
	{
		if (!Utilities::IsName(tempname, iter->first->name) && !Utilities::IsName(tempname, iter->first->keywords))
			continue;
		if (++count == number)
			return iter->first;
	}
	return NULL;
}

bool Character::IsItemInRoom(Item * i)
{
	if (i == nullptr)
		return false;

	for (auto iter = room->items.begin(); iter != room->items.end(); ++iter)
	{
		if (iter->first == i)
			return true;
	}
	return false;
}

//Find a character in this room or any adjacent room. takes an optional direction argument
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
				if (!Utilities::IsName(tempname, (*iter)->name))
					continue;
				if (++count == number)
					return (*iter);
			}
		}
		return nullptr;
	}

    for(iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if(!Utilities::IsName(tempname, (*iter)->name))
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
                if(!Utilities::IsName(tempname, (*iter)->name))
		            continue;
		        if(++count == number)
		            return (*iter);
            }
        }
    }
    return nullptr;
}

Character * Character::GetCharacterRoom(Character * target)
{
    if(!target)
        return NULL;
    if(target == this)
        return target;

    std::list<Character *>::iterator iter;
    for(iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if((*iter) == target)
            return (*iter);
    }
    return NULL;
}

//Find a character in this room or any adjacent room
Character * Character::GetCharacterAdjacentRoom(Character * target)
{
    if(!target)
        return NULL;
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
    return NULL;
}

void Character::Move(int direction)
{
    if(room == NULL)
    {
        LogFile::Log("error", "Character::Move, room == NULL");
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

    //Terrainmap
	/*if(in_room->area->terrainmap && playerData && direction != EXIT_UP && direction != EXIT_DOWN)
	{
		Message(this, name + " leaves " + directionName[direction] + ".", MSG_NOTCHAR, POS_SITTING);

		int cx = 0; int cy = 0;
		DirectionCoordChange(direction, cx, cy);

		TerrainExit * exreal;
		//if(flagset(real exit))
		//cout << direction << " " << x << " " << y << endl;
		//cout << playerData->atx + cx << " " << playerData->aty + cy << endl;
		//cout << (int)playerData->map[playerData->aty + cy][playerData->atx + cx].rgbtGreen << endl;
		if((int)playerData->map[playerData->aty + cy][playerData->atx + cx].rgbtGreen == 1
			&& (exreal = in_room->area->terrainmap->GetExit(direction, x, y)) != NULL)
		{
			//cout << "here" << endl;
			toroom = exreal->exit->toroom;
		}
		else
		{
			toroom = in_room;
		}
		CharFromRoom();
		CharToRoom(toroom, x + cx, y + cy);
		Message(this, name + " has arrived from the " + revdirectionName[direction] + ".", MSG_NOTCHAR, POS_SITTING);
		cmd_look(this, "");
		return;
	}*/

    //Regular rooms
	if(room->exits[direction] == NULL || room->exits[direction]->to == NULL)
	{
		Send("You cannot move in that direction.\n\r");
		return;
	}

	if (IsAlive())
	{
		Message(name + " leaves " + Exit::exitNames[direction] + ".", MSG_ROOM_NOTCHAR);
	}
	toroom = room->exits[direction]->to;

	//need to search the TerrainExit list
	/*if(toroom->area->terrainmap)
	{
		for(TerrainExit * te = toroom->area->terrainmap->realExits; te; te = te->next)
		{
			if(te->exit->toroom == in_room)
			{
				CharFromRoom();
				CharToRoom(toroom, te->ex_x, te->ex_y);
				break;
			}
		}
	}
	else
	{*/
        ChangeRooms(toroom);
	//}

    //TODO: move the checks for movement triggers here
	if (IsAlive())
	{
		Message(name + " has arrived from " + ((direction != Exit::DIR_UP && direction != Exit::DIR_DOWN) ? "the " : "") + Exit::reverseExitNames[direction] + ".", MSG_ROOM_NOTCHAR);
	}

    cmd_look(this, "");

    //check npc aggro
	if (IsAlive())
	{
		for (std::list<Character*>::iterator iter = room->characters.begin(); iter != room->characters.end(); ++iter)
		{
			if ((*iter)->IsNPC() && (*iter)->IsAlive() && Utilities::FlagIsSet((*iter)->flags, Character::FLAG_AGGRESSIVE) && !(*iter)->InCombat() && player && !player->IMMORTAL())
			{
				(*iter)->EnterCombat(this);
				EnterCombat(*iter);
				Send((*iter)->name + " begins attacking you!\n\r");
				(*iter)->AutoAttack(this);
			}
		}
	}
}

void Character::Sit()
{
	if (position == Position::POSITION_SITTING)
		return;

	Send("You sit down.\n\r");
	position = Position::POSITION_SITTING;
}

void Character::Stand()
{
	if (position == Position::POSITION_STANDING)
		return;

	bool removed = false;
	while (RemoveSpellAffectsByAura(false, SpellAffect::Auras::AURA_EATING)) //true if we removed a spell affect
	{
		removed = true;
	}
	if (removed)
	{
		Send("You stop eating or drinking.\n\r");
	}

	Send("You stand up.\n\r");
	position = Position::POSITION_STANDING;
}

Character * Character::LoadPlayer(std::string name, User * user)
{
    StoreQueryResult characterres = Server::sqlQueue->Read("select * from players where name='" + name + "'");
    if(characterres.empty())
        return NULL;

    Row row = *characterres.begin();

    Character * loaded = Game::GetGame()->NewCharacter(name, user);

    loaded->name = row["name"];
    loaded->gender = row["gender"];
	loaded->race = row["race"];
    loaded->title = row["title"];
    loaded->level = row["level"];
    loaded->agility = row["agility"];
    loaded->intellect = row["intellect"];
    loaded->strength = row["strength"];
    loaded->stamina = row["stamina"];
    loaded->wisdom = row["wisdom"];
	loaded->spirit = row["spirit"];
    loaded->health = row["health"];
    loaded->mana = row["mana"];

    loaded->room = Game::GetGame()->GetRoom(row["room"]); 

	if(!loaded->player)
		loaded->player = new Player(user);
    loaded->player->currentClass = Game::GetGame()->GetClass(row["class"]);
    loaded->player->password = row["password"];
    loaded->player->immlevel = row["immlevel"];
    loaded->player->experience = row["experience"];
	loaded->player->recall = row["recall"];
	if (row["ghost"])
	{
		loaded->SetGhost();
		loaded->player->corpse_room = row["corpse_room"];
		loaded->player->graveyard_room = row["room"];
		loaded->deathTime = row["ghost"];
		loaded->player->death_timer = Player::DEFAULT_DEATH_TIME;
		loaded->player->death_timer_runback = Player::DEFAULT_DEATH_TIME_RUNBACK;
	}
	loaded->player->statPoints = row["stat_points"];

	StoreQueryResult playerclassres = Server::sqlQueue->Read("SELECT * FROM player_class_data where player='" + loaded->name + "'");
	StoreQueryResult::iterator iter;
	for (iter = playerclassres.begin(); iter != playerclassres.end(); ++iter)
	{
		Row classrow = *iter;
		int id = classrow["class"];
		int level = classrow["level"];
		loaded->player->AddClass(id, level);
	}

	//Skills saved with a player no longer a thing, determine skills from classes. Still load them for the session with AddSkill
	loaded->AddClassSkills();
	//For imms also add every skill there is...
	if (loaded->player && loaded->player->IMMORTAL())
	{
		for (auto iter = Game::GetGame()->skills.begin(); iter != Game::GetGame()->skills.end(); ++iter)
		{
			loaded->AddSkill(iter->second);
		}
	}

	StoreQueryResult playerinventoryres = Server::sqlQueue->Read("SELECT * FROM player_inventory where player='" + loaded->name + "'");
	for (iter = playerinventoryres.begin(); iter != playerinventoryres.end(); ++iter)
	{
		Row invrow = *iter;
		int id = invrow["item"];
		int count = invrow["count"];
		int location = invrow["location"]; //Location means equipped, in inventory, in bank...
		switch (location)
		{
			case DB_INVENTORY_EQUIPPED:
			{
				Item * equip = Game::GetGame()->GetItem(id);
				//todo Equipping an item should be in an easy to use function OR we should really just assume the state the character was saved in is valid
				int equiploc = loaded->player->GetEquipLocation(equip);
				Item * removed = loaded->player->RemoveItemEquipped(equiploc); //remove any item already in the slot
				if (removed != NULL)
					loaded->player->AddItemInventory(removed);
				if (equiploc == Player::EQUIP_MAINHAND && equip->equipLocation == Item::EQUIP_TWOHAND)
				{   //pretty unlikely this will happen when loading a character, but just in case
					Item * offhand = loaded->player->RemoveItemEquipped(Player::EQUIP_OFFHAND); //remove any offhand when equipping a two hand
					if (offhand != NULL)
						loaded->player->AddItemInventory(offhand);
				}
				else if (equiploc == Player::EQUIP_OFFHAND) //remove a twohand when equipping an offhand
				{
					if (loaded->player->equipped[Player::EQUIP_MAINHAND] != NULL && loaded->player->equipped[Player::EQUIP_MAINHAND]->type == Item::EQUIP_TWOHAND)
					{
						loaded->player->AddItemInventory(loaded->player->RemoveItemEquipped(Player::EQUIP_MAINHAND));
					}
				}
				loaded->player->AddItemInventory(equip);
				loaded->player->EquipItemFromInventory(equip, equiploc);

				break;
			}

			case DB_INVENTORY_INVENTORY:
			{
				for (int i = 0; i < count; i++)
				{
					loaded->player->AddItemInventory(Game::GetGame()->GetItem(id));
				}
				break;
			}

			case DB_INVENTORY_BANK:
			{
				//todo
				break;
			}
		}
	}

	loaded->ResetMaxStats(); //Set maxhealth/mana/energy/rage/combos based on post equipment stats

	StoreQueryResult playerqcres = Server::sqlQueue->Read("SELECT * FROM player_completed_quests where player='" + loaded->name + "'");
	for (iter = playerqcres.begin(); iter != playerqcres.end(); ++iter)
	{
		Row qcrow = *iter;
		int id = qcrow["quest"];
		loaded->player->completedQuests.insert(id);
	}

	StoreQueryResult playerqares = Server::sqlQueue->Read("SELECT * FROM player_active_quests where player='" + loaded->name + "'");
	int i = 0;
	for (iter = playerqares.begin(); iter != playerqares.end(); ++iter)
	{
		Row qarow = *iter;
		int id = qarow["quest"];
		string objectives = (string)qarow["objectives"];
		
		Quest * q;
		if ((q = Game::GetGame()->GetQuest(id)) != NULL)
		{
			loaded->player->questLog.push_back(q);
			std::vector<int> playerObjectives;
			loaded->player->questObjectives.push_back(playerObjectives);

			int count;
			int first = 0;
			for (int last = (int)objectives.find(","); last != std::string::npos; last = (int)objectives.find(",", first))
			{
				count = Utilities::atoi(objectives.substr(first, last - first));
				loaded->player->questObjectives[i].push_back(count);
				first = last + 1;
			}
			count = Utilities::atoi(objectives.substr(first, objectives.length() - first));
			loaded->player->questObjectives[i].push_back(count);
			i++;
		}
	}

	StoreQueryResult playeraliasres = Server::sqlQueue->Read("SELECT * FROM player_alias where player='" + loaded->name + "'");
	for (iter = playeraliasres.begin(); iter != playeraliasres.end(); ++iter)
	{
		Row aliasrow = *iter;
		string word = (string)aliasrow["word"];
		string substitution = (string)aliasrow["substitution"];
		loaded->player->alias[word] = substitution;
	}
	
    return loaded;
}

void Character::Save()
{
    string sql;
    if(player) //save players
    {
        string password = Utilities::SQLFixQuotes(player->password);
        string fixtitle = Utilities::SQLFixQuotes(title);

        sql = "INSERT INTO players (name, password, immlevel, title, experience, room, level, gender, race, agility, intellect, strength, stamina, ";
        sql += "wisdom, spirit, health, mana, class, recall, ghost, corpse_room, stat_points) values ('";
        sql += name + "','" + password + "'," + Utilities::itos(player->immlevel);
        sql += ",'" + fixtitle + "'," + Utilities::itos(player->experience) + "," + Utilities::itos(room->id);
        sql += "," + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intellect) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(stamina) + "," + Utilities::itos(wisdom) + "," + Utilities::itos(spirit) + ",";
        sql += Utilities::itos(health) + "," + Utilities::itos(mana);
		sql += "," + Utilities::itos(player->currentClass->id) + "," + Utilities::itos(player->recall) + ", ";
		if (IsGhost() || IsCorpse())
		{
			sql += Utilities::dtos(deathTime, 0) + "," + Utilities::itos(player->corpse_room) + ",";
		}
		else
		{
			sql += "0,0,";
		}
		sql += Utilities::itos(player->statPoints) + ")";

        sql += " ON DUPLICATE KEY UPDATE name=VALUES(name), password=VALUES(password), immlevel=VALUES(immlevel), title=VALUES(title), ";
        sql += "experience=VALUES(experience), room=VALUES(room), level=VALUES(level), gender=VALUES(gender), race=VALUES(race), agility=VALUES(agility), ";
        sql += "intellect=VALUES(intellect), strength=VALUES(strength), stamina=VALUES(stamina), wisdom=VALUES(wisdom), spirit=VALUES(spirit), ";
        sql += "health=VALUES(health), mana=VALUES(mana), ";
        sql += "class=VALUES(class), ";
        sql += "recall=VALUES(recall), ghost=VALUES(ghost), corpse_room=VALUES(corpse_room),stat_points=VALUES(stat_points)";

		//player_completed_quests
		std::set<int>::iterator questiter;
		for (questiter = player->completedQuests.begin(); questiter != player->completedQuests.end(); ++questiter)
		{
			string qcsql = "INSERT IGNORE INTO player_completed_quests (player, quest) values ";
			qcsql += "('" + name + "', " + Utilities::itos(*questiter) + ")";
			Server::sqlQueue->Write(qcsql);
		}
		
		//player_active_quests
		Server::sqlQueue->Write("DELETE FROM player_active_quests where player='" + name + "'");
		for (int i = 0; i < (int)player->questLog.size(); i++)
		{
			string qasql = "INSERT INTO player_active_quests (player, quest, objectives) values ('" + name + "',";
			qasql += Utilities::itos(player->questLog[i]->id) + ",'";
			for (int j = 0; j < (int)player->questObjectives[i].size();)
			{
				qasql += Utilities::itos(player->questObjectives[i][j]);
				j++;
				if (j < (int)player->questObjectives[i].size())
					qasql += ",";
			}
			qasql += "')";
			Server::sqlQueue->Write(qasql);
		}

		//player_inventory
		Server::sqlQueue->Write("DELETE FROM player_inventory where player='" + name + "'");
		for (int i = 0; i < (int)player->equipped.size(); i++)
		{
			if (player->equipped[i] != NULL)
			{
				string equippedsql = "INSERT INTO player_inventory (player, item, count, location) values ('" + name + "',";
				equippedsql += Utilities::itos(player->equipped[i]->id) + ",1," + Utilities::itos(DB_INVENTORY_EQUIPPED) + ")";
				Server::sqlQueue->Write(equippedsql);
			}
		}
		for (auto inviter = player->inventory.begin(); inviter != player->inventory.end(); ++inviter)
		{
			string equippedsql = "INSERT INTO player_inventory (player, item, count, location) values ('" + name + "',";
			equippedsql += Utilities::itos(inviter->first->id) + "," + Utilities::itos(inviter->second) + "," + Utilities::itos(DB_INVENTORY_INVENTORY) + ")";
			Server::sqlQueue->Write(equippedsql);
		}
		
		//player_class_data
		Server::sqlQueue->Write("DELETE FROM player_class_data where player='" + name + "'");
		std::list<Player::ClassData>::iterator classiter;
		for (classiter = player->classList.begin(); classiter != player->classList.end(); ++classiter)
		{
			string classsql = "INSERT INTO player_class_data (player, class, level) values ('" + name + "',";
			classsql += Utilities::itos((*classiter).id) + "," + Utilities::itos((*classiter).level) + ")";
			Server::sqlQueue->Write(classsql);
		}

		//player aliases
		Server::sqlQueue->Write("DELETE FROM player_alias where player='" + name + "'");
		for (auto aliasiter = player->alias.begin(); aliasiter != player->alias.end(); ++aliasiter)
		{
			string aliassql = "INSERT INTO player_alias (player, word, substitution) values ('" + name + "','";
			aliassql += Utilities::SQLFixQuotes(aliasiter->first) + "','" + Utilities::SQLFixQuotes(aliasiter->second) + "')";
			Server::sqlQueue->Write(aliassql);
		}

        player->saved = (int)Game::GetGame()->currentTime;
    }
    else //save npcs
    {
        string fixtitle = Utilities::SQLFixQuotes(title);

        sql = "INSERT INTO npcs (id, name, keywords, level, gender, race, agility, intellect, strength, stamina, ";
        sql += "wisdom, spirit, health, mana, energy, rage, title, attack_speed, damage_low, damage_high, flags, speechtext) values (";
        sql += Utilities::itos(id) + ", '";
        sql += name + "', '" + keywords + "', " + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intellect) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(stamina) + "," + Utilities::itos(wisdom) + "," + Utilities::itos(spirit) + ",";
        sql += Utilities::itos(maxHealth) + "," + Utilities::itos(maxMana) + "," + Utilities::itos(maxEnergy) + "," + Utilities::itos(maxRage);
        sql += ", '" + fixtitle + "', " + Utilities::dtos(npcAttackSpeed, 2) + ", " + Utilities::itos(npcDamageLow) + ", ";
        sql += Utilities::itos(npcDamageHigh) + ",'";

		std::vector<int>::iterator flagiter;
		for (flagiter = flags.begin(); flagiter != flags.end(); ++flagiter)
		{
			sql += Utilities::itos((*flagiter)) + ";";
		}
		sql += "','" + Utilities::SQLFixQuotes(speechText) + "')";

        sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), level=VALUES(level), gender=VALUES(gender), race=VALUES(race), agility=VALUES(agility), ";
        sql += "intellect=VALUES(intellect), strength=VALUES(strength), stamina=VALUES(stamina), wisdom=VALUES(wisdom), ";
        sql += "health=VALUES(health), mana=VALUES(mana), energy=VALUES(energy), rage=VALUES(rage),";
        sql += "title=VALUES(title), attack_speed=VALUES(attack_speed), damage_low=VALUES(damage_low), ";
        sql += "damage_high=VALUES(damage_high), flags=VALUES(flags), speechtext=VALUES(speechtext)";

		//save skills
		Server::sqlQueue->Write("DELETE FROM npc_skills where npc=" + Utilities::itos(id));
		std::map<std::string, Skill *>::iterator skilliter;
		for (skilliter = knownSkills.begin(); skilliter != knownSkills.end(); ++skilliter)
		{
			string skillsql = "INSERT INTO npc_skills (npc, id) values ";
			skillsql += "(" + Utilities::itos(id) + ", " + Utilities::itos((*skilliter).second->id) + ")";
			skillsql += " ON DUPLICATE KEY UPDATE npc=VALUES(npc), id=VALUES(id)";
			Server::sqlQueue->Write(skillsql);
		}

		//save drops
		Server::sqlQueue->Write("DELETE FROM npc_drops where npc=" + Utilities::itos(id));
		std::list<DropData>::iterator dropsiter;
		for(dropsiter = drops.begin(); dropsiter != drops.end(); ++dropsiter)
		{
			string dropsql = "INSERT INTO npc_drops (npc, items, percent) values ";
			dropsql += "(" + Utilities::itos(id) + ", '";
			for(int i = 0; i < (int)(*dropsiter).id.size(); i++)
			{
				dropsql += Utilities::itos((*dropsiter).id[i]) + ";";
			}
			
			
			dropsql += "', " + Utilities::itos((*dropsiter).percent) + ")";
			dropsql += " ON DUPLICATE KEY UPDATE npc=VALUES(npc), items=VALUES(items), percent=VALUES(percent)";
			Server::sqlQueue->Write(dropsql);

		}

        //save Triggers
        std::map<int, Trigger>::iterator trigiter = triggers.begin();
        while(trigiter != triggers.end())
        {
            Trigger * t = &((*trigiter).second);
            ++trigiter;
            if(t->removeme)
            {
                string triggersql = "DELETE FROM triggers where triggers.parent_type="+Utilities::itos(Trigger::PARENT_NPC)
                                    +" and triggers.parent_id="+Utilities::itos(id)+" and triggers.id="+Utilities::itos(t->id);
                Server::sqlQueue->Write(triggersql);
                triggers.erase(t->id);
                t = NULL;
            }
            else
            {
                string triggersql = "INSERT INTO triggers (parent_id, id, parent_type, type, argument, script, function) values ";
                triggersql += "(" + Utilities::itos(id) + ", " + Utilities::itos(t->id) + ", " + Utilities::itos(Trigger::PARENT_NPC) + ", ";
                triggersql += Utilities::itos(t->GetType()) + ", '" + Utilities::SQLFixQuotes(t->GetArgument()) + "', '";
                triggersql += Utilities::SQLFixQuotes(t->GetScript()) + "', '" + Utilities::SQLFixQuotes(t->GetFunction()) + "')";

                triggersql += " ON DUPLICATE KEY UPDATE parent_id=VALUES(parent_id), id=VALUES(id), parent_type=VALUES(parent_type), ";
                triggersql += "type=VALUES(type), argument=VALUES(argument), script=VALUES(script), function=VALUES(function)";

                Server::sqlQueue->Write(triggersql);
            }
        }
    }
    Server::sqlQueue->Write(sql);
}

void Character::SetLevel(int newlevel)
{
    if(level == newlevel || newlevel > Game::MAX_LEVEL || newlevel < 1)
        return;

	Send("|W***You have reached level " + Utilities::itos(newlevel) + "!***|X\n\r");

    if(player)
    {
        player->SetClassLevel(player->currentClass->id, 
                              Utilities::MAX(0, player->GetClassLevel(player->currentClass->id) + (newlevel - level)));
	
		player->statPoints += Player::STATS_PER_LEVEL;
		Send("|WYou gain " + Utilities::itos(Player::STATS_PER_LEVEL) + " attribute points. Assign attribute points with the \"train\" command.|X\n\r");
		
		std::list<Class::SkillData>::iterator newskills;
		for (newskills = player->currentClass->classSkills.begin(); newskills != player->currentClass->classSkills.end(); newskills++)
		{
			if (newskills->level == player->GetClassLevel(player->currentClass->id) && !HasSkill(newskills->skill)) //Found a new skill to add
			{
				AddSkill(newskills->skill);
				Send("|WYou have learned the skill \"" + newskills->skill->long_name + "\"|X\n\r");
			}
		}
	}
	level = newlevel;
	SetHealth(maxHealth);
	SetMana(maxMana);
	SetEnergy(maxEnergy);
}

int Character::GetLevel()
{
    return level;
}

string Character::GetName()
{
    return name;
}

Player * Character::GetPlayer()
{
	return player;
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
				return NULL;
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
        sa->casterName = caster->name;
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

    if(sk != NULL)
    {
        string func = sk->function_name + "_apply";
        try
        {
			Server::lua[func.c_str()](caster, this, sa);
            //luabind::call_function<void>(Server::luaState, func.c_str(), caster, this, sa);
        }
		catch(const std::runtime_error & e)
		{
			LogFile::Log("error", e.what());
			/*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
			if(logstring != NULL)
				LogFile::Log("error", logstring);*/
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

void Character::SaveCooldowns()
{
    string sql = "DELETE FROM player_cooldowns WHERE player='" + name + "';";
    Server::sqlQueue->Write(sql);

    std::map<string, Skill *>::iterator iter;
    for(iter = knownSkills.begin(); iter != knownSkills.end(); ++iter)
    {
        std::map<int, double>::iterator iter2;
        iter2 = cooldowns.find((*iter).second->id);
        if(iter2 == cooldowns.end()) //not on cooldown
            continue;
        if((*iter2).second <= Game::currentTime) //not on cooldown
            continue;
        
        sql = "INSERT INTO player_cooldowns (player_cooldowns.player, player_cooldowns.skill, player_cooldowns.timestamp) VALUES ('";
        sql += name + "', " + Utilities::itos((*iter).second->id) + ", " + Utilities::dtos((*iter2).second, 2) + ");";
        Server::sqlQueue->Write(sql);
    }
}

void Character::LoadCooldowns()
{
	StoreQueryResult cooldownres;
	try {
		cooldownres = Server::sqlQueue->Read("SELECT * FROM player_cooldowns WHERE player='" + name + "';");
		if (!cooldownres || cooldownres.empty())
			return;
	}
	catch (std::exception e)
	{
		LogFile::Log("error", e.what());
		return;
	}

    Row row;
    StoreQueryResult::iterator i;
    for(i = cooldownres.begin(); i != cooldownres.end(); i++)
    {
        row = *i;
        Skill * sk = Game::GetGame()->GetSkill(row["skill"]);
        if(sk != NULL)
        {
            cooldowns[sk->id] = row["timestamp"];
        }
    }

    string sql = "DELETE FROM player_cooldowns WHERE player='" + name + "';";
    Server::sqlQueue->Write(sql);
}

void Character::SaveSpellAffects()
{
    if(!player)
        return;

    string deletesql = "DELETE FROM player_spell_affects WHERE player = '" + name + "';";
    Server::sqlQueue->Write(deletesql);

    std::list<SpellAffect*>::iterator iter;
    for(iter = buffs.begin(); iter != buffs.end(); ++iter)
    {
        (*iter)->Save(name);
    }
    for(iter = debuffs.begin(); iter != debuffs.end(); ++iter)
    {
        (*iter)->Save(name);
    }
}

void Character::LoadSpellAffects()
{
    if(!player)
        return;

    SpellAffect::Load(this);
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

    if(target == NULL || target == victim)
    {
        SetTarget(victim);
        meleeActive = true;
    }
    //if(victim->target == NULL || victim->target == this)
    if((victim->IsNPC() && victim->meleeActive == false) || victim->target == NULL || victim->target == this)
    {
        victim->SetTarget(this);
        victim->meleeActive = true;
    }
    combat = true;
    victim->combat = true;

    if(player)
        player->lastCombatAction = Game::currentTime;
    if(victim->player)
        victim->player->lastCombatAction = Game::currentTime;

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
		int first_slot = group->FindFirstSlotInSubgroup(this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (group->members[i] != nullptr && group->members[i] != this)
			{
				group->members[i]->SendGMCP("group.vitals " + combat.dump());
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
    if(player)
        player->lastCombatAction = Game::currentTime;

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
		int first_slot = group->FindFirstSlotInSubgroup(this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (group->members[i] != nullptr && group->members[i] != this)
			{
				group->members[i]->SendGMCP("group.vitals " + combat.dump());
			}
		}
	}
}

void Character::ExitCombat()
{
    combat = false;
    meleeActive = false;
    RemoveThreat(NULL, true);
    movementSpeed = Character::NORMAL_MOVE_SPEED;
	json combat = { { "combat", 0 } };
	SendGMCP("char.vitals " + combat.dump());

	SendTargetSubscriberGMCP("target.vitals " + combat.dump());

	if (HasGroup())
	{
		combat = { { "name", GetName() },{ "combat", 0 } };
		int first_slot = group->FindFirstSlotInSubgroup(this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (group->members[i] != nullptr && group->members[i] != this)
			{
				group->members[i]->SendGMCP("group.vitals " + combat.dump());
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
    if(victim == NULL)
        return;

    //unarmed
    double weaponSpeed_main = 2.0; 
    double weaponSpeed_off = 2.0;
    int damage_main = 1;
    int damage_off = 1;
    bool attack_mh = true;
    bool attack_oh = false; //weapon required for offhand attack (no unarmed)

	//NPC autoattack
    if(player == NULL && lastAutoAttack_main + npcAttackSpeed <= Game::currentTime)
    {
        if(victim->target == NULL) //Force a target on our victim
        {
            victim->SetTarget(this);
            //Have the victim retaliate when attacked with no target set
            victim->meleeActive = true;
        }
        if(victim->player)
            victim->player->lastCombatAction = Game::currentTime;
        lastAutoAttack_main = Game::currentTime;

        int damage = npcDamageLow;
        if(npcDamageHigh != npcDamageLow)
            damage = (Server::rand() % (npcDamageHigh - npcDamageLow)) + npcDamageLow;
		if (victim->player)
			victim->GenerateRageOnTakeDamage(damage);

		victim->Send("|Y" + name + " hits you for " + Utilities::itos(damage) + " damage.|X\n\r");
		//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

        OneHit(victim, damage); //TODO fancy damage calculations, block miss hit crit 
        //victim may be invalid here if it was killed!
    }
    else if(player != NULL) //Player autoattack
    {
		weaponSpeed_main = GetMainhandWeaponSpeed();
		damage_main = GetMainhandDamageRandomHit();
		if(damage_main == 0)
			attack_mh = false; //no mainhand attack if we're holding a non weapon

		weaponSpeed_off = GetOffhandWeaponSpeed();
		damage_off = GetOffhandDamageRandomHit();
		if(damage_off == 0)
			attack_oh = false;

        if(attack_mh && lastAutoAttack_main + weaponSpeed_main <= Game::currentTime)
        {
			damage_main += (int)ceil((strength * Character::STRENGTH_DAMAGE_MODIFIER) / weaponSpeed_main);
            if(victim->target == NULL) //Force a target on our victim
            {     
                victim->SetTarget(this);
                //Have the victim retaliate when attacked with no target set
                victim->meleeActive = true;
            }
            player->lastCombatAction = Game::currentTime;
            if(victim->player)
                victim->player->lastCombatAction = Game::currentTime;
            lastAutoAttack_main = Game::currentTime;
			GenerateRageOnAttack(damage_main, weaponSpeed_main, true, false);

			string tapcolor = "|G";
			Character * thetap = victim->GetTap();
			if (thetap != nullptr && thetap != this && !thetap->InSameGroup(this))
			{
				tapcolor = "|D";
			}
			Send(tapcolor + "You hit " + victim->name + " for " + Utilities::itos(damage_main) + " damage.|X\n\r");
			victim->Send("|Y" + name + " hits you for " + Utilities::itos(damage_main) + " damage.|X\n\r");
			//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage_main) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

            OneHit(victim, damage_main); //TODO fancy damage calculations, block miss hit crit 
            //victim may be invalid if it was killed!
        }
        if(attack_oh && lastAutoAttack_off + weaponSpeed_off <= Game::currentTime)
        {
			damage_off += (int)ceil((strength * Character::STRENGTH_DAMAGE_MODIFIER) / weaponSpeed_off);
            if(victim->target == NULL) //Force a target on our victim
            {     
                victim->SetTarget(this);
                //Have the victim retaliate when attacked with no target set
                victim->meleeActive = true;
            }
            player->lastCombatAction = Game::currentTime;
            if(victim->player)
                victim->player->lastCombatAction = Game::currentTime;
            lastAutoAttack_off = Game::currentTime;
			GenerateRageOnAttack(damage_off, weaponSpeed_off, false, false);

			string tapcolor = "|G";
			Character * thetap = victim->GetTap();
			if (thetap != nullptr && thetap != this && !thetap->InSameGroup(this))
			{
				tapcolor = "|D";
			}
			Send(tapcolor + "You hit " + victim->name + " for " + Utilities::itos(damage_off) + " damage.|X\n\r");
			victim->Send("|Y" + name + " hits you for " + Utilities::itos(damage_off) + " damage.|X\n\r");
			//Message("|G" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage_off) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);

            OneHit(victim, damage_off); //TODO fancy damage calculations, block miss hit crit 
            //victim may be invalid if it was killed!
        }
    }
}

void Character::OneHit(Character * victim, int damage) 
{
    if(victim == NULL)
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
	if (victim == NULL)
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

double Character::GetMainhandWeaponSpeed()
{
	if (player == NULL)	//These checks probably mean this should be a CPlayer function
		return 0;

	if (player->equipped[Player::EQUIP_MAINHAND] != NULL
		&& player->equipped[Player::EQUIP_MAINHAND]->speed > 0)
	{
		return player->equipped[Player::EQUIP_MAINHAND]->speed;
	}
	return 0;
}

double Character::GetOffhandWeaponSpeed()
{
	if (player == NULL)	//These checks probably mean this should be a CPlayer function
		return 0;

	if (player->equipped[Player::EQUIP_OFFHAND] != NULL
		&& player->equipped[Player::EQUIP_OFFHAND]->speed > 0)
	{
		return player->equipped[Player::EQUIP_OFFHAND]->speed;
	}
	return 0;
}

int Character::GetMainhandDamageRandomHit()
{
	if (player == NULL)
		return 1;

	int damage = 1;
	if (player->equipped[Player::EQUIP_MAINHAND] != NULL
		&& player->equipped[Player::EQUIP_MAINHAND]->speed > 0
		&& player->equipped[Player::EQUIP_MAINHAND]->damageHigh > 0)
	{
		int high = player->equipped[Player::EQUIP_MAINHAND]->damageHigh;
		int low = player->equipped[Player::EQUIP_MAINHAND]->damageLow;
		if (high != low)
			damage = (Server::rand() % (high+1 - low)) + low;
		else
			damage = low;
	}
	else
	{
		return 0;
	}
	return damage;
}

double Character::GetMainhandDamagePerSecond()
{
	if (player == NULL)
		return 1;

	double dps = 1;
	if (player->equipped[Player::EQUIP_MAINHAND] != NULL
		&& player->equipped[Player::EQUIP_MAINHAND]->speed > 0 
		&& player->equipped[Player::EQUIP_MAINHAND]->damageHigh > 0)
	{
		double weaponSpeed_main = player->equipped[Player::EQUIP_MAINHAND]->speed;
		int high = player->equipped[Player::EQUIP_MAINHAND]->damageHigh;
		int low = player->equipped[Player::EQUIP_MAINHAND]->damageLow;
		dps = ((low + high) / 2.0) / weaponSpeed_main;
	}
	else
	{
		return 0;
	}
	return dps;
}

int Character::GetOffhandDamageRandomHit()
{
	if (player == NULL)
		return 1;

	int damage = 1;
	if (player->equipped[Player::EQUIP_OFFHAND] != NULL
		&& player->equipped[Player::EQUIP_OFFHAND]->speed > 0
		&& player->equipped[Player::EQUIP_OFFHAND]->damageHigh > 0)
	{
		int high = player->equipped[Player::EQUIP_OFFHAND]->damageHigh;
		int low = player->equipped[Player::EQUIP_OFFHAND]->damageLow;
		if (high != low)
			damage = (Server::rand() % (high + 1 - low)) + low;
		else
			damage = low;
	}
	else
	{
		return 0;
	}
	return damage;
}

double Character::GetOffhandDamagePerSecond()
{
	if (player == NULL)
		return 1;

	double dps = 1;
	if (player->equipped[Player::EQUIP_OFFHAND] != NULL
		&& player->equipped[Player::EQUIP_OFFHAND]->speed > 0
		&& player->equipped[Player::EQUIP_OFFHAND]->damageHigh > 0)
	{
		double weaponSpeed_main = player->equipped[Player::EQUIP_OFFHAND]->speed;
		int high = player->equipped[Player::EQUIP_OFFHAND]->damageHigh;
		int low = player->equipped[Player::EQUIP_OFFHAND]->damageLow;
		dps = ((low + high) / 2.0) / weaponSpeed_main;
	}
	else
	{
		return 0;
	}
	return dps;
}

int Character::GetIntellect()
{
	return intellect;
}

int Character::GetStrength()
{
	return strength;
}

bool Character::IsFighting(Character * target)
{
    if(this == target)
        return false; //never fighting ourself
    if(this->target == target && meleeActive)
        return true;
    return false;
}

int Character::GetHealth()
{
    return health;
}

int Character::GetMaxHealth()
{
	return maxHealth;
}

int Character::GetMana()
{
    return mana;
}

int Character::GetMaxMana()
{
	return maxMana;
}

int Character::GetEnergy()
{
	return energy;
}

int Character::GetMaxEnergy()
{
	return maxEnergy;
}

int Character::GetRage()
{
	return rage;
}

int Character::GetMaxRage()
{
	return maxRage;
}

void Character::GenerateRageOnAttack(int damage, double weapon_speed, bool mainhand, bool wascrit)
{
	//Rage generation stuff, inspired by wow, for now
	double conversionvalue = (.008 * level * level) + 4;
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
	double conversionvalue = (.008 * level * level) + 4;
	AdjustRage(this, (int)ceil((5 * damage) / (2 * conversionvalue)));
}

int Character::GetComboPoints()
{
	return comboPoints;
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
		if (threat->ch->comboPointTarget && threat->ch->comboPointTarget == this)
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
		ExitCombat();			//Removes our threat list
		SetQuery("Release spirit? ('release') ", NULL, releaseSpiritQuery);
	}
	else if (IsNPC()) //NPC killed, figure out by who...
	{
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
			tap->HandleNPCKillRewards(this);
			this->validLooters.push_back(tap);

			if (tap->HasGroup())
			{
				for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
				{
					Character * group_member = tap->group->members[i];
					if (group_member != nullptr && group_member != tap && FindDistance(this->room, group_member->room, 5) != -1)
					{
						group_member->HandleNPCKillRewards(this);
						this->validLooters.push_back(group_member);
					}
				}
			}
		}
		ExitCombat(); //Removes NPC's threat list
	}
	SetCorpse();
}

void Character::HandleNPCKillRewards(Character * killed)
{
	int exp = Game::CalculateExperience(this, killed);
	Send("|BYou have gained |Y" + Utilities::itos(exp) + "|B experience.|X\n\r");
	ApplyExperience(exp);
	player->QuestCompleteObjective(Quest::OBJECTIVE_KILLNPC, (void*)killed);

	for (auto dropiter = killed->drops.begin(); dropiter != killed->drops.end(); ++dropiter)
	{
		if (Server::rand() % 100 <= (*dropiter).percent && (*dropiter).id.size() > 0)
		{
			int which = Server::rand() % ((int)(*dropiter).id.size());
			Item * drop = Game::GetGame()->GetItem((*dropiter).id[which]);

			if (drop->quest && !player->ShouldDropQuestItem(drop))
			{
				continue;
			}
			Send("You receive loot: " + (string)Item::quality_strings[drop->quality] + drop->name + "|X.\n\r");
			Message(name + " receives loot: " + Item::quality_strings[drop->quality] + drop->name + "|X.",
				Character::MSG_ROOM_NOTCHAR);
			player->AddItemInventory(drop);
		}
	}
}

void Character::MakeCorpse()
{
	if (!player)
		return;

	Item * thecorpse = new Item("The corpse of " + name, 0);
	thecorpse->keywords = "corpse " + name;
	Utilities::FlagSet(thecorpse->flags, Item::FLAG_ROOMONLY);
	room->AddItem(thecorpse);
}

void Character::RemoveCorpse()
{
	if (!player)
		return;

	if (player->corpse_room == 0)
	{
		LogFile::Log("error", "Character::RemoveCorpse() with bad player->corpse_room");
		return;
	}

	Room * corpseroom = Game::GetGame()->GetRoom(player->corpse_room);

	for (auto itemiter = corpseroom->items.begin(); itemiter != corpseroom->items.end(); itemiter++)
	{
		if (itemiter->first->keywords.find(name) != std::string::npos)
		{
			corpseroom->RemoveItem(itemiter->first);

			if (room && room == corpseroom)
			{
				Message(name + "'s corpse crumbles into dust.", Character::MessageType::MSG_ROOM_NOTCHAR);
			}
			else
			{
				corpseroom->Message(name + "'s corpse crumbles into dust.");
			}

			break;
		}
	}
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
			if (comboPoints >= amount)
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
	if (source == NULL)
	{
		//a possibility
		source = this; //self damage!?
	}
	SetHealth(GetHealth() + amount);

	if (GetHealth() == 0)
	{
		Message("|R" + name + " has been slain!|X", Character::MSG_ROOM_NOTCHARVICT, source);
		source->Send("|R" + name + " has been slain!|X\n\r");
		Send("|RYou have been slain!|X\n\r");

		OnDeath(); //The source of damage shouldn't matter except where cases of "killing blow" matters (none so far?)
	}
}

void Character::SetHealth(int amount)
{
	if (amount <= 0)
	{
		if (player && player->IMMORTAL())
			health = 1;
		else
			health = 0;
	}
	else if (amount > maxHealth)
	{
		health = maxHealth;
	}
	else
	{
		health = amount;
	}

	json vitals = { { "hp", health } };
	SendGMCP("char.vitals " + vitals.dump());
	int percent = 0;
	if (maxHealth > 0)
		percent = (health * 100) / maxHealth;
	vitals = { { "hppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	SendTargetTargetSubscriberGMCP("targettarget.vitals " + vitals.dump());

	if (HasGroup())
	{
		int first_slot = group->FindFirstSlotInSubgroup(this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (group->members[i] != nullptr && group->members[i] != this)
			{
				vitals = { { "name", GetName() }, { "hp", GetHealth() } };
				group->members[i]->SendGMCP("group.vitals " + vitals.dump());
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
	else if (amount > maxMana)
	{
		mana = maxMana;
	}
	else
	{
		mana = amount;
	}

	json vitals = { { "mp", mana } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxMana > 0)
		percent = (mana * 100) / maxMana;
	vitals = { { "mppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());

	if (HasGroup())
	{
		int first_slot = group->FindFirstSlotInSubgroup(this);
		for (int i = first_slot; i < first_slot + Group::MAX_GROUP_SIZE; i++)
		{
			if (group->members[i] != nullptr && group->members[i] != this)
			{
				vitals = { { "name", GetName() },{ "mp", GetMana() } };
				group->members[i]->SendGMCP("group.vitals " + vitals.dump());
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
	else if (amount > maxEnergy)
	{
		energy = maxEnergy;
	}
	else
	{
		energy = amount;
	}
	json vitals = { { "en", energy } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxEnergy > 0)
		percent = (energy * 100) / maxEnergy;
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
	else if (amount > maxRage)
	{
		rage = maxRage;
	}
	else
	{
		rage = amount;
	}
	json vitals = { { "rage", rage } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxRage > 0)
		percent = (rage * 100) / maxRage;
	vitals = { { "ragepercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}

void Character::SetMaxHealth(int amount)
{
	maxHealth = amount;
	if (IsPlayer())
	{
		json vitals = { { "hpmax", maxHealth } };
		SendGMCP("char.vitals " + vitals.dump());

		int percent = 0;
		if (maxHealth > 0)
			percent = (health * 100) / maxHealth;
		vitals = { { "hppercent", percent } };
		SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	}
}

void Character::SetMaxMana(int amount)
{
	maxMana = amount;
	if (IsPlayer())
	{
		json vitals = { { "mpmax", maxMana } };
		SendGMCP("char.vitals " + vitals.dump());

		int percent = 0;
		if (maxMana > 0)
			percent = (mana * 100) / maxMana;
		vitals = { { "mppercent", percent } };
		SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	}
}

void Character::SetMaxEnergy(int amount)
{
	maxEnergy = amount;
	if (IsPlayer())
	{
		json vitals = { { "enmax", maxEnergy } };
		SendGMCP("char.vitals " + vitals.dump());

		int percent = 0;
		if (maxEnergy > 0)
			percent = (energy * 100) / maxEnergy;
		vitals = { { "enpercent", percent } };
		SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	}
}

void Character::SetMaxRage(int amount)
{
	maxRage = amount;
	if (IsPlayer())
	{
		json vitals = { { "ragemax", maxRage } };
		SendGMCP("char.vitals " + vitals.dump());

		int percent = 0;
		if (maxRage > 0)
			percent = (rage * 100) / maxRage;
		vitals = { { "ragepercent", percent } };
		SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	}
}

void Character::SetComboPoints(int howmany)
{
	if (howmany < 0 || howmany > maxComboPoints)
		return;
	comboPoints = howmany;
}

void Character::GenerateComboPoint(Character * target)
{
	if (target == nullptr)
		return;

	if (!target->IsAlive())
	{
		LogFile::Log("error", "GenerateComboPoint on !IsAlive target");
		return;
	}

	if (target != comboPointTarget)	//Changing our combo target
	{
		if (comboPointTarget != nullptr)	//If we had a previous combo target...
		{
			comboPointTarget->RemoveSubscriber(this);
		}
		comboPointTarget = target;
		comboPoints = 1;
		target->AddSubscriber(this);
	}
	else if(comboPoints < maxComboPoints)
	{
		comboPoints++;
	}
}

int Character::SpendComboPoints(Character * target)
{
	if (target == nullptr || comboPointTarget == nullptr || target != comboPointTarget || comboPoints <= 0)
		return 0;

	comboPointTarget->RemoveSubscriber(this);
	comboPointTarget = nullptr;
	int combos = comboPoints;
	comboPoints = 0;
	return combos;
}

void Character::ClearComboPointTarget()
{
	if (comboPointTarget == nullptr)
		return;

	comboPointTarget->RemoveSubscriber(this);
	comboPointTarget = nullptr;
	comboPoints = 0;
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

std::string Character::HisHer()
{
    return (gender == 1 ? "his" : "her");
}

std::string Character::HimHer()
{
	return (gender == 1 ? "him" : "her");
}

std::string Character::HisHers()
{
	return (gender == 1 ? "his" : "hers");
}

bool Character::CancelActiveDelay()
{
	if (delay_active)
	{
		if (delayData.charTarget && delayData.caster && delayData.charTarget != delayData.caster)
		{
			delayData.charTarget->RemoveSubscriber(delayData.caster);
		}
		if (global > Game::currentTime) //If we're canceling a cast and gcd is active, reset it
		{
			global = Game::currentTime;
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
	else if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_FRIENDLY))
	{
		return "|G";
	}
	else if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_NEUTRAL))
	{
		return "|Y";
	}
	else// if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_AGGRESSIVE))
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
	else if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_FRIENDLY))
	{
		return "|g";
	}
	else if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_NEUTRAL))
	{
		return "|y";
	}
	else// if (Utilities::FlagIsSet(target->flags, Character::Flags::FLAG_AGGRESSIVE))
	{
		return "|r";
	}
}

bool Character::CanAttack(Character * victim)
{
	if ((victim->IsNPC() && Utilities::FlagIsSet(victim->flags, Character::FLAG_FRIENDLY))
		|| (!victim->IsNPC() && room->pvp == 0)
		|| (!victim->IsAlive()))
	{
		return false;
	}
	return true;
}

bool Character::CanMove()
{
    if(player && player->IMMORTAL())
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

void Character::AddSkill(Skill * newskill)
{
    if(newskill == NULL)
        return;
    knownSkills[newskill->name] = newskill;
}

void Character::RemoveSkill(Skill * sk)
{
    std::map<string, Skill*>::iterator iter;

    iter = knownSkills.find(sk->name);
    if(iter != knownSkills.end() && (*iter).second->id == sk->id)
    {
        knownSkills.erase(iter);
    }
}

void Character::RemoveSkill(string name)
{
    std::map<string, Skill*>::iterator iter;

    iter = knownSkills.find(name);
    if(iter != knownSkills.end())
    {
        knownSkills.erase(iter);
    }
}

Skill * Character::GetSkillShortName(string name)
{
    std::map<string, Skill*>::iterator iter;

    name = Utilities::ToLower(name);
    iter = knownSkills.find(name);
    if(iter != knownSkills.end())
    {
        return (*iter).second;
    }
    return NULL;
}

bool Character::HasSkill(Skill * sk)
{
    std::map<string, Skill*>::iterator iter;

    iter = knownSkills.find(sk->name);
    if(iter != knownSkills.end() && (*iter).second->id == sk->id)
    {
        return true;
    }
    return false;
}

bool Character::HasSkillByName(string name) //Not guaranteed to be the same skill id, just the same name
{
    std::map<string, Skill*>::iterator iter;

    iter = knownSkills.find(name);
    if(iter != knownSkills.end())
    {
        return true;
    }
    return false;
}

void Character::StartGlobalCooldown()
{
	global = Game::currentTime + 1.5;
}

void Character::SetCooldown(Skill * sk, double length) //USE LENGTH -1 TO USE SKILL->COOLDOWN
{
    if(length == 0)
        return;

    if(sk != NULL)
    {
        if(length < 0) //if length default argument == -1, use sk->cooldown
            length = sk->cooldown;
        cooldowns[sk->id] = Game::currentTime + length;
    }
}

double Character::GetCooldownRemaining(Skill * sk)
{
    if(sk == NULL)
        return 0;

    if(player && player->IMMORTAL())
        return 0;

	//Global cooldown in progress
	double remaining_global = 0;
	if (!Utilities::FlagIsSet(sk->flags, Skill::FLAG_GCDIMMUNE) && this->global > Game::currentTime)
	{
		remaining_global = this->global - Game::currentTime;
	}

	double remaining_cd = 0;
    auto iter = cooldowns.find(sk->id);
	if (iter != cooldowns.end() && (*iter).second > Game::currentTime)
		remaining_cd = (*iter).second - Game::currentTime;

	if(remaining_global > remaining_cd)
		return remaining_global;
	return remaining_cd;
}

bool Character::IsNPC()
{
    return (player == NULL);
}

bool Character::IsPlayer()
{
	return (player != NULL);
}

//room == NULL to remove from room only
bool Character::ChangeRooms(Room * toroom)
{
    if(room != NULL)
        room->characters.remove(this);
    if(toroom != NULL)
    {
		if (player != NULL && player->user != NULL)
		{
			//Send GMCP room.info
			json roominfo;
			roominfo["name"] = toroom->name;
			roominfo["num"] = toroom->id;
			if(toroom->area != 0 && Game::GetGame()->GetArea(toroom->area) != NULL)
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
        //Check if this room is a quest objective
        if(player)
        {
            player->QuestCompleteObjective(Quest::OBJECTIVE_ROOM, (void*)toroom);
        }
        //Check for room movement triggers
        Trigger * trig = NULL;
        int ctr = 0;
        Trigger::TriggerType tt;

        //ENTER_NPC, ENTER_PC
        if(player)
            tt = Trigger::ENTER_PC;
        else
            tt = Trigger::ENTER_NPC;

        while((trig = toroom->GetTrigger(ctr, tt)) != NULL)
        {
            ctr++;
            string func = trig->GetFunction();
            try
            {
                //TODO: dont load the script every time?
                //LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for room " + Utilities::itos(toroom->id));
                //string nil = trig->GetFunction() + " = nil;";
				Server::lua.script(trig->GetScript().c_str());
				Server::lua[func.c_str()](this, toroom);
            }
            catch(const std::exception & e)
			{
				LogFile::Log("error", e.what());
				/*if(logstring != NULL)
					LogFile::Log("error", logstring);*/
			}
            catch(...)
	        {
		        LogFile::Log("error", "call_function unhandled exception ENTER_PC ENTER_NPC");
	        }
        }

        //ENTER_CHAR
        tt = Trigger::ENTER_CHAR;
        while((trig = toroom->GetTrigger(ctr, tt)) != NULL)
        {
            ctr++;
            string func = trig->GetFunction();
            try
            {
                //TODO: dont load the script every time?
                //LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for room " + Utilities::itos(toroom->id));
                //string nil = trig->GetFunction() + " = nil;";
				Server::lua.script(trig->GetScript().c_str());
				Server::lua[func.c_str()](this, toroom);
            }
            catch(const std::exception & e)
			{
				LogFile::Log("error", e.what());
				/*if(logstring != NULL)
					LogFile::Log("error", logstring);*/
			}
            catch(...)
	        {
		        LogFile::Log("error", "call_function unhandled exception ENTER_CHAR");
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
    return ChangeRooms(NULL);
}

bool Character::IsAlive()
{
	if (IsNPC() && !IsCorpse())
		return true;
	if (!IsNPC() && !IsCorpse() && !player->IsGhost())
		return true;
	return false;
}

void Character::SetCorpse()
{
	isCorpse = true;
	if (!IsNPC())
	{
		player->UnsetGhost();
		player->death_timer = Player::DEFAULT_DEATH_TIME;
		player->death_timer_runback = Player::DEFAULT_DEATH_TIME_RUNBACK;
	}

	deathTime = Utilities::GetTime();
}

void Character::SetGhost()
{
	if (IsNPC())
		return;
	player->SetGhost();
	isCorpse = false;
}

void Character::SetAlive()
{
	isCorpse = false;
	if(!IsNPC())
		player->UnsetGhost();
}

bool Character::IsCorpse()
{
	return isCorpse;
}

bool Character::IsGhost()
{
	if (IsNPC())
		return false;
	return player->IsGhost();
}

int Character::TimeSinceDeath()
{
	return (int)(Game::currentTime - deathTime);
}

void Character::ApplyExperience(int amount)
{
    if(!player)
        return;

    player->SetExperience(player->experience + amount);

    while(player->experience < Game::ExperienceForLevel(level-1))
    {
        //lose levels
        SetLevel(level - 1);
    }
    while(level < Game::MAX_LEVEL && player->experience >= Game::ExperienceForLevel(level + 1))
    {
        //gain levels
        SetLevel(level + 1);
    }
}

void Character::SetTarget(Character * t)
{
    if(target == t)
        return;
    if(target != NULL)
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
        target = NULL;
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
        target = NULL;
    }

    if(delay_active && delayData.charTarget == lm)
    {
        delayData.charTarget->RemoveSubscriber(this);
		//cout << "Character::Notify delaydata REMOVE" << endl;
        delayData.charTarget = NULL;
		delay_active = false;
    }

    if(HasThreat((Character*)lm))
    {
        RemoveThreat((Character*)lm, false);
    }

	if (comboPointTarget && lm == comboPointTarget)
	{
		comboPointTarget->RemoveSubscriber(this);
		comboPointTarget = NULL;
	}

	if (delay_active && delayData.itemTarget == lm)
	{
		delayData.itemTarget->RemoveSubscriber(this);
		delayData.itemTarget = nullptr;
		delay_active = false;
	}

	if (hasQuery && (Character *)queryData == (Character *)lm) //We have a query pending where the 'data' payload is the Character being deleted (group invite)
	{
		QueryClear();
	}
}

void Character::AddTrigger(Trigger & trig)
{
    int ctr = 1;
    std::map<int, Trigger>::iterator iter;
    for(iter = triggers.begin(); iter != triggers.end(); ++iter)
    {
        if(ctr != iter->second.id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }
    trig.id = ctr;
    changed = true;
    triggers.insert(std::pair<int, Trigger>(trig.id, trig));
}

//If default argument type = -1, search for trigger id
//Otherwise search for id-th trigger of type
Trigger * Character::GetTrigger(int id, int type)
{
    if(type == -1)
    {
        std::map<int,Trigger>::iterator it = triggers.find(id);
        if(it != triggers.end())
            return &(it->second);
        return NULL;
    }
    else
    {
        std::map<int,Trigger>::iterator it;
        int count = 0;
        for(it = triggers.begin(); it != triggers.end(); ++it)
        {
            if(it->second.GetType() == type && count++ >= id)
            {
                return &(it->second);
            }
        }
        return NULL;
    }
    return NULL;
}

void Character::AddClassSkills()
{
	if (!player)
		return;

	std::list<Player::ClassData>::iterator iter;
	for (iter = player->classList.begin(); iter != player->classList.end(); ++iter) //For every class this player has multiclassed into...
	{
		Class * iclass = Game::GetGame()->classes.at(iter->id);	//Grab that class from the game...
		std::list<Class::SkillData>::iterator csiter;
		for (csiter = iclass->classSkills.begin(); csiter != iclass->classSkills.end(); csiter++) //For every skill players of that class get...
		{
			if (csiter->level <= iter->level || player->IMMORTAL()) //if that skill's level < our level of the class, add it
			{
				AddSkill(csiter->skill);
			}
		}
	}
}

bool Character::HasGroup()
{
	if (group != nullptr)
		return true;
	return false;
}

bool Character::InSameGroup(Character * ch)
{
	if (ch == this)
		return true;
	if (HasGroup() && ch->HasGroup() && group == ch->group)
		return true;
	return false;
}
