#ifndef CCLASS_H
#define CCLASS_H

class Skill;

class Class
{
public:

    Class() = delete;
	Class(int id_);
    ~Class();

    struct SkillData
    {
        Skill * skill;
        int level;
    };

	//SkillData Comparison functor
	struct CompareClassSkillBySkillID
	{
		CompareClassSkillBySkillID(int val) : value(val) {}
		bool operator()(struct SkillData & elem) const;
	private:
		int value;
	};
    
	std::string name;
	std::string color;
	std::string items; //starting items
	std::map<int, int> armor; //armor type and level gained //TYPE_ARMOR_CLOTH, TYPE_ARMOR_LEATHER, TYPE_ARMOR_MAIL, TYPE_ARMOR_PLATE
	std::list<SkillData> classSkills;
    bool changed;

	int GetID() { return id; };
    void Save();
	bool HasSkill(int id);
	void AddSkill(int id, int level);
	void RemoveSkill(int id);
	int GetArmorLevel(int armortype);

private:
	int id;
};
    
#endif