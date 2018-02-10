#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
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

class Character;
class Reset;
class SpellAffect;

SubscriberManager::~SubscriberManager()
{
    std::list<SubscriberCount>::iterator iter = subscribers_.begin();
    while(iter != subscribers_.end())
    {
        iter = subscribers_.erase(iter);
    }
}

void SubscriberManager::AddSubscriber(Subscriber * l )
{
	std::list<SubscriberCount>::iterator findIter = std::find(subscribers_.begin(), subscribers_.end(), l);
	if (findIter != subscribers_.end())
	{
		findIter->refcount++;
	}
	else
	{
		subscribers_.push_front(l);
	}
}

void SubscriberManager::RemoveSubscriber(Subscriber * l)
{
    std::list<SubscriberCount>::iterator iter;
    for(iter = subscribers_.begin(); iter != subscribers_.end(); ++iter)
    {
        if((*iter).subscriber == l)
        {
			iter->refcount--;
			if (iter->refcount == 0)
			{
				subscribers_.erase(iter);
				break;
			}
        }
    }
}

bool SubscriberManager::HasSubscriber(Subscriber * l)
{
    std::list<SubscriberCount>::iterator iter;
    for(iter = subscribers_.begin(); iter != subscribers_.end(); ++iter)
    {
        if((*iter).subscriber == l)
        {
            return true;
        }
    }
    return false;
}

void SubscriberManager::DebugPrintSubscribers()
{
	//TODO
}

void SubscriberManager::NotifySubscribers()
{
	//If we're a Character about to notify our subscribers, we need to make sure we're not subscribed to anyone else either
	//Clear target, combopointtarget, delaydata, and threat
	//TODO: really this should all be done before every call to NotifySubscribers as a CCharacter class function, not in CSubManager
	Character * submanager_as_char;
	if (submanager_as_char = dynamic_cast<Character*>(this))
	{
		submanager_as_char->ClearTarget();
		submanager_as_char->ClearComboPointTarget();
		submanager_as_char->CancelActiveDelay();
		submanager_as_char->RemoveThreat(nullptr, true);
	}

    std::list<SubscriberCount>::iterator iter = subscribers_.begin();
    while(iter != subscribers_.end()) //This form allows for a call to RemoveSubscriber from within Notify, maybe??
    {
        Subscriber * l = (*iter).subscriber;
        ++iter;
        l->Notify(this);
    }

    /*for(std::list<Subscriber*>::iterator iter = subscribers_.begin(); iter != subscribers_.end(); ++iter)
    {
        (*iter)->Notify();
    }*/
    /*std::list<Subscriber*>::iterator *///iter = subscribers_.begin();
    /*while(iter != subscribers_.end())
    {
        if((*iter)->remove)
        {
            iter = subscribers_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }*/
}