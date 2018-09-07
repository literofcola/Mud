#ifndef CNPCINDEX_H
#define CNPCINDEX_H

#include <string>
#include <vector>
#include <list>
#include <map>

class Quest;
class Skill;
class Trigger;

class NPCIndex
{
public:
	NPCIndex() = delete;
	NPCIndex(int id_, std::string name_);
	NPCIndex(NPCIndex & copy) = delete;
	~NPCIndex();

	enum Flags
	{
		FLAG_FRIENDLY, FLAG_NEUTRAL, FLAG_AGGRESSIVE, FLAG_GUILD, FLAG_VENDOR, FLAG_REPAIR,
		FLAG_TRAINER
	};
	struct flag_type
	{
		int flag;
		std::string flag_name;
	};
	struct DropData
	{
		std::vector<int> id;
		int percent;
	};

	static flag_type flag_table[];

	int id;
	std::string name;
	std::string keywords;
	int level;
	int gender;
	int race; //index into Character::race_table
	int maxHealth;
	int maxMana;
	int maxEnergy;
	int maxRage;
	std::string title;
	double npcAttackSpeed;
	int npcDamageLow;
	int npcDamageHigh;
	std::vector<int> flags; //a vector of constants to indicate flag is set
	std::string speechText;
	
	std::list<DropData> drops; //TODO Random drops

	std::vector<Quest *> questStart;
	std::vector<Quest *> questEnd;

	std::map<int, Trigger> triggers;

	std::map<std::string, Skill *> knownSkills;
	
	bool changed;
	bool remove;

	//For search  Store a reference to all searchable class data by type
	std::map<std::string, std::string*> stringTable;
	std::map<std::string, int*> intTable;
	std::map<std::string, double*> doubleTable;

	void Save();
	void AddTrigger(Trigger & trig);
	Trigger * GetTrigger(int id, int type = -1);
	bool FlagIsSet(const int flag);

	bool HasSkill(Skill * sk);
	bool HasSkillByName(std::string name);
	void AddSkill(Skill * newskill);
	void RemoveSkill(Skill * sk);
	void RemoveSkill(std::string name);
	Skill * GetSkillShortName(std::string name);
};

#endif