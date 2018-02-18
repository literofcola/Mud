#ifndef CPLAYER_H
#define CPLAYER_H

class Server;
class User;

#define PLAYER_DEATH_TIME 600
#define PLAYER_DEATH_TIME_RUNBACK 120

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

    //Inventory and equipment
    std::vector<Item *> equipped;   //TODO save id only... no "oldstat" items... if all we save is the id, effectively the same thing?
    std::list<Item *> inventory;	
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
    //std::map<int, Quest *> completedQuests; //TODO Save quest stuff, really only need a std::set of id's
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
    Item * NewItemInventory(Item * itemindex);
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
	void MakeCorpse();
	void MakeGhost();
	void MakeAlive();
	bool IsCorpse();
	bool IsGhost();
	bool IsAlive();
	int TimeSinceDeath();
	void SetResurrectTime(int seconds);
	bool CanResAtCorpse();
	bool CanRes();

    User * user; //Talk about circular dependencies... bad design
private:

	bool isCorpse;
	bool isGhost;	 
	double deathTime; //timestamp for res timer
};

#endif