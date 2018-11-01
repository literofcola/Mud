#ifndef CPLAYER_H
#define CPLAYER_H

#include "CSpellAffect.h"
#include "CCharacter.h"
#include "CServer.h"
#include "mud.h"
#include <set>
#include <vector>
#include <list>

class Class;
class Quest;
class Character;
class User;

class Player : public Character
{
public:
	Player() = delete;
	Player(std::string name_, User * user_);
	Player(const Player & copy) = delete;
    ~Player();

	static const int HEALTH_FROM_STAMINA = 10;
	static const int MANA_FROM_WISDOM = 10;
	static constexpr double DODGE_FROM_AGILITY = 0.035;
	static constexpr double CRIT_FROM_AGILITY = 0.03;
	static constexpr double STRENGTH_DAMAGE_MODIFIER = 0.10;	//auto attack damage increased by 10% of strength
	static const int GROUP_LOOT_DISTANCE = 10;
	static const int DEFAULT_DEATH_TIME = 600;
	static const int DEFAULT_DEATH_TIME_RUNBACK = 120;
	static const int STATS_PER_LEVEL = 3;
	static const int DB_INVENTORY_EQUIPPED = 0;
	static const int DB_INVENTORY_INVENTORY = 1;
	static const int DB_INVENTORY_BANK = 2;
	static const int QUESTLOG_MAX_SIZE = 20;

	enum Position
	{
		POSITION_ANY, POSITION_SITTING, POSITION_STANDING
	};
	enum EditState
	{
		ED_NONE, ED_ROOM, ED_SKILL, ED_NPC, ED_ITEM, ED_QUEST, ED_CLASS, ED_PLAYER, ED_HELP, ED_AREA
	};
	enum EquipmentSlot
	{
		EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
		EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER1, EQUIP_FINGER2, EQUIP_TRINKET1, EQUIP_TRINKET2,
		EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_LAST
	};
	struct LootRoll //A piece of loot in a corpse somewhere that we're rolling on
	{
		int my_id;
		int corpse_id;
		NPC * corpse;
	};
	struct ClassData
	{
		int id;
		int level;
	};

	//DATA
	//Stats
	
	int statPoints;

	//Combat
	Character * comboPointTarget;
	double lastAutoAttack_off;	//Time stamp for melee swing timer
	//double lastAutoAttack_main; //moved to Character
	double lastCombatAction; //Timestamp for pvp combat flag

	//Movement
	int position;	//standing, sitting... only used for eating/drinking right now? (flying?)
	int recall; //ID of recall room

	//Skills / Spell Affects
	double globalCooldown;
	std::map<std::string, Skill *> knownSkills;

	//Death Handling
	int death_timer;
	int death_timer_runback;
	int graveyard_room; //graveyard room ID copied from the area death_room. prevent graveyard hopping to other areas
	int corpse_room; //room our corpse is in

	//Group
	Group * group;

	//Inventory and equipment
	std::vector<Item *> equipped;
	std::list<std::pair<Item *, int>> inventory;	//structured this way to support item stacks
	int inventorySize;
	int maxInventorySize; //new players start with 16 slots
	std::list<LootRoll> pending_loot_rolls; //pointer back to a corpse that has uncommon+ loot we're rolling on

	//Class
	Class * currentClass;
	std::list<ClassData> classList;

	//Quest
	std::set<int> completedQuests;
	std::vector<Quest *> questLog;
	std::vector< std::vector<int> > questObjectives; //Maps to questLog[i]->objectives[j]

	//Misc data
	User * user;
	EditState editState;
	void * editData;
	std::string password;
	std::string pwtemp;
	bool prompt;
	std::map<std::string, std::string> alias;
	int saved;
	int race; //index into Character::race_table
	int experience;
	std::string title;
	
	//FUNCTIONS
	//Stats
	void ResetMaxStats();
	void AddEquipmentStats(Item * add);
	void RemoveEquipmentStats(Item * add);
	void SetExperience(int newexp);
	void ApplyExperience(int amount);

