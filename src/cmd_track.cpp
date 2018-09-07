#include "stdafx.h"
#include "mud.h"
#include "utils.h"
#include "CCharacter.h"
#include "CPlayer.h"
#include "CExit.h"
#include "CRoom.h"
#include "CGame.h"

using std::string;

Exit::Direction FindDirection(Character * ch, Character * target, int depth);
Exit::Direction FindDirection(Room * start, Room * end, int depth);
int FindDistance(Room * start, Room * end, int maxSearchDist);

void cmd_track(Player * ch, string argument)
{
    string arg1;

    if(argument.empty())
    {
        ch->Send("Track who?\n\r");
        return;
    }

    argument = Utilities::one_argument(argument, arg1);

    Character * target = Game::GetGame()->GetPlayerWorld(ch, arg1);   

    if(target == nullptr)
    {
        ch->Send("Player not found.\n\r");
        return;
    }
    if(ch == target)
    {
        ch->Send("You cannot track yourself.\n\r");
        return;
    }
    if(ch->room == target->room)
    {
        ch->Send("Try looking around.\n\r");
        return;
    }

    HighResTimer timer;
    timer.Reset();
    Exit::Direction dir = FindDirection(ch, target, 30);
    ch->Send("GetDirection took " + Utilities::dtos(timer.ElapsedMillis(), 5) + "ms\n\r");
    
    if(dir == Exit::DIR_LAST)
    {
        ch->Send("Could not find a path.\n\r");
        return;
    }

    ch->Send("You sense a trail " + Exit::exitNames[dir] + " from here.\n\r");
}

Exit::Direction FindDirection(Room * start, Room * end, int depth)
{
	//Depth limited breadth first search

	if(!start || !end)
		return Exit::DIR_LAST;

	static std::map<int, Room *> visited; //Mark a node as visited with its parent Room so we can backtrack
	static std::deque<Room *> searchQ;
	Exit::Direction retval = Exit::DIR_LAST;
	int currentDepth = 0;

	if (start->id == end->id)
	{
		return Exit::DIR_LAST;
	}

	searchQ.push_back(start);
	visited[start->id] = nullptr;
	searchQ.push_back(nullptr); //nullptr as depth marker
	currentDepth++;

	while (/*!searchQ.empty()*/1) //searchQ will always have a room or nullptr
	{
		Room * searchme = searchQ.front();
		searchQ.pop_front();

		if (searchme == nullptr) //depth marker
		{
			if (searchQ.empty()) //if all the queue has is nullptr, we couldn't find it
				break;

			searchQ.push_back(nullptr);
			currentDepth++;
			if (currentDepth > depth)
			{
				//ch->Send("depth limit exceeded " + Utilities::itos(currentDepth) + "\n\r");
				break;
			}
			continue;
		}

		if (end == searchme)
		{
			//find "searchme" in "visited" and trace the path back to our room
			std::map<int, Room *>::iterator iter = visited.find(searchme->id);
			while (iter->second->id != start->id)
			{
				iter = visited.find(iter->second->id);
			}
			for (int i = 0; i < Exit::DIR_LAST; i++)
			{
				if (start->exits[i] && start->exits[i]->to->id == iter->first)
				{
					retval = (Exit::Direction)i;
				}
			}
			visited.clear();
			searchQ.clear();
			return retval;
		}
		for (int i = 0; i < Exit::DIR_LAST; i++)
		{
			if (searchme->exits[i] && searchme->exits[i]->to)
			{
				if (visited.find(searchme->exits[i]->to->id) == visited.end())
				{
					//ch->Send("searching room " + Utilities::itos(searchme->exits[i]->to->id) + "\n\r");
					visited[searchme->exits[i]->to->id] = searchme;
					searchQ.push_back(searchme->exits[i]->to);
				}
			}
		}
	}

	visited.clear();
	searchQ.clear();
	return Exit::DIR_LAST;
}

Exit::Direction FindDirection(Character * ch, Character * target, int depth)
{
	if (!ch->room || !target->room)
		return Exit::DIR_LAST;

	return FindDirection(ch->room, target->room, depth);
}

int FindDistance(Room * start, Room * end, int maxSearchDist)
{
	//Depth limited breadth first search
	if (!start || !end)
		return -1;

	static std::map<int, Room *> visited; //Mark a node as visited with its parent Room so we can backtrack
	static std::deque<Room *> searchQ;
	Exit::Direction retval = Exit::DIR_LAST;
	int currentDepth = 0;

	if (start->id == end->id)
	{
		return 0;
	}

	searchQ.push_back(start);
	visited[start->id] = nullptr;
	searchQ.push_back(nullptr); //nullptr as depth marker
	currentDepth++;

	while (/*!searchQ.empty()*/1) //searchQ will always have a room or nullptr
	{
		Room * searchme = searchQ.front();
		searchQ.pop_front();

		if (searchme == nullptr) //depth marker
		{
			if (searchQ.empty()) //if all the queue has is nullptr, we couldn't find it
				break;

			searchQ.push_back(nullptr);
			currentDepth++;
			if (currentDepth > maxSearchDist)
			{
				//ch->Send("depth limit exceeded " + Utilities::itos(currentDepth) + "\n\r");
				break;
			}
			continue;
		}

		if (end == searchme)
		{   //Found it, we just want the distance not the path
			visited.clear();
			searchQ.clear();
			return currentDepth;
		}
		for (int i = 0; i < Exit::DIR_LAST; i++)
		{
			if (searchme->exits[i] && searchme->exits[i]->to)
			{
				if (visited.find(searchme->exits[i]->to->id) == visited.end())
				{
					//ch->Send("searching room " + Utilities::itos(searchme->exits[i]->to->id) + "\n\r");
					visited[searchme->exits[i]->to->id] = searchme;
					searchQ.push_back(searchme->exits[i]->to);
				}
			}
		}
	}

	visited.clear();
	searchQ.clear();
	return -1;
}