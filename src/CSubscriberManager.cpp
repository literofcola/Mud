#include "stdafx.h"
#include "CSubscriberManager.h"
#include "CSubscriber.h"
#include "CCharacter.h"
#include <string>

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
	/* debug
	Character * submanager_as_char;
	Character * sub_as_char;
	if ((submanager_as_char = dynamic_cast<Character*>(this)) && (sub_as_char = dynamic_cast<Character*>(l)))
	{
		cout << "Trying to add " << sub_as_char->name << " to " << submanager_as_char->name << endl;
	}
	*/

	std::list<SubscriberCount>::iterator findIter = std::find(subscribers_.begin(), subscribers_.end(), l);
	if (findIter != subscribers_.end())
	{
		findIter->refcount++;
		//cout << "manager " << this << " refcount++ for subscriber " << l << " is " << findIter->refcount << endl;
	}
	else
	{
		subscribers_.push_front(l);
		//cout << "manager " << this << " refcount++ for subscriber " << l << " is 0" << endl;
	}
}

void SubscriberManager::RemoveSubscriber(Subscriber * l)
{
	/*Character * submanager_as_char;
	Character * sub_as_char;
	if ((submanager_as_char = dynamic_cast<Character*>(this)) && (sub_as_char = dynamic_cast<Character*>(l)))
	{
		cout << "Trying to remove " << sub_as_char->name << " from " << submanager_as_char->name << endl;
	}*/

	//bool debug_found = false;
    std::list<SubscriberCount>::iterator iter;
    for(iter = subscribers_.begin(); iter != subscribers_.end(); ++iter)
    {
        if((*iter).subscriber == l)
        {
			//debug_found = true;
			iter->refcount--;
			//cout << "manager " << this << " refcount-- for subscriber " << l << " is " << iter->refcount << endl;
			if (iter->refcount == 0)
			{
				subscribers_.erase(iter);
				break;
			}
        }
    }
	/*if (!debug_found)
		cout << "RemoveSubscriber() didn't find the subscriber!" << endl;*/
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

std::string SubscriberManager::DebugPrintSubscribers()
{
	//handling the case where the manager is a Character, and we print all the subscribers that are characters
	std::string mysubscribers;
	Character * submanager_as_char;
	if (submanager_as_char = dynamic_cast<Character*>(this))
	{
		for (auto iter = submanager_as_char->subscribers_.begin(); iter != submanager_as_char->subscribers_.end(); ++iter)
		{
			Character * subscriber_as_char;
			if (subscriber_as_char = dynamic_cast<Character*>(iter->subscriber))
			{
				mysubscribers += subscriber_as_char->GetName() + " ";
			}
		}
	}
	return mysubscribers;
}

void SubscriberManager::NotifySubscribers()
{
    std::list<SubscriberCount>::iterator iter = subscribers_.begin();
    while(iter != subscribers_.end()) //This form allows for a call to RemoveSubscriber from within Notify, maybe??
    {
        Subscriber * l = (*iter).subscriber;
        ++iter;
        l->Notify(this);
    }
}