	//Combat
	void SetComboPoints(int howmany);
	void GenerateComboPoint(Character * target);
	int SpendComboPoints(Character * target);
	void ClearComboPointTarget();
	int GetComboPoints() override { return comboPoints; };
	bool HasComboPointTarget() override { return (comboPointTarget != nullptr); };
	Character * GetComboPointTarget() override { return comboPointTarget; };
	double GetMainhandWeaponSpeed() override;
	double GetOffhandWeaponSpeed() override;
	double GetMainhandDamagePerSecond() override;
	int GetOffhandDamageRandomHit() override;
	double GetOffhandDamagePerSecond() override;
	int GetMainhandDamageRandomHit() override;
	void HandleNPCKillRewards(Character * killed);
	double GetDodge() override;
	double GetCrit() override;
	double GetParry() override;

	//Movement
	inline int GetRecall() override { return recall; };
	void Stand() override;
	void Sit() override;

	//Skills / Spell Affects
	bool HasSkill(Skill * sk) override;
	bool HasSkillByName(std::string name) override;
	void AddSkill(Skill * newskill) override;
	void RemoveSkill(Skill * sk) override;
	void RemoveSkill(std::string name) override;
	Skill * GetSkillShortName(std::string name) override;
	void StartGlobalCooldown();
	inline double GetGlobalCooldown() override { return globalCooldown; };
	inline void SetGlobalCooldown(double time) override { globalCooldown = time; };
	void SaveSpellAffects();
	void LoadSpellAffects();
	void SaveCooldowns();
	void LoadCooldowns();

	//Death Handling
	void MakeCorpse();
	void RemoveCorpse();
	void SetGhost() override;
	void SetCorpse() override;
	void SetAlive() override;
	void UnsetGhost();
	inline bool IsGhost() override { return isGhost; };
	bool IsAlive() override;
	void SetResurrectTime(int seconds);
	bool CanResAtCorpse(int time_since_death);
	bool CanRes(int time_since_death);

	//Group
	bool HasGroup() override;
	inline Group * GetGroup() override { return group; };

	//Communication
	void SendBW(std::string str) override;
	void Send(std::string str) override;
	void Send(char * str) override;
	void SendGMCP(std::string str) override;
	void SendGMCP(char * str) override;
	void QueryClear(bool(*whichFunc)(Player *, std::string));
    void QueryClearAll();
	void AddQuery(std::string prompt, void * data, bool(*queryFunction)(Player *, std::string));
    void AddQuery(std::string prompt, int data, bool(*func)(Player *, std::string));
    void LuaAddQuery(std::string prompt, sol::userdata * data, std::string whichQuery);
	void * GetQueryDataPtr(bool(*whichFunc)(Player *, std::string));
    int GetQueryDataInt(bool(*whichFunc)(Player *, std::string));
    bool HasQuery();
	bool HasQuery(bool(*whichFunc)(Player *, std::string));
	bool(*GetQueryFunc(int queryindex))(Player *, std::string);

	//Inventory and equipment
	int AddLootRoll(int corpse_id, NPC * corpse);
	bool HasLootRoll(Character * corpse, int corpse_id);
	void RemoveLootRoll(int my_id);	//Remove only one pending roll by roll-ers ID
	void RemoveLootRoll(Character * corpse); //Remove all pending rolls that point to a specific corpse
	void RemoveLootRoll(Character * corpse, int corpse_id); //Remove only one pending roll for specific corpse/ID combo
	void RemoveAllLootRolls();
	bool AddItemInventory(Item * item);
	Item * GetItemInventory(int id);
	Item * GetItemInventory(std::string name);
	Item * RemoveItemInventory(int id);
	Item * RemoveItemInventory(std::string name);
	bool RemoveItemInventory(Item * item);
	int GetEquipLocation(Item * equip);
	bool EquipItemFromInventory(Item * wear);
	bool EquipItem(Item * wear);
	Item * RemoveItemEquipped(int index);
	int GetEquippedItemIndex(std::string name);
	Item * GetItemEquipped(std::string name);
	bool CanWearArmor(int armortype);
	std::stringstream FormatEquipment();

	//Class
	void AddClass(int id, int level);
	int GetClassLevel(int classid);
	void SetClassLevel(int classid, int newlevel);
	void AddClassSkills();

