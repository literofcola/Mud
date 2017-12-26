#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"

ListenerManager::~ListenerManager()
{
    std::list<Listener*>::iterator iter = listeners_.begin();
    while(iter != listeners_.end())
    {
        iter = listeners_.erase(iter);
    }
}

void ListenerManager::AddListener(Listener * l )
{
    //l->remove = false;
    if(!HasListener(l))
    {
        listeners_.push_front(l);
    }
}

void ListenerManager::RemoveListener(Listener * l)
{
    //listeners_.remove(l);
    std::list<Listener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); ++iter)
    {
        if((*iter) == l)
        {
            listeners_.erase(iter);
            break;
        }
    }
}

bool ListenerManager::HasListener(Listener * l)
{
    std::list<Listener*>::iterator iter;
    for(iter = listeners_.begin(); iter != listeners_.end(); ++iter)
    {
        if((*iter) == l)
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
    std::list<Listener*>::iterator iter = listeners_.begin();
    while(iter != listeners_.end()) //This form allows for a call to RemoveListener from within Notify, maybe??
    {
        Listener * l = (*iter);
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