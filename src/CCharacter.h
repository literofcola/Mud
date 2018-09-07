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
    Character(const Character & copy) = delete;
    virtual ~Character();

    void Notify(SubscriberManager *);

    enum MessageType
    {
        MSG_CHAR, MSG_CHAR_VICT, MSG_ROOM, MSG_ROOM_NOTCHAR, MSG_ROOM_NOTVICT, MSG_ROOM_NOTCHARVICT, MSG_GROUP, MSG_GROUP_NOTCHAR
    };
	enum ResourceType
	{
		RESOURCE_HEALTH = 1, RESOURCE_MANA, RESOURCE_ENERGY, RESOURCE_RAGE, RESOURCE_COMBO
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
		bool tapped; //This character has a valid "tap" on us
		enum Type { THREAT_DAMAGE, THREAT_HEALING, THREAT_OTHER };
	};
	struct DelayData
	{
		Character * caster;
		Skill * sk;
		Character * charTarget;
		Item * itemTarget;
	};

	static RaceType race_table[];
	
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

    //Spells/Skills
    double delay;
    DelayData delayData;
    bool delay_active;
    void (*delayFunction)(DelayData);
    std::list<SpellAffect *> buffs;
    std::list<SpellAffect *> debuffs;
    //bool debuffs_invalid;
    //bool buffs_invalid;
    std::map<int, double> cooldowns;
    double lastSpellCast;  //time stamp for mana regen 5 second rule
	double lastAutoAttack_main;

	bool remove;

	//These aren't really in the spirit of inheritance but this whole OO design is bad so who cares
	virtual bool IsNPC() = 0;
	virtual bool IsPlayer() = 0;
	virtual NPCIndex * GetNPCIndex() { return nullptr; };

	virtual void SendBW(std::string str) {};
	virtual void Send(std::string str) {};
	virtual void Send(char * str) {};
	virtual void SendGMCP(std::string str) {};
	virtual void SendGMCP(char * str) {};
	void SendTargetSubscriberGMCP(std::string str);
	void SendTargetTargetSubscriberGMCP(std::string str);
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
	//virtual void GetAgility() { };

    void Message(const std::string & txt, MessageType msg_type, Character * vict = nullptr);

	virtual Group * GetGroup() { return nullptr; };

    Character * GetCharacterRoom(std::string name);
	Room * GetRoom() { return room; }
	Item * GetItemRoom(std::string name);
	bool IsItemInRoom(Item * i);
    Character * GetCharacterAdjacentRoom(std::string name, std::string direction);
    Character * GetCharacterRoom(Character * target);
    Character * GetCharacterAdjacentRoom(Character * target);

	virtual double GetGlobalCooldown() { return 0; };
	virtual void SetGlobalCooldown(double time) { };

    void SetTarget(Character * t);
    void ClearTarget();
    Character * GetTarget();
    void Move(int direction);
	virtual void Stand() { };
	virtual void Sit() { };
	virtual bool FlagIsSet(const int flag) { return false; };

    bool ChangeRoomsID(int roomid);
    bool ChangeRooms(Room * room);
	virtual void Save() { };
    
    virtual int GetLevel() = 0;
	virtual bool IsImmortal() { return false; };
	virtual int GetImmLevel() { return 0; };
	virtual std::string GetName() = 0;
	inline virtual int GetGender() = 0;
	inline virtual std::string GetTitle() = 0;
	std::string HisHer();
	std::string HimHer();
	std::string HisHers();
	bool CancelActiveDelay();
	bool CancelCastOnHit();
	std::string AggressionColor(Character * target);
	std::string AggressionLightColor(Character * target);
	bool CanAttack(Character * victim);
    void EnterCombat(Character * victim);
	void EnterCombatAssist(Character * friendly);
    void ExitCombat();
    bool InCombat();
    bool IsFighting(Character * target);
	bool CheckThreatCombat();
    void UpdateThreat(Character * ch, double value, int type);
    double GetThreat(Character * ch);
    Character * GetTopThreat();
    bool HasThreat(Character * ch);
	void RemoveThreat(Character * ch, bool removeall);
	bool HasTap(Character * target);
	Character * GetTap();
    void AutoAttack(Character * victim);
    void OneHit(Character * victim, int damage);
	void OneHeal(Character * target, int heal);
	
	

	

	void GenerateRageOnAttack(int damage, double weapon_speed, bool mainhand, bool wascrit);
	void GenerateRageOnTakeDamage(int damage);
	
	void SetComboPoints(int howmany);
	void GenerateComboPoint(Character * target);
	int SpendComboPoints(Character * target);
	void ClearComboPointTarget();
    void ConsumeMana(int amount);
	void ConsumeEnergy(int amount);
	void ConsumeRage(int amount);
	void AdjustHealth(Character * source, int amount);
	void OnDeath();
	//void DoLootRoll(NPC::OneLoot * oneloot);
	
	virtual void RemoveAllLootRolls() { };
	virtual void RemoveAllLooters() { };
	//void SetRollType(Character * who, int corpse_id, int type); //Set a looter's roll type in the corpse's loot object
	void MakeCorpse();
	void RemoveCorpse();
    void AdjustMana(Character * source, int amount);
	void AdjustEnergy(Character * source, int amount);
	void AdjustRage(Character * source, int amount);
	bool HasResource(int which, int amount);
    void ApplyExperience(int amount);
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
    bool CanMove();
    double GetMoveSpeed();
    
    virtual bool HasSkill(Skill * sk) = 0;
	virtual bool HasSkillByName(std::string name) = 0;
	virtual void AddSkill(Skill * newskill) = 0;
	virtual void RemoveSkill(Skill * sk) = 0;
	virtual void RemoveSkill(std::string name) = 0;
	virtual Skill * GetSkillShortName(std::string name) = 0;

	void StartGlobalCooldown();
    void SetCooldown(Skill * sk, double length);
    double GetCooldownRemaining(Skill * sk);
	void AddClassSkills();
	virtual bool HasGroup() { return false; };
	bool InSameGroup(Character * ch);

	virtual void SetCorpse();
	virtual void SetGhost() { };
	virtual void SetAlive();
	bool IsCorpse();
	inline virtual bool IsGhost() { return false; };
	virtual bool IsAlive() = 0;
	int TimeSinceDeath();
    
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