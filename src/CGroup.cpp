#include "stdafx.h"
#include "CGroup.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "utils.h"

Group::Group(Player * l)
{
	raid = false;
	leader = l;
	count = 1;
	for (int i = 0; i < MAX_GROUP_SIZE; i++)
	{
		members[i] = nullptr;
	}
	members[0] = l;
}

Group::~Group()
{
}

int Group::FindNextEmptySlot()
{
	int group_size = MAX_GROUP_SIZE;
	if (raid)
		group_size = MAX_RAID_SIZE;

	for (int i = 0; i < group_size; i++)
	{
		if (members[i] == nullptr)
			return i;
	}
	return -1;
}

int Group::FindMemberSlot(Player *ch)
{
	for (int i = 0; i < MAX_RAID_SIZE; i++)
	{
		if (members[i] == ch)
			return i;
	}
	return -1;
}

int Group::FindFirstSlotInSubgroup(Player *ch)
{
	int i;
	for (i = 0; i < MAX_RAID_SIZE; i++)
	{
		if (members[i] == ch)
			break;
	}
	if (i != MAX_RAID_SIZE)
	{
		return (i / MAX_GROUP_SIZE) * MAX_GROUP_SIZE;
	}
	return -1;
}

bool Group::IsSlotEmpty(int slot)
{
	if (slot <= 0 || slot > MAX_RAID_SIZE)
		return false;

	if (members[slot] == nullptr)
		return true;

	return false;
}

bool Group::IsSubgroupEmpty(int slot)
{
	int start = (slot / MAX_GROUP_SIZE) * MAX_GROUP_SIZE;
	for (int i = start; i < start + MAX_GROUP_SIZE; i++)
	{
		if (members[i] != nullptr)
			return false;
	}
	return true;
}

int Group::GetSubgroupCount(int slot)
{
	int count = 0;
	int start = (slot / MAX_GROUP_SIZE) * MAX_GROUP_SIZE;
	for (int i = start; i < start + MAX_GROUP_SIZE; i++)
	{
		if (members[i] != nullptr)
			++count;
	}
	return count;
}

bool Group::Add(Player * ch)
{
	int empty_slot = FindNextEmptySlot();
	if (empty_slot == -1)
		return false;
	members[empty_slot] = ch;
	ch->group = this;
	count++;
	return true;
}

bool Group::Remove(Player *ch)
{
	int char_slot = FindMemberSlot(ch);
	if (char_slot == -1)
		return false;
	members[char_slot] = nullptr;
	ch->group = nullptr;
	count--;
	return true;
}

void Group::Move(Player * ch, int slot) //slot is indexed from 1
{
	int from = FindMemberSlot(ch);
	if (from == -1 || from == slot || slot < 0 || slot >= MAX_RAID_SIZE)
		return;

	members[from] = members[slot];
	members[slot] = ch;
}

bool Group::IsGroupLeader(Player * ch)
{
	if (ch != nullptr && leader != nullptr && ch == leader)
		return true;
	return false;
}

Player * Group::FindByName(std::string name)
{
	for (int i = 0; i < Group::MAX_RAID_SIZE; i++)
	{
		if (members[i] != nullptr && !Utilities::str_cmp(members[i]->GetName(), name))
		{
			return members[i];
		}
	}
	return nullptr;
}

Player * Group::GetMember(int index)
{
    if(index < 0 || index >= Group::MAX_RAID_SIZE)
        return nullptr;
    return members[index];
}
