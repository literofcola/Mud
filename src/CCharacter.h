#ifndef CCHARACTER_H
#define CCHARACTER_H

class Server;
class User;
class SpellAffect;
class Reset;

class Character : public Subscriber, public SubscriberManager
{
public:
    Character();
    Character(std::string name_, int id_);
    Character(std::string name_, User * user_);
    Character(const Character&);
    ~Character();
    void SetDefaults();
    void Notify(SubscriberManager *);

    enum MessageType
    {
        MSG_CHAR, MSG_CHAR_VICT, MSG_ROOM, MSG_ROOM_NOTCHAR, MSG_ROOM_NOTVICT, MSG_ROOM_NOTCHARVICT, MSG_GROUP, MSG_GROUP_NOTCHAR
    };

    int id;
    int level;
    int gender;

    int agility;	//crit chance and avoidance
    int intellect;  //spell power
    int strength;	//attack power
    int stamina;	//health
    int wisdom;		//mana
	int spirit;		//mana regen

	enum ResourceType
	{
		RESOURCE_HEALTH = 1, RESOURCE_MANA, RESOURCE_ENERGY, RESOURCE_RAGE, RESOURCE_COMBO
	};
    
	Character * comboPointTarget;
    static const int HEALTH_FROM_STAMINA = 10;
    static const int MANA_FROM_WISDOM = 10; //todo: move intellect into class per level up!
	static constexpr double STRENGTH_DAMAGE_MODIFIER = 0.30;	//auto attack damage increased by 30% of strength
    std::string name;
    std::string title;
    Room * room;
	Group * group;
    Player * player;

    int race; //index into Character::race_table
	enum Races
	{
		RACE_NONE, RACE_HUMAN, RACE_ELF, RACE_DWARF, RACE_ORC, RACE_GNOME, RACE_GOBLIN, RACE_UNDEAD, RACE_MINOTAUR, RACE_TROLL
	};
    //TODO, make this a class? std::list<Skill*> racials? starting stat bonuses?
    struct RaceType
    {
        int id;
        std::string name;
    };
    static RaceType race_table[];

    //npc only stuff
    struct DropData
    {
        std::vector<int> id;
        int percent;
    };
    std::list<DropData> drops; //TODO Random drops
    Reset * reset; //reset that spawned this npc, if any
    std::vector<Quest *> questStart;
    std::vector<Quest *> questEnd;
	std::string speechText;
	std::string keywords;
    bool changed;
    std::vector<int> flags; //a vector of constants to indicate flag is set

	enum Flags
    {
        FLAG_FRIENDLY,FLAG_NEUTRAL,FLAG_AGGRESSIVE,FLAG_GUILD,FLAG_VENDOR,FLAG_REPAIR,
        FLAG_TRAINER
    };
    struct flag_type
    {
        int flag;
        std::string flag_name;
    };
    static flag_type flag_table[];
    bool remove;

    //Movement
    double movementSpeed; 
    static constexpr double NORMAL_MOVE_SPEED = 1;						//default move speed in rooms per second
    static constexpr double COMBAT_MOVE_SPEED = NORMAL_MOVE_SPEED * .3; //30% of normal
    double lastMoveTime;
    std::deque<void(*)(Character *, std::string)> movementQueue;
	enum Position
	{
		POSITION_ANY, POSITION_SITTING, POSITION_STANDING
	};
	int position; //standing, sitting... only used for eating/drinking right now? (flying?)

    //Combat
    bool meleeActive;
    double lastAutoAttack_off; //Time stamp for melee swing timer
    double lastAutoAttack_main;
    double npcAttackSpeed;
    int npcDamageLow;
    int npcDamageHigh;

    struct LeashData //Keep track of wander distance and leash distance
    { 
        int type;
        Room * room;
        Exit * next;
    };
    std::list<LeashData> leashData;
    std::map<int, Exit*> wanderPath;
    std::list<std::pair<Room *, int>> leashPath;
	Room * leashOrigin;

    struct Threat
    {
        Character * ch;
        int threat;
		int damage;
		int healing;
		bool tapped; //This character has a valid "tap" on us
		enum Type { THREAT_DAMAGE, THREAT_HEALING, THREAT_OTHER };
    };
    std::list<Threat> threatList;

    //Spells/Skills
    struct DelayData
    {
        Character * caster;
        Skill * sk;
        Character * charTarget;
        Item * itemTarget;
    };
    double delay;
    DelayData delayData;
    bool delay_active;
    void (*delayFunction)(DelayData);
    std::list<SpellAffect *> buffs;
    std::list<SpellAffect *> debuffs;
    bool debuffs_invalid;
    bool buffs_invalid;
    std::map<std::string, Skill *> knownSkills;
    std::map<std::string, double> cooldowns;
    double lastSpellCast;  //time stamp for mana regen 5 second rule

    std::map<int, Trigger> triggers;
    void AddTrigger(Trigger & trig);
    Trigger * GetTrigger(int id, int type = -1);

	//this should for sure be in user right...
    enum EditState
    {
        ED_NONE, ED_ROOM, ED_SKILL, ED_NPC, ED_ITEM, ED_QUEST, ED_CLASS, ED_PLAYER, ED_HELP, ED_AREA
    };
    EditState editState;
    void * editData;

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;
    std::map<std::string, double*> doubleTable;

