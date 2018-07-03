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
	for (int i = 0; i < MAX_RAID_SIZE; i++)
	{
		if (members[i] == NULL)
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

void Group::Remove(Character *ch)
{
	members[FindCharacterSlot(ch)] = NULL;
	count--;
}

