#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
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

Room::flag_type Room::flag_table[] = 
{
    { Room::FLAG_RECALL, "recall" },
    { Room::FLAG_NOPVP, "nopvp" },
    { -1, "" }
};

Room::Room(int id_)
{
    id = id_;
    name = "";
    description = "";
    changed = false;
    for(int i = 0; i < Exit::DIR_LAST; i++)
    {
        exits[i] = NULL;
    }
    stringTable["name"] = &name;
    stringTable["description"] = &description;
    intTable["area"] = &area;
    intTable["pvp"] = &pvp;
    intTable["id"] = &id; //searching by id seems somewhat silly
}

Room::Room(int id_, string name_, string description_)
{
    id = id_;
    name = name_;
    description = description_;
    changed = false;
    for(int i = 0; i < Exit::DIR_LAST; i++)
    {
        exits[i] = NULL;
    }
    stringTable["name"] = &name;
    stringTable["description"] = &description;
    intTable["area"] = &area;
    intTable["pvp"] = &pvp;
    intTable["id"] = &id; //searching by id seems somewhat silly
}

Room::~Room()
{
    triggers.clear();
    for(int i = 0; i < Exit::DIR_LAST; i++)
    {
        if(exits[i] != NULL)
            delete exits[i];
    }
    for(std::map<int, Reset *>::iterator iter = resets.begin(); iter != resets.end(); ++iter)
    {
        if((*iter).second != NULL)
            delete (*iter).second;
    }
}

void Room::Save()
{
    if(!changed)
        return;

    string fixdescription = Utilities::SQLFixQuotes(description);
    string fixname = Utilities::SQLFixQuotes(name);

    string roomsql = "INSERT INTO rooms (id, name, description, area, flags) values ";
    roomsql += "(" + Utilities::itos(id) + ", '" + fixname + "', '" + fixdescription + "', " + Utilities::itos(area) + ",'";

	std::vector<int>::iterator flagiter;
    for(flagiter = flags.begin(); flagiter != flags.end(); ++flagiter)
    {
        roomsql += Utilities::itos((*flagiter)) + ";";
    }
    roomsql += "')";

    roomsql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), description=VALUES(description), area=VALUES(area), flags=VALUES(flags)";

    Server::sqlQueue->Write(roomsql);

	//save "Exit" data
    for(int i = 0; i < Exit::DIR_LAST; i++)
	{
        if(exits[i] && exits[i]->removeme)
        {
            string exitsql = "DELETE FROM exits where exits.from="+Utilities::itos(id)+" and exits.direction="+Utilities::itos(i);
            Server::sqlQueue->Write(exitsql);
            delete exits[i];
            exits[i] = NULL;
        }
		else if(exits[i] && exits[i]->to)
		{
            string exitsql = "INSERT INTO exits (exits.from, exits.to, exits.direction) values ";
            exitsql += "(" + Utilities::itos(id) + ", " + Utilities::itos(exits[i]->to->id) + ", " + Utilities::itos(i) + ")";
            exitsql += " ON DUPLICATE KEY UPDATE exits.from=VALUES(exits.from), exits.to=VALUES(exits.to), exits.direction=VALUES(exits.direction)";
            Server::sqlQueue->Write(exitsql);
		}
	}
    //save Resets
    std::map<int, Reset*>::iterator resetiter = resets.begin();
    while(resetiter != resets.end())
    {
        Reset * r = (*resetiter).second;
        ++resetiter;
        if(r->removeme)
        {
            string resetsql = "DELETE FROM resets where resets.room_id="+Utilities::itos(id)+" and resets.id="+Utilities::itos(r->id);
            Server::sqlQueue->Write(resetsql);
            resets.erase(r->id);
            if(r->npc)
            {
                //LogFile::Log("status", "Removing listener reset id = " + Utilities::itos(r->id) + " from npc " + r->npc->name);
                r->npc->RemoveListener(r);
            }
            delete r;
            r = NULL;
        }
        else
        {
            int target_id;
            switch(r->type)
            {
                case 1: 
                    target_id = r->npcID;
                    break;
            }
            string resetsql = "INSERT INTO resets (room_id, id, type, target_id, wander_dist, leash_dist, resets.interval) values ";
            resetsql += "(" + Utilities::itos(id) + ", " + Utilities::itos(r->id) + ", " + Utilities::itos(r->type) + ", ";
            resetsql += Utilities::itos(target_id) + ", " + Utilities::itos(r->wanderDistance) + ", ";
            resetsql += Utilities::itos(r->leashDistance) + ", " + Utilities::itos(r->interval) + ")";
            resetsql += " ON DUPLICATE KEY UPDATE room_id=VALUES(room_id), id=VALUES(id), type=VALUES(type), target_id=VALUES(target_id), ";
            resetsql += "wander_dist=VALUES(wander_dist), leash_dist=VALUES(leash_dist), resets.interval=VALUES(resets.interval)";
            Server::sqlQueue->Write(resetsql);
        }
    }

    //save Triggers
    std::map<int, Trigger>::iterator trigiter = triggers.begin();
    while(trigiter != triggers.end())
    {
        Trigger * t = &((*trigiter).second);
        ++trigiter;
        if(t->removeme)
        {
            string triggersql = "DELETE FROM triggers where triggers.parent_type="+Utilities::itos(Trigger::PARENT_ROOM)
                                +" and triggers.parent_id="+Utilities::itos(id)+" and triggers.id="+Utilities::itos(t->id);
            Server::sqlQueue->Write(triggersql);
            triggers.erase(t->id);
            t = NULL;
        }
        else
        {
            string triggersql = "INSERT INTO triggers (parent_id, id, parent_type, type, argument, script, function) values ";
            triggersql += "(" + Utilities::itos(id) + ", " + Utilities::itos(t->id) + ", " + Utilities::itos(Trigger::PARENT_ROOM) + ", ";
            triggersql += Utilities::itos(t->GetType()) + ", '" + Utilities::SQLFixQuotes(t->GetArgument()) + "', '";
            triggersql += Utilities::SQLFixQuotes(t->GetScript()) + "', '" + Utilities::SQLFixQuotes(t->GetFunction()) + "')";

            triggersql += " ON DUPLICATE KEY UPDATE parent_id=VALUES(parent_id), id=VALUES(id), parent_type=VALUES(parent_type), ";
            triggersql += "type=VALUES(type), argument=VALUES(argument), script=VALUES(script), function=VALUES(function)";

            Server::sqlQueue->Write(triggersql);
        }
    }
    changed = false;
}

void Room::AddReset(Reset * reset)
{
    int ctr = 1;
    std::map<int, Reset*>::iterator iter;
    for(iter = resets.begin(); iter != resets.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }
    reset->id = ctr;
    changed = true;
    resets.insert(std::pair<int, Reset *>(reset->id, reset));
}

Reset * Room::GetReset(int id)
{
    std::map<int,Reset *>::iterator it = resets.find(id);
    if(it != resets.end())
        return it->second;
    return NULL;
}

void Room::AddTrigger(Trigger & trig)
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
Trigger * Room::GetTrigger(int id, int type)
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