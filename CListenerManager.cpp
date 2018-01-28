#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"

ListenerManager::~ListenerManager()
{
    std::list<ListenerCount>::iterator iter = listeners_.begin();
    while(iter != listeners_.end())
    {
        iter = listeners_.erase(iter);
    }
}

void ListenerManager::AddListener(Listener * l )
{
	std::list<ListenerCount>::iterator findIter = std::find(listeners_.begin(), listeners_.end(), l);
	if (findIter != listeners_.end())
	{
		findIter->refcount++;
	}
	else
	{
		listeners_.push_front(l);
	}
}

void ListenerManager::RemoveListener(Listener * l)
{
    std::list<ListenerCount>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); ++iter)
    {
        if((*iter).listener == l)
        {
			iter->refcount--;
			if (iter->refcount == 0)
			{
				listeners_.erase(iter);
				break;
			}
        }
    }
}

bool ListenerManager::HasListener(Listener * l)
{
    std::list<ListenerCount>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); ++iter)
    {
        if((*iter).listener == l)
        {
            return true;
        }
    }
    return false;
}

/*(void ListenerManager::DeferredRemoveListener(Listener * l)
{
    //l->remove = true;
}*/

void ListenerManager::NotifyListeners()
{
    std::list<ListenerCount>::iterator iter = listeners_.begin();
    while(iter != listeners_.end()) //This form allows for a call to RemoveListener from within Notify, maybe??
    {
        Listener * l = (*iter).listener;
        ++iter;
        l->Notify(this);
    }

    /*for(std::list<Listener*>::iterator iter = listeners_.begin(); iter != listeners_.end(); ++iter)
    {
        (*iter)->Notify();
    }*/
    /*std::list<Listener*>::iterator */iter = listeners_.begin();
    /*while(iter != listeners_.end())
    {
        if((*iter)->remove)
        {
            iter = listeners_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }*/
}