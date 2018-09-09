#ifndef CGROUP_H
#define CGROUP_H

#include <string>

class Character;
class Player;

class Group
{
public:
	const static int MAX_GROUP_SIZE = 4;
	const static int MAX_RAID_SIZE = 24;

	Group(Player * l);
	~Group();

	Player * members[Group::MAX_RAID_SIZE];
	Player * leader;

	int FindNextEmptySlot();
	int FindMemberSlot(Player *ch);
	int FindFirstSlotInSubgroup(Player *ch);
	bool IsSlotEmpty(int slot);
	bool IsSubgroupEmpty(int slot);
	int GetSubgroupCount(int slot);
	bool Add(Player * ch);
	bool Remove(Player *ch);
	void Move(Player * ch, int slot);
	bool IsGroupLeader(Player * ch);
	Player * FindByName(std::string name);
	int GetMemberCount() { return count; };
	bool IsRaidGroup() { return raid; };
	void MakeRaidGroup() { raid = true; };

private:
	int count;
	bool raid;
};

#endif //CGROUP_H