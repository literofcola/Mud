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
    std::string items;

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