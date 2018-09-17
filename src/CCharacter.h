#ifndef CCHARACTER_H
#define CCHARACTER_H

#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include <deque>
#include <map>
#include <string>

class Server;
class User;
class SpellAffect;
class Reset;
class NPC;
class Skill;
class Item;
class Room;
class NPCIndex;
class Group;
class Player;

class Character : public Subscriber, public SubscriberManager
{
public:
	Character();
    Character(const Character & copy) = delete; //I probably don't know the real implications of doing this
    virtual ~Character();

    enum MessageType
    {
        MSG_CHAR, MSG_CHAR_VICT, MSG_ROOM, MSG_ROOM_NOTCHAR, MSG_ROOM_NOTVICT, MSG_ROOM_NOTCHARVICT, MSG_GROUP, MSG_GROUP_NOTCHAR
    };
	enum ResourceType
	{
		RESOURCE_HEALTH = 1, RESOURCE_MANA, RESOURCE_ENERGY, RESOURCE_RAGE, RESOURCE_COMBO
	};
	enum AttackType
	{
		ATTACK_MISS = 1, ATTACK_DODGE, ATTACK_PARRY, ATTACK_BLOCK, ATTACK_CRIT, ATTACK_HIT, ATTACK_RESIST, ATTACK_ABSORB
	};
	enum Races
	{
		RACE_NONE, RACE_HUMAN, RACE_ELF, RACE_DWARF, RACE_ORC, RACE_GNOME, RACE_GOBLIN, RACE_UNDEAD, RACE_MINOTAUR, RACE_TROLL
	};
	struct RaceType
	{	
		//TODO, make this a class? std::list<Skill*> racials? starting stat bonuses?
		int id;
		std::string name;
	};
	struct Threat
	{
		Character * ch;
		double threat;
		double damage;
		double healing;
		bool tapped; //This 'ch' has a valid "tap" on us
		enum Type { THREAT_DAMAGE, THREAT_HEALING, THREAT_OTHER };
	};
	struct DelayData
	{
		Character * caster;
		Skill * sk;
		Character * charTarget;
		Item * itemTarget;
	};

	//DATA
    //Movement
	Room * room;
    double movementSpeed; 
    static constexpr double NORMAL_MOVE_SPEED = 1;						//default move speed in rooms per second
    static constexpr double COMBAT_MOVE_SPEED = NORMAL_MOVE_SPEED * .3; //30% of normal
    double lastMoveTime;
    std::deque<void(*)(Player *, std::string)> movementQueue;
	
    //Combat
    bool meleeActive;
    std::list<Threat> threatList;
	static constexpr double BASE_MISS_CHANCE = 5.00;
	static constexpr double CRIT_DAMAGE_BONUS = 1.5;
	static constexpr int CRIT_MAX = 75;
	static constexpr int DODGE_MAX = 50;
	static constexpr double ARMOR_MITIGATION_MAX = 0.75;

    //Spells/Skills
    double delay;
    DelayData delayData;
    bool delay_active;
    void (*delayFunction)(DelayData);
    std::list<SpellAffect *> buffs;
    std::list<SpellAffect *> debuffs;
    std::map<int, double> cooldowns;
    double lastSpellCast;  //time stamp for mana regen 5 second rule
	double lastAutoAttack_main;

	//misc
	bool remove;
	static RaceType race_table[];

	//FUNCTIONS
	//Movement
	void Move(int direction);
	bool CanMove();
	double GetMoveSpeed();
	bool ChangeRoomsID(int roomid);
	bool ChangeRooms(Room * room);
	virtual void Stand() { };
	virtual void Sit() { };
	virtual int GetRecall() { return 0; };

	//Combat
	void SetTarget(Character * t);
	void ClearTarget();
	Character * GetTarget();
	virtual double GetMainhandWeaponSpeed() = 0;
	virtual double GetOffhandWeaponSpeed() = 0;
	virtual double GetMainhandDamagePerSecond() = 0;
	virtual int GetOffhandDamageRandomHit() = 0;
	virtual double GetOffhandDamagePerSecond() = 0;
	virtual int GetMainhandDamageRandomHit() = 0;
	void GenerateRageOnAttack(int damage, double weapon_speed, bool mainhand, bool wascrit);
	void GenerateRageOnTakeDamage(int damage);
	bool CanAttack(Character * victim);
	void EnterCombat(Character * victim);
	void EnterCombatAssist(Character * friendly);
	void ExitCombat();
	bool InCombat();
	bool IsFighting(Character * target);
	void AutoAttack(Character * victim);
	void OneHit(Character * victim, int damage);
	void OneHeal(Character * target, int heal);
	bool CheckThreatCombat();
	void UpdateThreat(Character * ch, double value, int type);
	double GetThreat(Character * ch);
	Character * GetTopThreat();
	bool HasThreat(Character * ch);
	void RemoveThreat(Character * ch, bool removeall);
	bool HasTap(Character * target);
	Character * GetTap();
	std::string AggressionColor(Character * target);
	std::string AggressionLightColor(Character * target);
	virtual double GetDodge() = 0;
	virtual double GetCrit() = 0;
	virtual double GetParry() = 0;
	int DoAttackRoll(Character * victim, int school);
	double CalculateArmorMitigation();

