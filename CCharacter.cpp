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

//#include "luabind/luabind.hpp"

using namespace std;

//in rooms per second
const double Character::NORMAL_MOVE_SPEED = 2.5;
const double Character::COMBAT_MOVE_SPEED = 0.75;

//TODO
Character::RaceType Character::race_table[] = 
{
    { 1, "human" },
    { 2, "elf"   }
};

Character::flag_type Character::flag_table[] = 
{
    { Character::FLAG_FRIENDLY, "friendly" },
    { Character::FLAG_NEUTRAL, "neutral" },
    { Character::FLAG_AGGRESSIVE, "aggressive" },
    { Character::FLAG_GUILD, "guild" },
    { Character::FLAG_VENDOR, "vendor" },
    { Character::FLAG_REPAIR, "repair" },
    { Character::FLAG_MAGETRAIN, "magetrain" },
    { Character::FLAG_WARRIORTRAIN, "warriortrain" },
    { Character::FLAG_ROGUETRAIN, "roguetrain" },
    { Character::FLAG_CLERICTRAIN, "clerictrain" },
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
    id = copy.id;
    title = copy.title;
    player = NULL;
    level = copy.level;
    sex = copy.sex;
    agility = copy.agility;
    intelligence = copy.intelligence;
    strength = copy.strength;
    vitality = copy.vitality;
    wisdom = copy.wisdom;
    health = maxHealth = copy.maxHealth;
    mana = maxMana = copy.maxMana;
    stamina = maxStamina = copy.maxStamina;
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
    sex = 1;
    agility = intelligence = strength = vitality = wisdom = 10;
    health = maxHealth = vitality * Character::HEALTH_FROM_VITALITY;
    mana = maxMana = intelligence * Character::MANA_FROM_INTELLIGENCE;
    stamina = maxStamina = strength * Character::STAMINA_FROM_STRENGTH;
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
    intTable["id"] = &id;
    intTable["level"] = &level;
    intTable["sex"] = &sex;
    intTable["agility"] = &agility;
    intTable["intelligence"] = &intelligence;
    intTable["strength"] = &strength;
    intTable["vitality"] = &vitality;
    intTable["wisdom"] = &wisdom;
    intTable["health"] = &health;
    intTable["mana"] = &mana;
    intTable["stamina"] = &stamina;
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

void Character::SendSubchannel(std::string str)
{
    if(player == NULL || player->user == NULL || !player->user->gmcp)
        return;

    player->user->SendSubchannel(str);
}

void Character::SendSubchannel(char * str)
{
    if(player == NULL || player->user == NULL)
        return;

    player->user->SendSubchannel(str);
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

        prompt += statColor + Utilities::itos(health) + "/|X" + Utilities::itos(maxHealth) + "|Bhp ";

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

        prompt += statColor + Utilities::itos(mana) + "/|X" + Utilities::itos(maxMana) + "|Bsp ";

        //Stamina
        if(stamina > 0 && maxStamina > 0)
            percent = (stamina * 100)/maxStamina;
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

        prompt += statColor + Utilities::itos(stamina) + "/|X" + Utilities::itos(maxStamina) + "|Bst";
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
        string targetLevel;
        if(player == NULL && Game::LevelDifficulty(level, GetTarget()->level) == 5) //NPC >= 10 levels
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
		string targetPrompt = "|B<" + targetLevel + " ";
        if(GetTarget() == this || Utilities::FlagIsSet(GetTarget()->flags, FLAG_FRIENDLY))
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

        targetPrompt += statColor + Utilities::itos(percent) + "|B%hp ";

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

        targetPrompt += statColor + Utilities::itos(percent) + "|B%sp ";

        //Stamina
        if(GetTarget()->stamina > 0 && GetTarget()->maxStamina > 0)
            percent = (GetTarget()->stamina * 100)/GetTarget()->maxStamina;
        else
            percent = 0;

        if(percent >= 75 || GetTarget()->maxStamina == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%st";
        targetPrompt += ">|x";

		prompt += targetPrompt;
	}

    //Target of target
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

        targetPrompt += statColor + Utilities::itos(percent) + "|B%hp ";

        //Mana
        if(targettarget->mana > 0 && targettarget->maxMana > 0)
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

        targetPrompt += statColor + Utilities::itos(percent) + "|B%sp ";

        //Stamina
        if(targettarget->stamina > 0 && targettarget->maxStamina > 0)
            percent = (targettarget->stamina * 100)/targettarget->maxStamina;
        else
            percent = 0;

        if(percent >= 75 || targettarget->maxStamina == 0)
            statColor = "|x";
        else if(percent >= 50)
            statColor = "|G";
        else if(percent >= 25)
            statColor = "|Y";
        else
            statColor = "|R";

        targetPrompt += statColor + Utilities::itos(percent) + "|B%st";
        targetPrompt += ">|x";

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
        if((*iter)->IsNPC() && Utilities::FlagIsSet((*iter)->flags, Character::FLAG_AGGRESSIVE) && !(*iter)->InCombat())
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
    loaded->sex = row["sex"];
    loaded->title = row["title"];
    loaded->level = row["level"];
    loaded->agility = row["agility"];
    loaded->intelligence = row["intelligence"];
    loaded->strength = row["strength"];
    loaded->vitality = row["vitality"];
    loaded->wisdom = row["wisdom"];
    //TODO: include equipment/buff bonuses
    loaded->health = row["health"];
    loaded->mana = row["mana"];
    loaded->stamina = row["stamina"];
    loaded->maxHealth = loaded->vitality * Character::HEALTH_FROM_VITALITY;
    loaded->maxMana = loaded->intelligence * Character::MANA_FROM_INTELLIGENCE;
    loaded->maxStamina = loaded->strength * Character::STAMINA_FROM_STRENGTH;

    loaded->room = Game::GetGame()->GetRoom(row["room"]); 

	if(!loaded->player)
		loaded->player = new Player(user);
    loaded->player->currentClass = Game::GetGame()->GetClass(row["class"]);
    loaded->player->password = row["password"];
    loaded->player->immlevel = row["immlevel"];
    loaded->player->experience = row["experience"];
	loaded->player->recall = row["recall"];
    loaded->player->isGhost = row["is_ghost"];

    string skilltext = (string)row["skills"];
    int first=0, last=0;
    while(first < (int)skilltext.length())
    {
        last = (int)skilltext.find(";", first);
        int id = Utilities::atoi(skilltext.substr(first, last - first));
        loaded->AddSkill(Game::GetGame()->GetSkill(id));
        first = last + 1;
    }

    string classtext = (string)row["class_data"];
    first = last = 0;
    while(first < (int)classtext.length())
    {
        last = (int)classtext.find(";", first);
        int comma = (int)classtext.find(",", first);
        int id = Utilities::atoi(classtext.substr(first, comma - first));
        int level = Utilities::atoi(classtext.substr(comma+1, last - comma+1));
        loaded->player->AddClass(id, level);
        first = last + 1;
    }

    string equippedtext = (string)row["equipped"];
    first = last = 0;
    while(first < (int)equippedtext.length())
    {
        last = (int)equippedtext.find(";", first);
        int id = Utilities::atoi(equippedtext.substr(first, last - first));

        Item * equip = Game::GetGame()->GetItemIndex(id);
        int equiploc = loaded->player->GetEquipLocation(equip);
        Item * removed = loaded->player->RemoveItemEquipped(equiploc); //remove any item already in the slot
        if(removed != NULL)
            loaded->player->AddItemInventory(removed);
        if(equiploc == Player::EQUIP_MAINHAND && equip->equipLocation == Item::EQUIP_TWOHAND)
        {   //pretty unlikely this will happen when loading a character, but just in case
            Item * offhand = loaded->player->RemoveItemEquipped(Player::EQUIP_OFFHAND); //remove any offhand when equipping a two hand
            if(offhand != NULL)
                loaded->player->AddItemInventory(offhand);
        }
        else if(equiploc == Player::EQUIP_OFFHAND) //remove a twohand when equipping an offhand
        {
            if(loaded->player->equipped[Player::EQUIP_MAINHAND] != NULL && loaded->player->equipped[Player::EQUIP_MAINHAND]->type == Item::EQUIP_TWOHAND)
            {
                loaded->player->AddItemInventory(loaded->player->RemoveItemEquipped(Player::EQUIP_MAINHAND));
            }
        }
        loaded->player->EquipItemFromInventory(loaded->player->NewItemInventory(equip), equiploc);
        first = last + 1;
    }

    string invtext = (string)row["inventory"];
    first = last = 0;
    while(first < (int)invtext.length())
    {
        last = (int)invtext.find(";", first);
        int id = Utilities::atoi(invtext.substr(first, last - first));
        loaded->player->NewItemInventory(Game::GetGame()->GetItemIndex(id));
        first = last + 1;
    }

    string completedquests = (string)row["completed_quests"];
    first=0; last=0;
    while(first < (int)completedquests.length())
    {
        last = (int)completedquests.find(";", first);
        int id = Utilities::atoi(completedquests.substr(first, last - first));
        Quest * q;
        if((q = Game::GetGame()->GetQuest(id)) != NULL)
            loaded->player->completedQuests[id] = q;
        first = last + 1;
    }

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

    return loaded;
}

/*Character * Character::LoadNPC(Server * server, int id) 
{
    StoreQueryResult characterres = server->sqlQueue->Read("select * from npcs where id=" + Utilities::itos(id));
    if(characterres.empty())
        return NULL;

    Row row = *characterres.begin();

    //Character * loaded = server->game->NewCharacter(); //Don't add to in-game characters
    Character * loaded = new Character();

    loaded->name = row["name"];
    loaded->sex = row["sex"];
    loaded->level = row["level"];
    loaded->agility = row["agility"];
    loaded->intelligence = row["intelligence"];
    loaded->strength = row["strength"];
    loaded->vitality = row["vitality"];
    loaded->wisdom = row["wisdom"];
    //TODO: add equipment and buff bonuses (for npcs??)
    loaded->health = row["health"];
    loaded->mana = row["mana"];
    loaded->stamina = row["stamina"];
    loaded->maxHealth = loaded->vitality * Character::HEALTH_FROM_VITALITY;
    loaded->maxMana = loaded->intelligence * Character::MANA_FROM_INTELLIGENCE;
    loaded->maxStamina = loaded->strength * Character::STAMINA_FROM_STRENGTH;

    string skilltext = (string)row["skills"];
    int first=0, last=0;
    while(first < skilltext.length())
    {
        last = skilltext.find(";", first);
        int id = Utilities::atoi(skilltext.substr(first, last - first));
        loaded->AddSkill(Game::GetGame()->GetSkill(id));
        first = last + 1;
    }

    string skilltext = (string)row["skills"];

    return loaded;
}*/

void Character::Save()
{
    string sql;
    if(player)
    {
        string password = Utilities::SQLFixQuotes(player->password);
        string fixtitle = Utilities::SQLFixQuotes(title);

        sql = "INSERT INTO players (name, password, immlevel, title, experience, room, level, sex, agility, intelligence, strength, vitality, ";
        sql += "wisdom, health, mana, stamina, skills, completed_quests, active_quests, equipped, inventory, class, class_data, recall, is_ghost) values ('";
        sql += name + "','" + password + "'," + Utilities::itos(player->immlevel);
        sql += ",'" + fixtitle + "'," + Utilities::itos(player->experience) + "," + Utilities::itos(room->id);
        sql += "," + Utilities::itos(level) + "," + Utilities::itos(sex) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intelligence) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(vitality) + "," + Utilities::itos(wisdom) + ",";
        sql += Utilities::itos(health) + "," + Utilities::itos(mana) + "," + Utilities::itos(stamina) + ",'";
        
        std::map<std::string, Skill *>::iterator iter;
        for(iter = knownSkills.begin(); iter != knownSkills.end(); ++iter)
        {
            sql += Utilities::itos((*iter).second->id) + ";";
        }
        sql += "','";
        std::map<int, Quest *>::iterator questiter;
        for(questiter = player->completedQuests.begin(); questiter != player->completedQuests.end(); ++questiter)
        {
            sql += Utilities::itos((*questiter).second->id) + ";";
        }
        sql += "','";
        for(int i = 0; i < (int)player->questLog.size(); i++)
        {
            sql += Utilities::itos(player->questLog[i]->id);
            for(int j = 0; j < (int)player->questObjectives[i].size(); j++)
            {
                sql += "," + Utilities::itos(player->questObjectives[i][j]);
            }
            sql += ";";
        }
        sql += "','";
        for(int i = 0; i < (int)player->equipped.size(); i++)
        {
            if(player->equipped[i] != NULL)
            {
                sql += Utilities::itos(player->equipped[i]->id) + ";";
            }
        }
        sql += "','";
        std::list<Item *>::iterator inviter;
        for(inviter = player->inventory.begin(); inviter != player->inventory.end(); ++inviter)
        {
            sql += Utilities::itos((*inviter)->id) + ";";
        }
        sql += "'," + Utilities::itos(player->currentClass->id) + ",'";
        std::list<Player::ClassData>::iterator classiter;
        for(classiter = player->classList.begin(); classiter != player->classList.end(); ++classiter)
        {
            sql += Utilities::itos((*classiter).id) + "," + Utilities::itos((*classiter).level) + ";";
        }
        sql += "',";
		sql += Utilities::itos(player->recall) + ", ";
        
        if(IsGhost() || IsCorpse())
            sql += "1)";
        else
            sql += "0)";

        sql += " ON DUPLICATE KEY UPDATE name=VALUES(name), password=VALUES(password), immlevel=VALUES(immlevel), title=VALUES(title), ";
        sql += "experience=VALUES(experience), room=VALUES(room), level=VALUES(level), sex=VALUES(sex), agility=VALUES(agility), ";
        sql += "intelligence=VALUES(intelligence), strength=VALUES(strength), vitality=VALUES(vitality), wisdom=VALUES(wisdom), ";
        sql += "health=VALUES(health), mana=VALUES(mana), stamina=VALUES(stamina), skills=VALUES(skills), completed_quests=VALUES(completed_quests), ";
        sql += "active_quests=VALUES(active_quests), equipped=VALUES(equipped), inventory=VALUES(inventory), class=VALUES(class), ";
        sql += "class_data=VALUES(class_data), recall=VALUES(recall), is_ghost=VALUES(is_ghost)";

        player->saved = (int)Game::GetGame()->currentTime;
    }
    else
    {
        string fixtitle = Utilities::SQLFixQuotes(title);

        sql = "INSERT INTO npcs (id, name, level, sex, agility, intelligence, strength, vitality, ";
        sql += "wisdom, health, mana, stamina, skills, flags, drops, title, attack_speed, damage_low, damage_high) values (";
        sql += Utilities::itos(id) + ", '";
        sql += name + "'," + Utilities::itos(level) + "," + Utilities::itos(sex) + ",";
        sql += Utilities::itos(agility) + "," + Utilities::itos(intelligence) + "," + Utilities::itos(strength) + ",";
        sql += Utilities::itos(vitality) + "," + Utilities::itos(wisdom) + ",";
        sql += Utilities::itos(health) + "," + Utilities::itos(mana) + "," + Utilities::itos(stamina) + ",'";
        
        std::map<std::string, Skill *>::iterator skilliter;
        for(skilliter = knownSkills.begin(); skilliter != knownSkills.end(); ++skilliter)
        {
            sql += Utilities::itos((*skilliter).second->id) + ";";
        }
        sql += "','";

        std::vector<int>::iterator flagiter;
        for(flagiter = flags.begin(); flagiter != flags.end(); ++flagiter)
        {
            sql += Utilities::itos((*flagiter)) + ";";
        }
        sql += "','";
        
        std::list<DropData>::iterator dropsiter;
        for(dropsiter = drops.begin(); dropsiter != drops.end(); ++dropsiter)
        {
            sql += Utilities::itos((*dropsiter).percent);
            for(int i = 0; i < (int)(*dropsiter).id.size(); i++)
            {
                sql += "," + Utilities::itos((*dropsiter).id[i]);
            }
            sql += ";";
        }

        sql += "','" + fixtitle + "', " + Utilities::dtos(npcAttackSpeed, 2) + ", " + Utilities::itos(npcDamageLow) + ", ";
        sql += Utilities::itos(npcDamageHigh) + ")";

        /*id, name, level, sex, agility, intelligence, strength, vitality, ";
        sql += "wisdom, health, mana, stamina, skills, flags, drops, title, attack_speed, damage_low, damage_high
        */
        sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), level=VALUES(level), sex=VALUES(sex), agility=VALUES(agility), ";
        sql += "intelligence=VALUES(intelligence), strength=VALUES(strength), vitality=VALUES(vitality), wisdom=VALUES(wisdom), ";
        sql += "health=VALUES(health), mana=VALUES(mana), stamina=VALUES(stamina), skills=VALUES(skills), flags=VALUES(flags), ";
        sql += "drops=VALUES(drops), title=VALUES(title), attack_speed=VALUES(attack_speed), damage_low=VALUES(damage_low), ";
        sql += "damage_high=VALUES(damage_high)";

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

    if(player)
    {
        player->SetClassLevel(player->currentClass->id, 
                              Utilities::UMAX(0, player->GetClassLevel(player->currentClass->id) + (newlevel - level)));
        if(player->currentClass->agilityIncrease > 0)
        {
            agility += player->currentClass->agilityIncrease * (newlevel - level);
            Send("|WYour agility increases by " + Utilities::itos(player->currentClass->agilityIncrease*(newlevel - level)) + ".|X\n\r");
        }
        if(player->currentClass->intelligenceIncrease > 0)
        {
            intelligence += player->currentClass->intelligenceIncrease*(newlevel - level);
            Send("|WYour intelligence increases by " + Utilities::itos(player->currentClass->intelligenceIncrease*(newlevel - level)) + ".|X\n\r");
        }
        if(player->currentClass->strengthIncrease > 0)
        {
            strength += player->currentClass->strengthIncrease*(newlevel - level);
            Send("|WYour strength increases by " + Utilities::itos(player->currentClass->strengthIncrease*(newlevel - level)) + ".|X\n\r");
        }
        if(player->currentClass->vitalityIncrease > 0)
        {
            vitality += player->currentClass->vitalityIncrease*(newlevel - level);
            Send("|WYour vitality increases by " + Utilities::itos(player->currentClass->vitalityIncrease*(newlevel - level)) + ".|X\n\r");
        }
        if(player->currentClass->wisdomIncrease > 0)
        {
            wisdom += player->currentClass->wisdomIncrease*(newlevel - level);
            Send("|WYour wisdom increases by " + Utilities::itos(player->currentClass->wisdomIncrease*(newlevel - level)) + ".|X\n\r");
        }
    }
    level = newlevel;
    health = maxHealth = vitality * Character::HEALTH_FROM_VITALITY;
    mana = maxMana = intelligence * Character::MANA_FROM_INTELLIGENCE;
    stamina = maxStamina = strength * Character::STAMINA_FROM_STRENGTH;
    Send("|WYou are now level " + Utilities::itos(newlevel) + ".|X\n\r");
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
                               bool hidden, bool stackable, int ticks, double duration, int category, Skill * sk)
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
            if(!Utilities::str_cmp((*iter)->name, name) && (*iter)->skill == sk)
                return NULL;
        }
    }

    SpellAffect * sa = new SpellAffect();
    sa->name = name;
    sa->hidden = hidden;
    sa->stackable = stackable;
    sa->ticks = ticks;
    sa->duration = duration;
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
    string sql = "DELETE FROM cooldowns WHERE player_name='" + name + "';";
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
        
        sql = "INSERT INTO cooldowns (cooldowns.player_name, cooldowns.skill_id, cooldowns.timestamp) VALUES ('";
        sql += name + "', " + Utilities::itos((*iter).second->id) + ", " + Utilities::dtos((*iter2).second, 2) + ");";
        Server::sqlQueue->Write(sql);
    }
}

