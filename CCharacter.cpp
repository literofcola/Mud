#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
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
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "utils.h"
#include "mud.h"

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
    lastMoveTime = 0;
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
    agility = intellect = strength = stamina = wisdom = 10;
	energy = maxEnergy = 100;
	rage = 0;
	maxRage = 100;
	comboPoints = 0;
	maxComboPoints = 5;
    health = maxHealth = stamina * Character::HEALTH_FROM_STAMINA;
	mana = maxMana = 100; //TODO mana is based on class level (add class->mana_per_level in db)
    //mana = maxMana = intellect * Character::MANA_FROM_INTELLECT;
    npcAttackSpeed = 2.0;
    npcDamageLow = npcDamageHigh = 1;
    delay_active = false;
    combat = false;
    meleeActive = false;
    movementSpeed = NORMAL_MOVE_SPEED;
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

    if(reset)
    {
        //LogFile::Log("status", "Removing listener reset ID " + Utilities::itos(reset->id) + " from character " + this->name);
        this->RemoveListener(reset);
    }
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
                if((*iter) != this)
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
                if((*iter) != vict)
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
                if((*iter) != vict && (*iter) != this)
                    (*iter)->Send(txt + "\n\r");
			    //mob trigger on msg here
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

void Character::ResetMaxStats()
{
	//todo: check equipment bonuses
	maxHealth = stamina * Character::HEALTH_FROM_STAMINA;
	maxMana = intellect * Character::MANA_FROM_INTELLECT;
	//todo: these might be higher based on skills or talents?
	maxEnergy = 100;
	maxRage = 100;
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
		/*if (energy > 0 && maxEnergy > 0)
			percent = (energy * 100) / maxEnergy;
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
			
		prompt += statColor + Utilities::itos(energy) + "/|X" + Utilities::itos(maxEnergy) + "|Ben "*/
		prompt += "|X" + Utilities::itos(energy) + "/" + Utilities::itos(maxEnergy) + "|Be ";

		//Rage
		/*if (mana > 0 && maxMana > 0)
			percent = (mana * 100) / maxMana;
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

		prompt += statColor + Utilities::itos(mana) + "/|X" + Utilities::itos(maxMana) + "|Bmp ";
		*/
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
		string targetPrompt = "";
		//Combo points
		if (comboPoints > 0 && GetTarget() == comboPointTarget)
		{
			targetPrompt += "|R(" + Utilities::itos(comboPoints) + ")|X";
		}
        string targetLevel;
        if(GetTarget()->IsNPC() && !Utilities::FlagIsSet(target->flags, Character::FLAG_FRIENDLY)
			&& Game::LevelDifficulty(level, GetTarget()->level) == 5) //NPC >= 10 levels
        {
            targetLevel = "|R??";
        }
        else
        {
            int difficulty = Game::LevelDifficulty(level, GetTarget()->level);
            switch(difficulty)
            {
            case 0: targetLevel = "|D"; break;
            case 1: targetLevel = "|G"; break;
            case 2: targetLevel = "|Y"; break;
            case 3: targetLevel = "|M"; break;
            case 4: targetLevel = "|R"; break;
            case 5: targetLevel = "|R"; break;
            }
            targetLevel += Utilities::itos(GetTarget()->level);
        }

        //TODO: Target name coloring based on pvp/attack status
		targetPrompt += "|B<" + targetLevel + " ";
        if(GetTarget() == this ||  Utilities::FlagIsSet(GetTarget()->flags, FLAG_FRIENDLY))
            targetPrompt += "|G";
        else if(Utilities::FlagIsSet(GetTarget()->flags, FLAG_NEUTRAL))
            targetPrompt += "|Y";
        else
            targetPrompt += "|R";
        targetPrompt += GetTarget()->name + "|X ";

        //Health
        if(GetTarget()->health > 0 && GetTarget()->maxHealth > 0)
            percent = (GetTarget()->health * 100)/GetTarget()->maxHealth;
        else
            percent = 0;

        if(percent >= 75 || GetTarget()->maxHealth == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%h ";

        //Mana
        if(GetTarget()->mana > 0 && GetTarget()->maxMana > 0)
            percent = (GetTarget()->mana * 100)/GetTarget()->maxMana;
        else
            percent = 0;

        if(percent >= 75 || GetTarget()->maxMana == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%m ";

		prompt += targetPrompt;

		//Energy
		if (GetTarget()->GetEnergy() > 0 && GetTarget()->maxEnergy > 0)
			percent = (GetTarget()->GetEnergy() * 100) / GetTarget()->maxEnergy;
		else
			percent = 0;
		prompt += "|X" + Utilities::itos(percent) + "|B%e ";

		//Rage
		if (GetTarget()->GetRage() > 0 && GetTarget()->maxRage > 0)
			percent = (GetTarget()->rage * 100) / GetTarget()->maxRage;
		else
			percent = 0;
		prompt += "|X" + Utilities::itos(rage) + "/|X" + Utilities::itos(maxRage) + "|Br>|X";
	}

    //Target of target (changed to display name, level, health only)
	if(GetTarget() != NULL && GetTarget()->GetTarget() != NULL)
	{
        Character * targettarget = GetTarget()->GetTarget();
        string targetLevel;
        if(player == NULL && Game::LevelDifficulty(level, targettarget->level) == 5) //NPC >= 10 levels
        {
            targetLevel = "|R??";
        }
        else
        {
            int difficulty = Game::LevelDifficulty(level, targettarget->level);
            switch(difficulty)
            {
            case 0: targetLevel = "|D"; break;
            case 1: targetLevel = "|G"; break;
            case 2: targetLevel = "|Y"; break;
            case 3: targetLevel = "|M"; break;
            case 4: targetLevel = "|R"; break;
            case 5: targetLevel = "|R"; break;
            }
            targetLevel += Utilities::itos(targettarget->level);
        }

        //TODO: Target name coloring based on pvp/attack status
		string targetPrompt = "|B<" + targetLevel + " ";
        if(targettarget == this || Utilities::FlagIsSet(targettarget->flags, FLAG_FRIENDLY))
            targetPrompt += "|G";
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

        //Mana
        /*if(targettarget->mana > 0 && targettarget->maxMana > 0)
            percent = (targettarget->mana * 100)/targettarget->maxMana;
        else
            percent = 0;

        if(percent >= 75 || targettarget->maxMana == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%mp ";
		*/
		prompt += targetPrompt;
	}
    
    /*
	if(group)
	{
		Group * g = group;
		int firstingroup = g->FindFirstInSubgroup(this);
		if(!g->IsEmptySubgroup(this))
		{
			prompt += "\n\r";
			for(int i = 0; i < SUBGROUP_SIZE; i++)
			{
				if(g->members[firstingroup + i] && g->members[firstingroup + i] != this)
				{
					prompt += "|B<";
					prompt += "|Y" + g->members[firstingroup + i]->name + "|X ";
					prompt += VitalsPercent(this) + "|B>|X";
				}
			}
		}
	}
    */
    
	if(hasQuery)
	{
		prompt += queryPrompt;
	}
    
    prompt += "\n\r";
	Send(prompt);

	//Really we should send updates when individual stats CHANGE, not every prompt
	json vitals = { { "hp", health }, { "hpmax", maxHealth }, { "mp", mana }, { "mpmax", maxMana }, 
				    { "en", energy }, { "enmax", maxEnergy }, { "rage", rage },{ "ragemax", maxRage } };
	SendGMCP("char.vitals " + vitals.dump());
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

//Find a character in this room or any adjacent room
Character * Character::GetCharacterAdjacentRoom(string name)
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
    return NULL;
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
    
    if(delay_active)
    {
        delay_active = false;
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

    Message(name + " leaves " + Exit::exitNames[direction] + ".", MSG_ROOM_NOTCHAR);
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
    Message(name + " has arrived from " + ((direction != Exit::DIR_UP && direction != Exit::DIR_DOWN) ? "the " : "") + Exit::reverseExitNames[direction] + ".", MSG_ROOM_NOTCHAR);

    cmd_look(this, "");

    //check npc aggro
    for(std::list<Character*>::iterator iter = room->characters.begin(); iter != room->characters.end(); ++iter)
    {
        if((*iter)->IsNPC() && Utilities::FlagIsSet((*iter)->flags, Character::FLAG_AGGRESSIVE) && !(*iter)->InCombat() && player && !player->IMMORTAL())
        {
            (*iter)->EnterCombat(this);
            EnterCombat(*iter);
            (*iter)->AutoAttack(this);
        }
    }
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
    loaded->player->isGhost = row["ghost"];
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
	/*string skilltext = (string)row["skills"];
	int first=0, last=0;
	while(first < (int)skilltext.length())
	{
	last = (int)skilltext.find(";", first);
	int id = Utilities::atoi(skilltext.substr(first, last - first));
	loaded->AddSkill(Game::GetGame()->GetSkill(id));
	first = last + 1;
	}*/

	StoreQueryResult playerinventoryres = Server::sqlQueue->Read("SELECT * FROM player_inventory where player='" + loaded->name + "'");
	for (iter = playerinventoryres.begin(); iter != playerinventoryres.end(); ++iter)
	{
		Row invrow = *iter;
		int id = invrow["item"];
		int location = invrow["location"]; //Location means equipped, in inventory, in bank...
		switch (location)
		{
			case DB_INVENTORY_EQUIPPED:
			{
				Item * equip = Game::GetGame()->GetItemIndex(id);
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
				loaded->player->EquipItemFromInventory(loaded->player->NewItemInventory(equip), equiploc);

				break;
			}

			case DB_INVENTORY_INVENTORY:
			{
				loaded->player->NewItemInventory(Game::GetGame()->GetItemIndex(id));
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
	/*
    string activequests = (string)row["active_quests"];
    first=0; last=0;
    int i = 0;
    while(first < (int)activequests.length())
    {
        last = (int)activequests.find(";", first);
        int firstcomma = (int)activequests.find(",", first);
        int id = Utilities::atoi(activequests.substr(first, firstcomma - first));
        Quest * q;
        if((q = Game::GetGame()->GetQuest(id)) != NULL)
        {
            loaded->player->questLog.push_back(q);
            std::vector<int> playerObjectives;
            loaded->player->questObjectives.push_back(playerObjectives);

            int count;
            for(int lastcomma = (int)activequests.find(",", firstcomma+1); lastcomma != std::string::npos && lastcomma < last; lastcomma = (int)activequests.find(",", firstcomma))
            {
                count = Utilities::atoi(activequests.substr(firstcomma+1, lastcomma - firstcomma+1));
                loaded->player->questObjectives[i].push_back(count);
                firstcomma = lastcomma+1;
            }
            //search for a ';' to grab the last objective
            count = Utilities::atoi(activequests.substr(firstcomma+1, activequests.find(";", firstcomma) - firstcomma+1));
            loaded->player->questObjectives[i].push_back(count);
        }
        i++;
        first = last + 1;
    }
	*/
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
        sql += "wisdom, health, mana, class, recall, ghost, stat_points) values ('";
        sql += name + "','" + password + "'," + Utilities::itos(player->immlevel);
        sql += ",'" + fixtitle + "'," + Utilities::itos(player->experience) + "," + Utilities::itos(room->id);
        sql += "," + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intellect) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(stamina) + "," + Utilities::itos(wisdom) + ",";
        sql += Utilities::itos(health) + "," + Utilities::itos(mana);
		sql += "," + Utilities::itos(player->currentClass->id) + "," + Utilities::itos(player->recall) + ", ";
        if(IsGhost() || IsCorpse())
            sql += "1,";
        else
            sql += "0,";
		sql += Utilities::itos(player->statPoints) + ")";

        sql += " ON DUPLICATE KEY UPDATE name=VALUES(name), password=VALUES(password), immlevel=VALUES(immlevel), title=VALUES(title), ";
        sql += "experience=VALUES(experience), room=VALUES(room), level=VALUES(level), gender=VALUES(gender), race=VALUES(race), agility=VALUES(agility), ";
        sql += "intellect=VALUES(intellect), strength=VALUES(strength), stamina=VALUES(stamina), wisdom=VALUES(wisdom), ";
        sql += "health=VALUES(health), mana=VALUES(mana), ";
        sql += "class=VALUES(class), ";
        sql += "recall=VALUES(recall), ghost=VALUES(ghost), stat_points=VALUES(stat_points)";

		//player_completed_quests
		std::set<int>::iterator questiter;
		for (questiter = player->completedQuests.begin(); questiter != player->completedQuests.end(); ++questiter)
		{
			string qcsql = "INSERT INTO player_completed_quests (player, quest) values ";
			qcsql += "('" + name + "', " + Utilities::itos(*questiter) + ")";
			Server::sqlQueue->Write(qcsql);
		}
		
		//player_active_quests
		Server::sqlQueue->Write("DELETE FROM player_active_quests where player='" + name + "'");
		for (int i = 0; i < (int)player->questLog.size(); i++)
		{
			string qasql = "INSERT INTO player_active_quests (player, quest, objectives) values ('" + name + "',";
			qasql += Utilities::itos(player->questLog[i]->id);
			for (int j = 0; j < (int)player->questObjectives[i].size(); j++)
			{
				qasql += "," + Utilities::itos(player->questObjectives[i][j]);
			}
			qasql += ")";
			Server::sqlQueue->Write(qasql);
		}

		//player_inventory
		Server::sqlQueue->Write("DELETE FROM player_inventory where player='" + name + "'");
		for (int i = 0; i < (int)player->equipped.size(); i++)
		{
			if (player->equipped[i] != NULL)
			{
				string equippedsql = "INSERT INTO player_inventory (player, item, location) values ('" + name + "',";
				equippedsql += Utilities::itos(player->equipped[i]->id) + "," + Utilities::itos(DB_INVENTORY_EQUIPPED) + ")";
				Server::sqlQueue->Write(equippedsql);
			}
		}

		std::list<Item *>::iterator inviter;
		for (inviter = player->inventory.begin(); inviter != player->inventory.end(); ++inviter)
		{
			string equippedsql = "INSERT INTO player_inventory (player, item, location) values ('" + name + "',";
			equippedsql += Utilities::itos((*inviter)->id) + "," + Utilities::itos(DB_INVENTORY_INVENTORY) + ")";
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

        player->saved = (int)Game::GetGame()->currentTime;
    }
    else //save npcs
    {
        string fixtitle = Utilities::SQLFixQuotes(title);

        sql = "INSERT INTO npcs (id, name, keywords, level, gender, race, agility, intellect, strength, stamina, ";
        sql += "wisdom, health, mana, energy, rage, title, attack_speed, damage_low, damage_high, flags, speechtext) values (";
        sql += Utilities::itos(id) + ", '";
        sql += name + "', '" + keywords + "', " + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intellect) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(stamina) + "," + Utilities::itos(wisdom) + ",";
        sql += Utilities::itos(health) + "," + Utilities::itos(mana) + "," + Utilities::itos(energy) + "," + Utilities::itos(rage);
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

	Send("|WYou have reached level " + Utilities::itos(newlevel) + "!|X\n\r");

    if(player)
    {
        player->SetClassLevel(player->currentClass->id, 
                              Utilities::MAX(0, player->GetClassLevel(player->currentClass->id) + (newlevel - level)));
	
		player->statPoints += Player::STATS_PER_LEVEL;
		Send("|WYou gain " + Utilities::itos(Player::STATS_PER_LEVEL) + " attribute points. Assign attribute points with the \"train\" command.|X\n\r");
    }
	level = newlevel;
	health = maxHealth;
	mana = maxMana;
	energy = maxEnergy;
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
    //LogFile::Log("status", "Adding listener to " + sa->caster->name + " of SA " + sa->name);
    if(sa->caster)
    {
        sa->caster->AddListener(sa);
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
    return NULL;
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
        std::map<string, double>::iterator iter2;
        iter2 = cooldowns.find((*iter).second->name);
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
    StoreQueryResult cooldownres = Server::sqlQueue->Read("SELECT * FROM player_cooldowns WHERE player='" + name + "';");
    if(cooldownres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = cooldownres.begin(); i != cooldownres.end(); i++)
    {
        row = *i;
        Skill * sk = Game::GetGame()->GetSkill(row["skill"]);
        if(sk != NULL)
        {
            cooldowns[sk->name] = row["timestamp"];
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

int Character::GetAuraModifier(int affect, int whatModifier)
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
                if((*iter2).affectID == affect)
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
                if((*iter2).affectID == affect)
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

int Character::GetTotalAuraModifier(int affect)
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
                if((*iter2).affectID == affect)
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
                if((*iter2).affectID == affect)
                    total += (*iter2).modifier;
            }
        }
    }
    return total;
}

int Character::GetLargestAuraModifier(int affect)
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
                if((*iter2).affectID == affect && (largest < (*iter2).modifier || largest == 0))
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
                if((*iter2).affectID == affect && (largest < (*iter2).modifier || largest == 0))
                    largest = (*iter2).modifier;
            }
        }
    }
    return largest;
}

