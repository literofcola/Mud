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
    //Level up stat bonuses
    int agilityPerLevel;
    int intellectPerLevel;
    int strengthPerLevel;
    int vitalityPerLevel;
    int wisdomPerLevel;

    struct SkillData
    {
        Skill * skill;
        int level;
        int learnCost;
    };
    std::list<SkillData> classSkills;

    bool changed;

    void Save();
    
private:
};
    
#endif