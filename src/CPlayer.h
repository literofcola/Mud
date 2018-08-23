#ifndef CPLAYER_H
#define CPLAYER_H

class Server;
class User;

class Player
{
public:
    Player(User * user_);
    ~Player();

    enum EquipmentSlot
    { 
        EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
        EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER1, EQUIP_FINGER2, EQUIP_TRINKET1, EQUIP_TRINKET2,
        EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_LAST 
    };

	static const int DEFAULT_DEATH_TIME = 600;
	static const int DEFAULT_DEATH_TIME_RUNBACK = 120;
	int death_timer;
	int death_timer_runback;
	int graveyard_room; //graveyard room ID copied from the area death_room. prevent graveyard hopping to other areas
	int corpse_room; //room our corpse is in

    std::string password;
    std::string pwtemp;
    int immlevel;
    int experience;
	int statPoints;
	static const int STATS_PER_LEVEL = 6;
	int recall; //ID of recall room
    double lastCombatAction; //Timestamp for pvp combat flag
    double hoursPlayed;
    
	bool prompt;

	std::map<std::string, std::string> alias;

    //Inventory and equipment
    std::vector<Item *> equipped;
	std::list<std::pair<Item *, int>> inventory;	//structured this way to support item stacks
    int inventorySize;
    int maxInventorySize; //new players start with 16 slots

    //Class
    struct ClassData
    {
        int id;
        int level;
    };
    Class * currentClass;
    std::list<ClassData> classList;

    //Quest
	std::set<int> completedQuests;
    std::vector<Quest *> questLog;
    static const int QUESTLOG_MAX_SIZE = 20;
    std::vector< std::vector<int> > questObjectives; //Maps to questLog[i]->objectives[j]

    int saved;

    inline bool IMMORTAL() { return (immlevel > 0); }
    //static Player * Load(Server * server, std::string name, User * user);
    void SetExperience(int newexp);
    bool QuestEligible(Quest * quest);
    bool QuestActive(Quest * quest);
    bool QuestCompleted(Quest * quest);
    bool QuestObjectivesComplete(Quest * quest);
    void QuestCompleteObjective(int type, void * obj);
	bool ShouldDropQuestItem(Item * founditem);
    void AddItemInventory(Item * item);
    Item * GetItemInventory(int id);
    Item * GetItemInventory(std::string name);
    Item * RemoveItemInventory(int id);
    Item * RemoveItemInventory(std::string name);
    bool RemoveItemInventory(Item * item);
    int GetEquipLocation(Item * equip);
    bool EquipItemFromInventory(Item * wear, int equipSlot);
    Item * RemoveItemEquipped(int index);
    int GetEquippedItemIndex(std::string name);
    Item * GetItemEquipped(std::string name);
    void AddClass(int id, int level);
    int GetClassLevel(int classid);
    void SetClassLevel(int classid, int newlevel);
	void SetGhost();
	void UnsetGhost();
	bool IsGhost();
	void SetResurrectTime(int seconds);
	bool CanResAtCorpse(int time_since_death);
	bool CanRes(int time_since_death);

    User * user; //Talk about circular dependencies... bad design
private:

	bool isGhost;
};

#endif