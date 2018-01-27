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
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "utils.h"

using namespace std;

Player::Player(User * user_)
{
    saved = -1;
    immlevel = 0;
    experience = 0;
	statPoints = 0;
	recall = 0;
    isGhost = false;
    isCorpse = false;
    user = user_;
    lastCombatAction = 0;
    inventorySize = 0;
    maxInventorySize = 16;
    equipped.resize(Player::EQUIP_LAST, NULL);
}

Player::~Player()
{
    for(int i = 0; i < (int)equipped.size(); i++)
    {
        if(equipped[i] != NULL)
        {
            delete equipped[i];
            equipped[i] = NULL;
        }
    }
    equipped.clear();

    std::list<Item *>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        delete (*iter);
    }
    inventory.clear();
}

void Player::SetExperience(int newexp)
{
    if(newexp < 0)
        return;

    if(newexp > Game::ExperienceForLevel(Game::MAX_LEVEL))
    {
        experience = Game::ExperienceForLevel(Game::MAX_LEVEL);
        return;
    }
    experience = newexp;
}

bool Player::QuestEligible(Quest * quest)
{
    //check level
    if(user && user->character && user->character->level < quest->levelRequirement)
    {
        return false;
    }

    //check if we've already accepted this quest
    if(QuestActive(quest))
        return false;

    //check if we've done it already
    if(QuestCompleted(quest))
        return false;

    //check if we have any prereq
    if(quest->questRequirement != 0)
    {
        Quest * prereq = Game::GetGame()->GetQuest(quest->questRequirement);
        if(prereq != NULL)
        {
            if(!QuestCompleted(prereq))
                return false;
        }
    }

    //check if we're on a quest that WOULD BE restricted if we accepted this quest
    std::vector<Quest *>::iterator questiter;
    for(questiter = questLog.begin(); questiter != questLog.end(); ++questiter)
    {
        if((*questiter)->questRestriction == quest->id)
            return false;
    }

    //check if we've completed a quest that restricts this quest
    if(quest->questRestriction != 0)
    {
        Quest * restrict = Game::GetGame()->GetQuest(quest->questRestriction);
        if(restrict != NULL)
        {
            if(QuestActive(restrict) || QuestCompleted(restrict))
                return false;
        }
    }

    return true;
}

bool Player::QuestActive(Quest * quest)
{
    std::vector<Quest *>::iterator questiter;
    for(questiter = questLog.begin(); questiter != questLog.end(); ++questiter)
    {
        if((*questiter)->id == quest->id)
            return true;
    }
    return false;
}

bool Player::QuestCompleted(Quest * quest)
{
    std::set<int>::iterator qmapiter;
    qmapiter = completedQuests.find(quest->id);
    if(qmapiter != completedQuests.end())
        return true;
    return false;
}