void Character::LoadCooldowns()
{
    StoreQueryResult cooldownres = Server::sqlQueue->Read("SELECT * FROM cooldowns WHERE player_name='" + name + "';");
    if(cooldownres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = cooldownres.begin(); i != cooldownres.end(); i++)
    {
        row = *i;
        Skill * sk = Game::GetGame()->GetSkill(row["skill_id"]);
        if(sk != NULL)
        {
            cooldowns[sk->name] = row["timestamp"];
        }
    }

    string sql = "DELETE FROM cooldowns WHERE player_name='" + name + "';";
    Server::sqlQueue->Write(sql);
}

void Character::SaveSpellAffects()
{
    if(!player)
        return;

    string deletesql = "DELETE FROM affects WHERE player_name = '" + name + "';";
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
    if(combat && meleeActive)
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
            damage = (rand() % (npcDamageHigh - npcDamageLow)) + npcDamageLow;
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
                    damage_main = (rand() % (high - low)) + low;
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
                    damage_off = (rand() % (high - low)) + low;
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
        Send("My threat on " + victim->name + " is " + Utilities::itos(victim->GetThreat(this)) + "\n\r");
    }
    
    victim->AdjustHealth(this, -damage);
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

int Character::GetStamina()
{
    return stamina;
}

void Character::AdjustHealth(Character * source, int amount)
{
    if(source == NULL)
    {
        //a possibility
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

	SendSubchannel("tt=\"health\";health=" + Utilities::itos(health));

    if(health == 0) //todo: functionize me. ROM = "ExtractChar"
    {
        Message("|R" + name + " has been slain!|X", Character::MSG_ROOM_NOTCHAR, source);
        Send("|RYou have been slain!|X\n\r");
        if(source->target == this)
        {
            source->meleeActive = false;
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
                if(rand() % 100 <= (*dropiter).percent && (*dropiter).id.size() > 0)
                {
                    int which = rand() % ((int)(*dropiter).id.size());
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
        case 1:
            if(health >= amount)
                return true;
            break;
        case 2:
            if(mana >= amount)
                return true;
            break;
        case 3:
            if(stamina >= amount)
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


void Character::AdjustStamina(Character * source, int amount)
{
    if(source == NULL)
    {
        //a possibility
    }
    if(amount < 0)
    {
        (stamina + amount >= 0) ? stamina += amount : stamina = 0;
    }
    else if(amount > 0)
    {
        (stamina + amount >= maxStamina) ? stamina = maxStamina : stamina += amount;
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
    return (sex == 1 ? "his" : "her");
}

bool Character::CanMove()
{
    if(player && player->IMMORTAL())
        return true;

    int low = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 5); //whatModifier: 1 total, 2 largest, 3 smallest, 4 largest positive, 5 smallest negative
    int high = GetAuraModifier(SpellAffect::AURA_MOVE_SPEED, 4);

    double newspeed = 100 + high + low;

    if(newspeed <= 0) 
        return false;

    newspeed /= 100.0;

    return (Game::currentTime > lastMoveTime + (1.0/(movementSpeed * newspeed)));
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

void Character::SetCooldown(Skill * sk, std::string name, bool global, double length)
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
                //luaL_dostring(Server::luaState, nil.c_str());
				Server::lua.script(trig->GetScript().c_str());
				Server::lua[func.c_str()](this, toroom);
                //luaL_dostring(Server::luaState, trig->GetScript().c_str());
                //luabind::call_function<void>(Server::luaState, func.c_str(), this, toroom);
            }
            catch(const std::exception & e)
			{
				LogFile::Log("error", e.what());
				/*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				if(logstring != NULL)
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
                //luaL_dostring(Server::luaState, nil.c_str());
				Server::lua.script(trig->GetScript().c_str());
				Server::lua[func.c_str()](this, toroom);
                //luaL_dostring(Server::luaState, trig->GetScript().c_str());
                //luabind::call_function<void>(Server::luaState, func.c_str(), this, toroom);
            }
            catch(const std::exception & e)
			{
				LogFile::Log("error", e.what());
				/*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				if(logstring != NULL)
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
