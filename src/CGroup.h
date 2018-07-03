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
	int count;
	bool israid;

	int GetNextFreeSlot();
	int FindCharacterSlot(Character *ch);
	int FindFirstInSubgroup(Character *ch);
	bool IsEmptySubgroup(Character *ch);
	void Remove(Character *ch);
};

#endif //CGROUP_H