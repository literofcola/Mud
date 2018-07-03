#ifndef CSKILL_H
#define CSKILL_H

class Server;

class Skill
{
public:
    Skill(int id_, std::string long_name_); 
    ~Skill();

    int id;
    std::string name; //keyword name for cmd_cast, unique per character
    std::string long_name; //global unique name
    std::string function_name;
	double castTime;
	double cooldown;
	enum TargetType { TARGET_SELF, TARGET_OTHER, TARGET_HOSTILE, TARGET_ANY, TARGET_FRIENDLY, TARGET_NONE, TARGET_PASSIVE };
	TargetType targetType;
	enum Interrupt { INTERRUPT_MOVE, INTERRUPT_HIT, INTERRUPT_NOPUSHBACK };
	std::bitset<3> interruptFlags;
	std::string description;
	std::string costDescription;
	std::string costFunction;
    std::string castScript;
    std::string applyScript;
    std::string tickScript;
    std::string removeScript;
    
    //bool ignoreGlobal; //TODO, flag to ignore global cooldowns
    bool changed;
    void Save();

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;
    std::map<std::string, double*> doubleTable;

	//This crazyness lets us do: std::find_if(skills.begin(), skills.end(), Skill::CompareSkillToString("Greater Heal"));
	//  to search skills for a specific ->long_name
	struct CompareSkillToString
	{
		CompareSkillToString(std::string val) : val_(val) {}
		bool operator()(const std::pair<int, Skill *>& elem) const;
		//{ return !Utilities::str_cmp(val_, elem.second->long_name); } //in CSkill.cpp
		private:
			std::string val_;
	};

private:
    Skill();
};

#endif //CSKILL_H