void Player::QuestCompleteObjective(int type, void * obj)
{
    if(questLog.empty())
        return;

    std::vector<Quest *>::iterator logiter;
    int i = 0;
    for(logiter = questLog.begin(); logiter != questLog.end(); ++logiter)
    {
        Quest * q = (*logiter);

        std::vector<Quest::QuestObjective>::iterator objiter;
        int j = 0;
        for(objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
        {
            if(questObjectives[i][j] < (*objiter).count && (*objiter).type == type)
            {
                switch(type)
                {
                    case Quest::OBJECTIVE_ITEM: 
                    {
                        Item * founditem = (Item*)obj;
                        Item * questitem = (Item*)((*objiter).objective);
                        if(founditem && questitem && founditem->id == questitem->id)
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\n\r\n\r");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_KILLNPC: 
                    {
                        Character * npc = (Character*)obj;
                        Character * questnpc = (Character*)((*objiter).objective);
                        if(npc && questnpc && npc->id == questnpc->id)
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\n\r\n\r");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_ROOM: 
                    {
                        Room * inroom = (Room*)obj;
                        Room * questroom = (Room*)((*objiter).objective);
                        if(inroom && questroom && inroom->id == questroom->id)
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\n\r\n\r");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_VISITNPC: 
                        break;
                }                
            }
            j++;
        }
        i++;
    }
}

bool Player::QuestObjectivesComplete(Quest * quest)
{
    if(questLog.empty())
        return false;

    std::vector<Quest *>::iterator logiter;
    int i = 0;
    for(logiter = questLog.begin(); logiter != questLog.end(); ++logiter)
    {
        Quest * q = (*logiter);

        if(q->id == quest->id)
        {
            std::vector<Quest::QuestObjective>::iterator objiter;
            int j = 0;
            for(objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
            {
                if(questObjectives[i][j] < (*objiter).count)
                {
                    return false;
                }
                j++;
            }
            return true;
        }
        i++;
    }
    return false;
}

Item * Player::NewItemInventory(Item * itemindex)
{
    Item * ret = new Item(*itemindex);
    inventory.push_back(ret);
    inventorySize++;
    //Check if this item is a quest objective
    QuestCompleteObjective(Quest::OBJECTIVE_ITEM, (void*)itemindex);
    return ret;
}

void Player::AddItemInventory(Item * item)
{
    inventory.push_front(item);
    inventorySize++;
    QuestCompleteObjective(Quest::OBJECTIVE_ITEM, (void*)item);
}

Item * Player::GetItemInventory(int id)
{
    std::list<Item*>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if((*iter)->id == id)
        {
            return (*iter);
        }
    }
    return NULL;
}

Item * Player::GetItemInventory(string name)
{
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    std::list<Item*>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(Utilities::IsName(name, (*iter)->name))
        {
            if(ctr == numberarg)
                return (*iter);
            ctr++;
        }
    }
    return NULL;
}

Item * Player::RemoveItemInventory(int id)
{
    std::list<Item*>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if((*iter)->id == id)
        {
            Item * ret = (*iter);
            inventory.erase(iter);
            inventorySize--;
            return ret;
        }
    }
    return NULL;
}

Item * Player::RemoveItemInventory(string name)
{
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    std::list<Item*>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(Utilities::IsName(name, (*iter)->name))
        {
            if(ctr == numberarg)
            {
                Item * ret = (*iter);
                inventory.erase(iter);
                inventorySize--;
                return ret;
            }
            ctr++;
        }
    }
    return NULL;
}

bool Player::RemoveItemInventory(Item * item)
{
    std::list<Item*>::iterator iter;
    for(iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if((*iter) == item)
        {
            Item * ret = (*iter);
            inventory.erase(iter);
            inventorySize--;
            return true;
        }
    }
    return false;
}

int Player::GetEquippedItemIndex(string name)
{
    for(int i = 0; i < Player::EQUIP_LAST; i++)
    {
        if(equipped[i] && Utilities::IsName(name, equipped[i]->name))
        {
            //return equipped[i];
            return i;
        }
    }
    return Player::EQUIP_LAST;
}

Item * Player::GetItemEquipped(string name)
{
    for(int i = 0; i < Player::EQUIP_LAST; i++)
    {
        if(equipped[i] && Utilities::IsName(name, equipped[i]->name))
        {
            return equipped[i];
        }
    }
    return NULL;
}

//Ignores inventory limit
Item * Player::RemoveItemEquipped(int index)
{
    Item * remove = equipped[index];
    equipped[index] = NULL;
    return remove;
}

//return the equip location for this item given what the player is currently wearing
// unfortunately different from equip->equipLocation, especially for weapons, double slot items (ring trinket)
int Player::GetEquipLocation(Item * equip)
{
    int equipSlot = Player::EQUIP_LAST;
    switch(equip->equipLocation)
    {
        case Item::EQUIP_BACK:
        case Item::EQUIP_CHEST:
        case Item::EQUIP_FEET:
        case Item::EQUIP_HANDS:
        case Item::EQUIP_HEAD:
        case Item::EQUIP_LEGS:
        case Item::EQUIP_NECK:
        case Item::EQUIP_WAIST:
        case Item::EQUIP_WRIST:
        case Item::EQUIP_SHOULDER:
            equipSlot = equip->equipLocation - 1; //If we change Item::EquipLocation or Player::EquipmentSlot, we'll be back here when stuff goes weird 
            break;
        case Item::EQUIP_FINGER:
            if(equipped[Player::EQUIP_FINGER1] == NULL)
                equipSlot = Player::EQUIP_FINGER1;
            else if(equipped[Player::EQUIP_FINGER2] == NULL)
                equipSlot = Player::EQUIP_FINGER2;
            else
                equipSlot = Player::EQUIP_FINGER1;
            break;
        case Item::EQUIP_TRINKET:
            if(equipped[Player::EQUIP_TRINKET1] == NULL)
                equipSlot = Player::EQUIP_TRINKET1;
            else if(equipped[Player::EQUIP_TRINKET2] == NULL)
                equipSlot = Player::EQUIP_TRINKET2;
            else
                equipSlot = Player::EQUIP_TRINKET1;
            break;
        case Item::EQUIP_MAINHAND:
            equipSlot = Player::EQUIP_MAINHAND;
            break;
        case Item::EQUIP_ONEHAND:
            if(equipped[Player::EQUIP_MAINHAND] != NULL && equipped[Player::EQUIP_MAINHAND]->equipLocation != Item::EQUIP_TWOHAND)
                equipSlot = Player::EQUIP_OFFHAND;
            else
                equipSlot = Player::EQUIP_MAINHAND;
            break;
        case Item::EQUIP_OFFHAND: 
            equipSlot = Player::EQUIP_OFFHAND;
            break;
        case Item::EQUIP_TWOHAND:
            equipSlot = Player::EQUIP_MAINHAND;
            break;
    }

    /*if(equipSlot == Player::EQUIP_LAST)
    {
        LogFile::Log("error", "Player::EquipItemFromInventory, bad wear_location");
    }*/

    return equipSlot;
}

bool Player::EquipItemFromInventory(Item * wear, int equipSlot)
{
	if (equipSlot < 0 || equipSlot >= Player::EQUIP_LAST)
	{
		LogFile::Log("error", "EquipItemFromInventory, invalid equipSlot");
		return false;
	}

    if(equipped[equipSlot] != NULL)
    {
        LogFile::Log("error", "EquipItemFromInventory, equipped[equipSlot] != NULL");
        return false;
    }
    RemoveItemInventory(wear);
    equipped[equipSlot] = wear;
    return true;
}

void Player::AddClass(int id, int level)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == id && (*iter).level < level)
        {
            (*iter).level = level;
            return;
        }
    }
    ClassData cd;
    cd.id = id;
    cd.level = level;
    classList.push_back(cd);
}

int Player::GetClassLevel(int classid)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == classid)
        {
            return (*iter).level;
        }
    }
    return 0;
}

void Player::SetClassLevel(int classid, int newlevel)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == classid)
        {
            (*iter).level = newlevel;
            return;
        }
    }
    AddClass(classid, newlevel);
}