int Character::GetSmallestAuraModifier(int affect)
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
                if((*iter2).affectID == affect && (smallest > (*iter2).modifier || smallest == 0))
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
                if((*iter2).affectID == affect && (smallest < (*iter2).modifier || smallest == 0))
                    smallest = (*iter2).modifier;
            }
        }
    }
    return smallest;
}

void Character::EnterCombat(Character * victim)
{
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
    //AddAggressor(victim);
    //victim->AddAggressor(this);

    if(player)
        player->lastCombatAction = Game::currentTime;
    if(victim->player)
        victim->player->lastCombatAction = Game::currentTime;

    if(victim->IsNPC())
    {
        victim->UpdateThreat(this, 0);
    }

    //SpellAffect * sa = AddSpellAffect(true, this, "combat_movement_speed", true, false, 0, 0, NULL);
    //if(sa != NULL)
    //    sa->ApplyAura(SpellAffect::AURA_MOD_MOVESPEED, -70);
    movementSpeed = Character::COMBAT_MOVE_SPEED;
    victim->movementSpeed = Character::COMBAT_MOVE_SPEED;
}

void Character::ExitCombat()
{
    combat = false;
    meleeActive = false;
    RemoveThreat(NULL, true);
    //RemoveSpellAffect(true, "combat_movement_speed");
    movementSpeed = Character::NORMAL_MOVE_SPEED;
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
        OneHit(victim, damage); //TODO fancy damage calculations, block miss hit crit 
        //victim may be invalid here if it was killed!
    }
    else if(player != NULL)
    {
        if(player->equipped[Player::EQUIP_MAINHAND] != NULL)
        {
            if(player->equipped[Player::EQUIP_MAINHAND]->speed > 0 && player->equipped[Player::EQUIP_MAINHAND]->damageHigh > 0)
            {
                weaponSpeed_main = player->equipped[Player::EQUIP_MAINHAND]->speed;
                int high = player->equipped[Player::EQUIP_MAINHAND]->damageHigh;
                int low = player->equipped[Player::EQUIP_MAINHAND]->damageLow;
                if(high != low)
                    damage_main = (Server::rand() % (high - low)) + low;
                else
                    damage_main = low;
            }
            else
            {
                attack_mh = false; //no mainhand attack only if we're holding a non weapon
            }
        }
        if(player->equipped[Player::EQUIP_OFFHAND] != NULL)
        {
            if(player->equipped[Player::EQUIP_OFFHAND]->speed > 0 && player->equipped[Player::EQUIP_OFFHAND]->damageHigh > 0)
            {
                weaponSpeed_off = player->equipped[Player::EQUIP_OFFHAND]->speed;
                int high = player->equipped[Player::EQUIP_OFFHAND]->damageHigh;
                int low = player->equipped[Player::EQUIP_OFFHAND]->damageLow;
                if(high != low)
                    damage_off = (Server::rand() % (high - low)) + low;
                else
                    damage_off = low;
                attack_oh = true; //offhand attack only if we're holding a weapon (no attack if empty)
            }
        }

        if(attack_mh && lastAutoAttack_main + weaponSpeed_main <= Game::currentTime)
        {
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

            OneHit(victim, damage_main); //TODO fancy damage calculations, block miss hit crit 
            //victim may be invalid if it was killed!
        }
        if(attack_oh && lastAutoAttack_off + weaponSpeed_off <= Game::currentTime)
        {
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
    //TODO fancy damage calculations, block miss hit crit, weapon damage
    Send("|GYou hit " + victim->name + " for " + Utilities::itos(damage) + " damage.|X\n\r");
    victim->Send("|Y" + name + " hits you for " + Utilities::itos(damage) + " damage.|X\n\r");
    Message("|Y" + name + "'s attack hits " + victim->name + " for " + Utilities::itos(damage) + " damage.|X", Character::MSG_ROOM_NOTCHARVICT, victim);
    
    if(victim->IsNPC())
    {
        victim->UpdateThreat(this, damage);
        //Send("My threat on " + victim->name + " is " + Utilities::itos(victim->GetThreat(this)) + "\n\r");
    }
	if (victim->CancelCast())
		victim->Send("Action Interrupted!\n\r");

    victim->AdjustHealth(this, -damage);
}

int Character::GetIntellect()
{
	return intellect;
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

int Character::GetRage()
{
	return rage;
}

int Character::GetComboPoints()
{
	return comboPoints;
}

void Character::AdjustHealth(Character * source, int amount)
{
    if(source == NULL)
    {
        //a possibility
		source = this; //self damage!?!? Will this work?
    }
    //TODO: update threat values for damage and healing (if in combat only?)
    if(amount < 0)
    {
        (health + amount >= 0) ? health += amount : health = 0;
        if(health == 0 && player && player->IMMORTAL())
            health = 1;
    }
    if(amount > 0)
    {
        (health + amount >= maxHealth) ? health = maxHealth : health += amount;
    }

    if(health == 0) //todo: functionize me. ROM = "ExtractChar"
    {
        Message("|R" + name + " has been slain!|X", Character::MSG_ROOM_NOTCHAR, source);
        Send("|RYou have been slain!|X\n\r");
        if(source->target == this)
        {
            source->meleeActive = false;
			source->ClearTarget();
        }
        if(delay_active)
        {
            delay_active = false;
        }
        if(!IsNPC() && !source->IsNPC()) //player - player
        {
            ExitCombat();
            RemoveAllSpellAffects();
            //set flag for corpseified
            this->player->isCorpse = true;
            //SpellAffect * sa = AddSpellAffect(true, this, "player_is_corpse", true, false, 0, 0, 0, NULL);

            queryData = NULL;
	        hasQuery = true;
	        queryPrompt = "Release spirit? (y) ";
	        queryFunction = releaseSpiritQuery;
        }
        else if(IsNPC() && !source->IsNPC()) //NPC killed by player
        {
            ExitCombat();
            ClearTarget();
            ChangeRooms(NULL);
            int exp = Game::CalculateExperience(source, this);
            source->Send("|BYou have gained |Y" + Utilities::itos(exp) + "|B experience.|X\n\r");
            source->ApplyExperience(exp);
            //TODO apply experience to source. need to handle groups/who tapped npc
            std::list<DropData>::iterator dropiter;
            for(dropiter = drops.begin(); dropiter != drops.end(); ++dropiter)
            {
                if(Server::rand() % 100 <= (*dropiter).percent && (*dropiter).id.size() > 0)
                {
                    int which = Server::rand() % ((int)(*dropiter).id.size());
                    Item * drop = Game::GetGame()->GetItemIndex((*dropiter).id[which]);
                    source->Send("You receive loot: " + (string)Item::quality_strings[drop->quality] + drop->name + "|X.\n\r");
                    source->Message(source->name + " receives loot: " + Item::quality_strings[drop->quality] + drop->name + "|X.\n\r",
                                    Character::MSG_ROOM_NOTCHAR);
                    source->player->NewItemInventory(drop);
                }
            }
            if(source->player)
            {
                source->player->QuestCompleteObjective(Quest::OBJECTIVE_KILLNPC, (void*)this);
            }
            Game::GetGame()->RemoveCharacter(this);
        }
        else if(!IsNPC() && source->IsNPC()) //player killed by NPC
        {
            ExitCombat();
            RemoveAllSpellAffects();
            //set flag for corpseified
            //SpellAffect * sa = AddSpellAffect(true, this, "player_is_corpse", true, false, 0, 0, 0, NULL);
            this->player->isCorpse = true;
            queryData = NULL;
	        hasQuery = true;
	        queryPrompt = "Release spirit? (y) ";
	        queryFunction = releaseSpiritQuery;
        }
        else if(IsNPC() && source->IsNPC()) //npc - npc
        {
            ExitCombat();
            ClearTarget();
            ChangeRooms(NULL);
            Game::GetGame()->RemoveCharacter(this);
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

//Mana adjusting function to be used by spells. Invokes 5 second rule and checks for AURA_RESOURCE_COST
//A negative amount would indicate a mana gain
//todo: we have ConsumeMana, SetMana, AdjustMana, ridiculous. Sort that out
void Character::ConsumeMana(int amount)
{
    int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);
    
    double increase = amount * (resource_cost / 100.0);
    amount += (int)increase;

    if(mana - amount < 0)
    {
        mana = 0;
    }
    else if(mana - amount > maxMana)
    {
        mana = maxMana;
    }
    else
    {
        mana -= amount;
    }

    //start 5 second rule if we lost mana
    if(amount > 0)
    {
        this->lastSpellCast = Game::currentTime;
    }   
}


void Character::SetMana(Character * source, int amount)
{
	//todo: this stuff should probably be in "AdjustMana" while this is just a simple set function
    if(source == NULL)
    {
        //a possibility
    }
    
    if(amount < 0)
    {
        mana = 0;
    }
    else if(amount > maxMana)
    {
        mana = maxMana;
    }
    else
    {
        mana = amount;
    }

    //start 5 second rule if source was ourself and we lost mana
    if(source == this && amount < 0)
    {
        this->lastSpellCast = Game::currentTime;
    }
}

void Character::AdjustMana(Character * source, int amount)
{
    SetMana(source, mana + amount);
}

void Character::AdjustEnergy(Character * source, int amount)
{
	energy += amount;
}

//Energy adjusting function to be used by spells. Checks for AURA_RESOURCE_COST
//A negative amount would indicate an energy gain
void Character::ConsumeEnergy(int amount)
{
	int resource_cost = GetAuraModifier(SpellAffect::AURA_RESOURCE_COST, 1);

	double increase = amount * (resource_cost / 100.0);
	amount += (int)increase;

	if (energy - amount < 0)
	{
		energy = 0;
	}
	else if (energy - amount > maxEnergy)
	{
		energy = maxEnergy;
	}
	else
	{
		energy -= amount;
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

	if (target != comboPointTarget)	//Changing our combo target
	{
		if (comboPointTarget != nullptr)	//If we had a previous combo target...
			comboPointTarget->RemoveListener(this);
		comboPointTarget = target;
		comboPoints = 1;
		target->AddListener(this);
	}
	else if(comboPoints < maxComboPoints)
	{
		comboPoints++;
	}
}

void Character::UpdateThreat(Character * ch, int value)
{
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
            (*iter).threat += value;
            return;
        }
    }
    ch->AddListener(this);
    Threat tt = {ch, value}; //TODO: are we storing a copy here?
    threatList.push_front(tt);
}

Character * Character::GetTopThreat()
{
    std::list<Threat>::iterator iter = threatList.begin();
    if(iter != threatList.end())
    {
        return (*iter).ch;
    }
    return NULL;
}

void Character::RemoveThreat(Character * ch, bool removeall)
{
    if(removeall)
    {
        std::list<Threat>::iterator iter;
        for(iter = threatList.begin(); iter != threatList.end(); ++iter)
        {
            (*iter).ch->RemoveListener(this);
        }
        threatList.clear();
        return;
    }
    
    std::list<Threat>::iterator iter;
    for(iter = threatList.begin(); iter != threatList.end(); ++iter)
    {
        if((*iter).ch == ch)
        {
            (*iter).ch->RemoveListener(this);
            threatList.erase(iter);
            return;
        }
    }
}

int Character::GetThreat(Character * ch)
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

std::string Character::HisHer()
{
    return (gender == 1 ? "his" : "her");
}

bool Character::CancelCast()
{
	if(delay_active && delayData.sk->interruptFlags.test(Skill::Interrupt::INTERRUPT_HIT))
	{
		delay_active = false;
		return true;
	}
	return false;
}

bool Character::CanMove()
{
    if(player && player->IMMORTAL())
        return true;

	double movespeed = GetMoveSpeed();

    if(movespeed <= 0)
        return false;

    return (Game::currentTime > lastMoveTime + (1.0/(movementSpeed * movespeed)));
}

double Character::GetMoveSpeed()
{
    int low = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 5);
    int high = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 4);
 
    double newspeed = 100 + high + low;

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

void Character::SetCooldown(Skill * sk, std::string name, bool global, double length) //USE LENGTH -1 TO USE SKILL->COOLDOWN
{
    if(global)//Set 1.5 second cooldown on everything
    {
        std::map<string, Skill *>::iterator iter;
        for(iter = knownSkills.begin(); iter != knownSkills.end(); ++iter)
        {
            //TODO
            //if(!(*iter).second->ignoreGlobal)
                cooldowns[(*iter).second->name] = Game::currentTime + 1.5;
        }
    }
    if(length == 0)
        return;
    if(sk == NULL && !name.empty())
    {
        sk = GetSkillShortName(name);
    }

    if(sk != NULL)
    {
        if(length < 0) //if length default argument == -1, use sk->cooldown
            length = sk->cooldown;
        if(length == 0)
            return;
        cooldowns[sk->name] = Game::currentTime + length;
    }
}

double Character::GetCooldownRemaining(Skill * sk)
{
    if(sk == NULL)
        return 0;

    if(player && player->IMMORTAL())
        return 0;

    std::map<string, double>::iterator iter;
    iter = cooldowns.find(sk->name);
    if(iter == cooldowns.end())
        return 0;
    if((*iter).second <= Game::currentTime)
        return 0;
    return (*iter).second - Game::currentTime;
}

bool Character::IsNPC()
{
    return (player == NULL);
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

bool Character::IsCorpse()
{
    if(player && player->isCorpse)
        return true;
    return false;
}

bool Character::IsGhost()
{
    if(player && player->isGhost)
        return true;
    return false;
}

void Character::ApplyExperience(int amount)
{
    if(!player)
        return;

    player->SetExperience(player->experience + amount);

    while(player->experience < Game::ExperienceForLevel(level))
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
    //LogFile::Log("status", "Adding " + name + " as a listener to " + t->name);
    //LogFile::Log("status", "Setting " + name + " target = " + t->name);
    t->AddListener(this);
    target = t;
}

void Character::ClearTarget()
{
    if(target)
    {
        //LogFile::Log("status", "Removing " + name + " as a listener from " + target->name);
        //LogFile::Log("status", "Setting " + name + " target = NULL");
        target->RemoveListener(this);
        //RemoveListener(target);
        target = NULL;
    }
}

Character * Character::GetTarget()
{
    return target;
}

void Character::Notify(ListenerManager * lm)
{
    //Character * thislistener = (Character *)lm;
    //Send("The target is " + target->name + " and listenermanager is " + thislistener->name + "\n\r");
    /*if(lm == target) //will this be the same address?
    {
        Send("yes\n\r");
    }*/
    //ListenerManager * test = new Character();
    //Listener * test = new Character();

    //with the new parameter we can check if the thing that just notified us is our target, 
    //  or is our spell cast target (delaydata.charTarget), or ...
    if(target && lm == target)
    {
        //LogFile::Log("status", "Removing " + name + " as a listener from " + target->name);
        //LogFile::Log("status", "Setting " + name + " target = NULL");
        target->RemoveListener(this);
        target = NULL;
    }

    if(delayData.charTarget == lm)
    {
        //LogFile::Log("status", "Removing delayData listener " + name + " from " + delayData.charTarget->name);
        delayData.charTarget->RemoveListener(this);
        delayData.charTarget = NULL;
    }

    if(HasThreat((Character*)lm))
    {
        RemoveThreat((Character*)lm, false);
    }

	if (comboPointTarget && lm == comboPointTarget)
	{
		comboPointTarget->RemoveListener(this);
		comboPointTarget = NULL;
	}
}

//TODO function to print listeners on a target

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