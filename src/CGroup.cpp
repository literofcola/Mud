#include "stdafx.h"

using namespace std;

//TODO: pasted this from old Mudv2, needs work
Group::Group(Character * l)
{
	israid = false;
	leader = l;
	count = 1;
	for (int i = 0; i < MAX_GROUP_SIZE; i++)
	{
		members[i] = NULL;
	}
	members[0] = l;
}

Group::~Group()
{
}

int Group::GetNextFreeSlot()
{
	int group_size = MAX_GROUP_SIZE;
	if (israid)
		group_size = MAX_RAID_SIZE;

	for (int i = 0; i < group_size; i++)
	{
		if (members[i] == nullptr)
			return i;
	}
	return -1;
}

int Group::FindCharacterSlot(Character *ch)
{
	for (int i = 0; i < MAX_RAID_SIZE; i++)
	{
		if (members[i] == ch)
			return i;
	}
	return -1;
}

int Group::FindFirstInSubgroup(Character *ch)
{
	int i;
	for (i = 0; i < MAX_RAID_SIZE; i++)
	{
		if (members[i] == ch)
			break;
	}
	if (i != MAX_RAID_SIZE)
	{
		if (i % MAX_GROUP_SIZE == 0)
			return i;
		return (i / MAX_GROUP_SIZE) * MAX_GROUP_SIZE;
	}

	return -1;
}

bool Group::IsEmptySubgroup(Character *ch)
{
	if (!ch->group)
		return false;

	int start = (FindCharacterSlot(ch) / MAX_GROUP_SIZE) * MAX_GROUP_SIZE;
	for (int i = start; i < start + MAX_GROUP_SIZE; i++)
	{
		if (members[i] != NULL)
			return false;
	}
	return true;
}

bool Group::Add(Character * ch)
{
	int empty_slot = GetNextFreeSlot();
	if (empty_slot == -1)
		return false;
	members[empty_slot] = ch;
	ch->group = this;
	count++;
	return true;
}

bool Group::Remove(Character *ch)
{
	int char_slot = FindCharacterSlot(ch);
	if (char_slot == -1)
		return false;
	members[char_slot] = nullptr;
	ch->group = nullptr;
	count--;
	return true;
}

bool Group::IsGroupLeader(Character * ch)
{
	if (ch != nullptr && leader != nullptr && ch == leader)
		return true;
	return false;
}

Character * Group::FindByName(std::string name)
{
	for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
	{
		if (members[i] != nullptr && !Utilities::str_cmp(members[i]->name, name))
		{
			return members[i];
		}
	}
	return nullptr;
}