    //TODO sort these functions!
	void ResetMaxStats();
    void GeneratePrompt(double currentTime);
    void QueryClear();
	void SetQuery(std::string prompt, void * data, bool(*queryFunction)(Character *, std::string));
	void * GetQueryData();
	bool HasQuery();
	bool(*GetQueryFunc())(Character *, std::string);
	void SendBW(std::string str);
    void Send(std::string str);
	void Send(char * str);
	void SendGMCP(std::string str);
	void SendGMCP(char * str);
    void Message(const std::string & txt, MessageType msg_type, Character * vict = NULL);
    Character * GetCharacterRoom(std::string name);
	Room * GetRoom() { return room; }
	Item * GetItemRoom(std::string name);
	bool IsItemInRoom(Item * i);
    Character * GetCharacterAdjacentRoom(std::string name, std::string direction);
    Character * GetCharacterRoom(Character * target);
    Character * GetCharacterAdjacentRoom(Character * target);
    void SetTarget(Character * t);
    void ClearTarget();
    Character * GetTarget();
    void Move(int direction);
	void Sit();
	void Stand();
    bool ChangeRoomsID(int roomid);
    bool ChangeRooms(Room * room);
    static Character * LoadPlayer(std::string name, User * user);
    //static Character * LoadNPC(Server_ptr server, int id);
    void Save();
    void SetLevel(int level);
    int GetLevel();
	Player * GetPlayer();
    std::string GetName();
	std::string HisHer();
	bool CancelActiveDelay();
	bool CancelCastOnHit();
	std::string AggressionColor(Character * target);
	std::string AggressionLightColor(Character * target);
    void EnterCombat(Character * victim);
	void EnterCombatAssist(Character * friendly);
    void ExitCombat();
    bool InCombat();
    bool IsFighting(Character * target);
	bool CheckThreatCombat();
    void UpdateThreat(Character * ch, int value, int type);
    int GetThreat(Character * ch);
    Character * GetTopThreat();
    bool HasThreat(Character * ch);
	void RemoveThreat(Character * ch, bool removeall);
	bool HasTap(Character * target);
	Character * GetTap();
    void AutoAttack(Character * victim);
    void OneHit(Character * victim, int damage);
	void OneHeal(Character * target, int heal);
	double GetMainhandWeaponSpeed();
	double GetMainhandDamagePerSecond();
	int GetOffhandDamageRandomHit();
	double GetOffhandDamagePerSecond();
	double GetOffhandWeaponSpeed();
	int GetMainhandDamageRandomHit();
	int GetIntellect();
	int GetStrength();
    int GetHealth();
	int GetMaxHealth();
    int GetMana();
	int GetMaxMana();
	int GetEnergy();
	int GetMaxEnergy();
	int GetRage();
	int GetMaxRage();
	void SetHealth(int amount);
	void SetMana(int amount);
	void SetEnergy(int amount);
	void SetRage(int amount);
	void SetMaxHealth(int amount);
	void SetMaxMana(int amount);
	void SetMaxEnergy(int amount);
	void SetMaxRage(int amount);
	void GenerateRageOnAttack(int damage, double weapon_speed, bool mainhand, bool wascrit);
	void GenerateRageOnTakeDamage(int damage);
	int GetComboPoints();
	void SetComboPoints(int howmany);
	void GenerateComboPoint(Character * target);
	int SpendComboPoints(Character * target);
	void ClearComboPointTarget();
    void ConsumeMana(int amount);
	void ConsumeEnergy(int amount);
	void ConsumeRage(int amount);
	void AdjustHealth(Character * source, int amount);
	void OnDeath();
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
    int CleanseSpellAffect(Character * cleanser, int category, int howMany = -1);
	bool RemoveSpellAffectsByAura(int isDebuff, int auraid);
    void RemoveSpellAffect(int isDebuff, int id);
    void RemoveSpellAffect(int isDebuff, std::string name);
    void RemoveAllSpellAffects();
    void SaveSpellAffects();
    void LoadSpellAffects();
    void SaveCooldowns();
    void LoadCooldowns();
    int GetAuraModifier(int affect, int whatModifier);
    int GetTotalAuraModifier(int affect);
    int GetSmallestAuraModifier(int affect);
    int GetLargestAuraModifier(int affect);
    bool CanMove();
    double GetMoveSpeed();
    void AddSkill(Skill * newskill);
    Skill * GetSkillShortName(std::string name);
    bool HasSkill(Skill * sk);
    bool HasSkillByName(std::string name);
    void RemoveSkill(Skill * sk);
    void RemoveSkill(std::string name);
    void SetCooldown(Skill * sk, std::string name, bool global, double length);
    double GetCooldownRemaining(Skill * sk);
	void AddClassSkills();
	bool HasGroup();
	bool InSameGroup(Character * ch);

    bool IsNPC();
	bool IsPlayer();

	void SetCorpse();
	void SetGhost();
	void SetAlive();
	bool IsCorpse();
	bool IsGhost();
	bool IsAlive();
	int TimeSinceDeath();
    
private:

    Character * target;
	int health;     //current health
	int maxHealth;  //permanent health(stamina*5) + gear and buff bonuses
	int mana;
	int maxMana;
	int energy;
	int maxEnergy;
	int rage;
	int maxRage;
	int comboPoints;
	int maxComboPoints;
	bool combat;

	bool hasQuery;
	bool(*queryFunction)(Character *, std::string);
	void * queryData;
	std::string queryPrompt;

	bool isCorpse;
	double deathTime; //timestamp for res timer and npc corpse decay

};

#endif