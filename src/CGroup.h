#ifndef CGROUP_H
#define CGROUP_H

class Character;

//TODO: pasted this from old Mudv2, needs work
class Group
{
public:
	const static int MAX_GROUP_SIZE = 4;
	const static int MAX_RAID_SIZE = 24;

	Group(Character * l);
	~Group();

	Character * members[Group::MAX_RAID_SIZE];
	Character * leader;
	bool israid;

	int FindNextEmptySlot();
	//int FindNextEmptySlotInSubgroup(int group);
	int FindMemberSlot(Character *ch);
	int FindFirstSlotInSubgroup(Character *ch);
	bool IsSlotEmpty(int slot);
	bool IsSubgroupEmpty(int slot);
	bool Add(Character * ch);
	bool Remove(Character *ch);
	void Move(Character * ch, int slot);
	bool IsGroupLeader(Character * ch);
	Character * FindByName(std::string name);
	int GetMemberCount() { return count; };

private:
	int count;
};

#endif //CGROUP_H