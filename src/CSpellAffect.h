#ifndef CSPELLAFFECT_H
#define CSPELLAFFECT_H

#include "CSubscriber.h"
#include <string>
#include <map>
#include <list>

class Character;
class Skill;

class SpellAffect : public Subscriber
{
public:
    SpellAffect();
    ~SpellAffect();
    std::string name;
    bool debuff;
    bool hidden;
    int id;
    bool stackable;
    int ticks;
    int ticksRemaining;
    double duration;
	std::string affectDescription;
    double appliedTime;
    Character * caster;
    std::string casterName; //At least save the name if caster becomes an invalid pointer
    Skill * skill;
    int affectCategory;
    bool remove_me;
    
    //std::map<std::string, boost::any> affectData; //some generic data when we need to pass between lua functions
    std::map<std::string, int> affectDataInt;
    std::map<std::string, double> affectDataDouble;
    std::map<std::string, std::string> affectDataString;

    struct CategoryType
    {
        std::string categoryName;
        int category;
    };
    //static const struct CategoryType category_table[];

    struct AuraAffect
    {
        int auraID;
        int modifier;
    };
    std::list<struct AuraAffect> auraAffects;

    struct AuraTable
    {
        std::string affectName;
        int auraID;
    };

    void ApplyAura(int aura_id, int modifier);
    void ApplyAura(std::string aura_name, int modifier);
	bool HasAura(int aura_id);
	void RemoveAura(int aura_id);
    std::string GetCasterName();
    std::string GetAffectCategoryName();
    void SaveDataInt(std::string tag, int val);
    void SaveDataDouble(std::string tag, double val);
    void SaveDataString(std::string tag, std::string val);
    int GetDataInt(std::string tag);
    double GetDataDouble(std::string tag);
    std::string GetDataString(std::string tag);
    void Notify(SubscriberManager *);

    void Save(std::string charname);
    static void Load(Character * ch);

    enum Categorys
    {
        AFFECT_NONE = 0, AFFECT_MAGIC = 1, AFFECT_CURSE = 2, AFFECT_POISON = 3, 
        AFFECT_DISEASE = 4, AFFECT_BLEED = 5, AFFECT_FRENZY = 6, AFFECT_LAST = 7
    };

    enum Auras
    {
        AURA_MOVE_SPEED = 1, AURA_RESOURCE_COST = 2, AURA_EATING = 3, AURA_DRINKING = 4, AURA_TAUNT = 5
    };

	//Comparison functor
	struct CompareAuraByID
	{
		CompareAuraByID(int val) : value(val) {}
		bool operator()(struct AuraAffect & elem) const;
	private:
		int value;
	};

private:
};

#endif //CSPELLAFFECT_H