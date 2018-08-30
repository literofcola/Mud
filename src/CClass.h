#ifndef CCLASS_H
#define CCLASS_H

class Class
{
public:

    Class();
    ~Class();

    int id;
    std::string name;
    std::string color;
    std::string items; //starting items
	std::map<int, int> armor; //armor type and level gained //TYPE_ARMOR_CLOTH, TYPE_ARMOR_LEATHER, TYPE_ARMOR_MAIL, TYPE_ARMOR_PLATE

    struct SkillData
    {
        Skill * skill;
        int level;
    };
    std::list<SkillData> classSkills;

    bool changed;

    void Save();
	bool HasSkill(int id);
	void AddSkill(int id, int level);
	void RemoveSkill(int id);
	int GetArmorLevel(int itemtype);

	//Comparison functor
	struct CompareClassSkillBySkillID
	{
		CompareClassSkillBySkillID(int val) : value(val) {}
		bool operator()(struct SkillData & elem) const;
	private:
		int value;
	};
    
private:
};
    
#endif