	//Skills/Spell Affects
	virtual bool HasSkill(Skill * sk) { return true; };
	virtual bool HasSkillByName(std::string name) { return true; };
	virtual void AddSkill(Skill * newskill) { };
	virtual void RemoveSkill(Skill * sk) { };
	virtual void RemoveSkill(std::string name) { };
	virtual Skill * GetSkillShortName(std::string name) { return nullptr; };
	bool CancelActiveDelay();
	bool CancelCastOnHit();
	void SetCooldown(Skill * sk, double length);
	double GetCooldownRemaining(Skill * sk);
	virtual double GetGlobalCooldown() { return 0; };
	virtual void SetGlobalCooldown(double time) { };
	SpellAffect * AddSpellAffect(int isDebuff, Character * caster, std::string name,
		bool hidden, bool stackable, int ticks, double duration, int category, Skill * sk, std::string affect_description);
	SpellAffect * HasSpellAffect(std::string name);
	SpellAffect * GetFirstSpellAffectWithAura(int aura_id);
	int CleanseSpellAffect(Character * cleanser, int category, int howMany = -1);
	bool RemoveSpellAffectsByAura(int isDebuff, int auraid);
	void RemoveSpellAffect(int isDebuff, int id);
	void RemoveSpellAffect(int isDebuff, std::string name);
	void RemoveAllSpellAffects();
	int GetAuraModifier(int aura_id, int whatModifier);
	int GetTotalAuraModifier(int aura_id);
	int GetSmallestAuraModifier(int aura_id);
	int GetLargestAuraModifier(int aura_id);

	//Stats
	void AdjustMana(Character * source, int amount);
	void AdjustEnergy(Character * source, int amount);
	void AdjustRage(Character * source, int amount);
	bool HasResource(int which, int amount);
	void ConsumeMana(int amount);
	void ConsumeEnergy(int amount);
	void ConsumeRage(int amount);
	void AdjustHealth(Character * source, int amount);
	virtual void SetComboPoints(int howmany) { };
	virtual void GenerateComboPoint(Character * target) { };
	virtual int SpendComboPoints(Character * target) { return 0; };
	virtual void ClearComboPointTarget() { };
	virtual bool HasComboPointTarget() { return false; };
	virtual Character * GetComboPointTarget() { return nullptr; };
	virtual int GetComboPoints() { return 0; };

	//Communication
	void Message(const std::string & txt, MessageType msg_type, Character * vict = nullptr);
	virtual void SendBW(std::string str) {};
	virtual void Send(std::string str) {};
	virtual void Send(char * str) {};
	virtual void SendGMCP(std::string str) {};
	virtual void SendGMCP(char * str) {};
	void SendTargetSubscriberGMCP(std::string str);
	void SendTargetTargetSubscriberGMCP(std::string str);

	//Group
	virtual Group * GetGroup() { return nullptr; };
	virtual bool HasGroup() { return false; };
	bool InSameGroup(Character * ch);

	//Death handling / Loot
	virtual void SetCorpse();
	virtual void SetGhost() { };
	virtual void SetAlive();
	bool IsCorpse();
	virtual bool IsGhost() { return false; };
	virtual bool IsAlive() = 0;
	int TimeSinceDeath();
	void OnDeath();
	virtual void RemoveAllLootRolls() { };
	virtual void RemoveAllLooters() { };

	//Lua friendly "commands"
	virtual void Look(std::string argument) {};
	virtual void Cast(std::string argument) {};

	//Get/Set
	int GetHealth() { return health; };
	int GetMana() { return mana; };
	int GetEnergy() { return energy; };
	int GetRage() { return rage; };
	void SetHealth(int amount);
	void SetMana(int amount);
	void SetEnergy(int amount);
	void SetRage(int amount);
	virtual int GetMaxHealth() = 0;
	virtual int GetMaxMana() = 0;
	virtual int GetMaxEnergy() = 0;
	virtual int GetMaxRage() = 0;
	virtual void SetMaxHealth(int amount) { };
	virtual void SetMaxMana(int amount) { };
	virtual void SetMaxEnergy(int amount) { };
	virtual void SetMaxRage(int amount) { };
	virtual int GetBaseAgility() { return 1; };
	virtual int GetBaseIntellect() { return 1; };
	virtual int GetBaseStrength() { return 1; };
	virtual int GetBaseStamina() { return 1; };
	virtual int GetBaseWisdom() { return 1; };
	virtual int GetBaseSpirit() { return 1; };
	virtual int GetAgility() { return 1; };
	virtual int GetIntellect() { return 1; };
	virtual int GetStrength() { return 1; };
	virtual int GetStamina() { return 1; };
	virtual int GetWisdom() { return 1; };
	virtual int GetSpirit() { return 1; };
	virtual int GetLevel() = 0;
	virtual bool IsImmortal() { return false; };
	virtual int GetImmLevel() { return 0; };
	virtual std::string GetName() = 0;
	virtual int GetGender() = 0;
	virtual std::string GetTitle() = 0;
	Room * GetRoom() { return room; }
	virtual int GetArmor() = 0;
	
	//Utility/Misc
	std::string HisHer();
	std::string HimHer();
	std::string HisHers();
	Item * GetItemRoom(std::string name);
	bool IsItemInRoom(Item * i);
	Character * GetCharacterRoom(std::string name);
	Character * GetCharacterRoom(Character * target);
	Character * GetCharacterAdjacentRoom(std::string name, std::string direction);
	Character * GetCharacterAdjacentRoom(Character * target);
	virtual bool FlagIsSet(const int flag) { return false; };
	virtual void Save() { };
	virtual void Notify(SubscriberManager *);
	//These aren't really in the spirit of inheritance but this whole OO design is bad so who cares
	virtual bool IsNPC() = 0;
	virtual bool IsPlayer() = 0;
	virtual sol::object AsPlayer() { return nullptr; }; //for lua usertype casting
	virtual sol::object AsNPC() { return nullptr; };    //for lua usertype casting
	virtual NPCIndex * GetNPCIndex() { return nullptr; };

protected:

    Character * target;
	int health;     
	int mana;
	int energy;
	int rage;
	
	bool combat;

	bool isCorpse;
	double deathTime; //timestamp for res timer and npc corpse decay
};

#endif