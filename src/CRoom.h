#ifndef CROOM_H
#define CROOM_H

#include "CExit.h"

class Character;
class Server;
class Trigger;
class Reset;
class SpellAffect;
class Item;
class Skill;

class Room
{
public:
    Room(int id_);
    Room(int id_, std::string name_, std::string description_);
    ~Room();

    int id;
    std::string name;
    std::string description;
    int area;
    int pvp; //TODO: overrides area.pvp
    std::list<Character *> characters;
    Exit * exits[Exit::DIR_LAST];
    std::map<int, Reset *> resets;
    std::map<int, Trigger> triggers;
	std::list<std::pair<Item *, int>> items;
    std::list<SpellAffect *> spell_affects;

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;

    bool changed;

	std::vector<int> flags; //a vector of constants to indicate flag is set

	enum Flags
    {
        FLAG_RECALL, FLAG_NOPVP
    };
    struct flag_type
    {
        int flag;
        std::string flag_name;
    };
    static flag_type flag_table[];

    void Save();
    void AddReset(Reset * reset);
    Reset * GetReset(int id);
    void AddTrigger(Trigger & trig);
    Trigger * GetTrigger(int startid, int type = -1);
    SpellAffect * AddSpellAffect(Character * caster, std::string name, bool hidden, int maxStacks, int ticks, double duration, int category, Skill * sk, std::string affect_description);
    SpellAffect * HasSpellAffect(std::string name);
    SpellAffect * GetFirstSpellAffectWithAura(int aura_id);
    int CleanseSpellAffect(Character * cleanser, int category, int howMany = -1);
    bool RemoveSpellAffectByAura(int auraid);
    void RemoveSpellAffect(int id);
    void RemoveSpellAffect(std::string name);
    void RemoveSpellAffect(SpellAffect * remove);
    void RemoveAllSpellAffects();
    void Message(const std::string & text);
	bool HasLivingCharacters();
	bool HasNonGhostCharacters();
	bool HasCharacters();
	bool HasItem(Item * i);
	bool HasItem(int id);
	bool RemoveItem(Item * i);
	void AddItem(Item * i);
	Item * GetItem(std::string name);
	Character * GetCharacter(std::string name);
	Character * GetCharacter(Character * target);

    bool operator<(const Room& r)const{return id<r.id;}

};

#endif