	//Quest
	bool QuestEligible(Quest * quest);
	bool QuestActive(Quest * quest);
	bool QuestCompleted(Quest * quest);
	bool QuestObjectivesComplete(Quest * quest);
	void QuestCompleteObjective(int type, void * obj);
	bool ShouldDropQuestItem(Item * founditem);

	//Get/Set
	void SetMaxHealth(int amount) override;
	void SetMaxMana(int amount) override;
	void SetMaxEnergy(int amount) override;
	void SetMaxRage(int amount) override;
	inline int GetMaxHealth() override { return maxHealth; };
	inline int GetMaxMana() override { return maxMana; };
	inline int GetMaxEnergy() override { return maxEnergy; };
	inline int GetMaxRage() override { return maxRage; };
	inline int GetBaseAgility() override { return agility; };
	inline int GetBaseIntellect() override { return intellect; };
	inline int GetBaseStrength() override { return strength; };
	inline int GetBaseStamina() override { return stamina; };
	inline int GetBaseWisdom() override { return wisdom; };
	inline int GetBaseSpirit() override { return spirit; };
	inline int GetAgility() override { return agility + bonus_agility; };
	inline int GetIntellect() override { return intellect + bonus_intellect; };
	inline int GetStrength() override { return strength + bonus_strength; };
	inline int GetStamina() override { return stamina + bonus_stamina; };
	inline int GetWisdom() override { return wisdom + bonus_wisdom; };
	inline int GetSpirit() override { return spirit + bonus_spirit; };
	inline void SetAgility(int val) { agility <= 0 ? agility = 1 : agility = val; };
	inline void SetIntellect(int val) { intellect <= 0 ? intellect = 1 : intellect = val; };
	inline void SetStrength(int val) { strength <= 0 ? strength = 1 : strength = val; };
	inline void SetStamina(int val) { stamina <= 0 ? stamina = 1 : stamina = val; };
	inline void SetWisdom(int val) { wisdom <= 0 ? wisdom = 1 : wisdom = val; };
	inline void SetSpirit(int val) { spirit <= 0 ? spirit = 1 : spirit = val; };
	int GetLevel() override;
	void SetLevel(int level);
	inline std::string GetName() override { return name; };
	inline int GetGender() override { return gender; };
	inline void SetGender(int g_) { gender = g_; };
	inline virtual std::string GetTitle() override { return title; };
	inline bool IsImmortal() override { return (immlevel > 0); };
	inline int GetImmLevel() override { return immlevel; };
	int GetArmor() override { return (armor + GetAuraModifier(SpellAffect::AURA_MODIFY_ARMOR, 1)) <= 0 ? 0 : (armor + GetAuraModifier(SpellAffect::AURA_MODIFY_ARMOR, 1)); };

	//Inheritence friendly "commands"
	void Look(std::string argument) override { ::cmd_look(this, argument); };
	void Cast(std::string argument) override { ::cmd_cast(this, argument); };

	//Utility/Misc
	inline bool IsNPC() override { return false; };
	inline bool IsPlayer() override { return true; };
	sol::object AsPlayer() override { return sol::make_object(Server::lua, this); };
	void Notify(SubscriberManager * lm);
	void GeneratePrompt(double currentTime);
	void Save() override;
	static Player * LoadPlayer(std::string name, User * user);

private:
	std::string name;
	int agility;	//crit chance and avoidance
	int intellect;  //spell power
	int strength;	//attack power
	int stamina;	//health
	int wisdom;		//mana
	int spirit;		//mana regen
	int bonus_agility;   //stats given by equipment and spells, not saved to db
	int bonus_intellect; //needed to make sure these bonuses never touch our permanent stats
	int bonus_strength;  //  and corrupt them say by editing an item's stats while wearing one
	int bonus_stamina;
	int bonus_wisdom;
	int bonus_spirit;
	int immlevel;
	int level;
	int gender;
	int maxHealth;
	int maxMana;
	int maxEnergy;
	int maxRage;
	int comboPoints;
	int maxComboPoints;
    int armor;
    bool isGhost;

    struct Query
    {
        //bool hasQuery;
        bool(*queryFunction)(Player *, std::string);
        void * queryDataPtr;
        int queryDataInt;
        std::string queryPrompt;
    };
    std::list<Query> queryList; //Only one query per Query::queryFunction allowed
};

#endif