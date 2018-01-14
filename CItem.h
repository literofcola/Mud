#ifndef CITEM_H
#define CITEM_H

class Server;

class Item
{
public:
    Item();
    Item(std::string name_, int id_);
    Item(const Item&);
    ~Item();
    void SetDefaults();

    enum EquipLocation
    { 
        EQUIP_NONE, EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
        EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER, EQUIP_TRINKET,
        EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_ONEHAND, EQUIP_TWOHAND, EQUIP_LAST
    };
    enum Quality
    {
        QUALITY_POOR, QUALITY_COMMON, QUALITY_UNCOMMON, QUALITY_RARE, QUALITY_EPIC, QUALITY_LEGENDARY, QUALITY_ARTIFACT, QUALITY_LAST
    };
	enum Binding
	{
		BIND_NONE, BIND_PICKUP, BIND_EQUIP, BIND_LAST
	};
    enum Type
    {
        TYPE_ARMOR_CLOTH, TYPE_ARMOR_LEATHER, TYPE_ARMOR_MAIL, TYPE_ARMOR_PLATE,TYPE_WEAPON_SWORD, TYPE_WEAPON_DAGGER, TYPE_WEAPON_MACE, 
        TYPE_WEAPON_AXE, TYPE_WEAPON_POLEARM, TYPE_WEAPON_STAFF, TYPE_CONTAINER, TYPE_FOOD, TYPE_CONSUMABLE, TYPE_MISC, TYPE_LAST
    };

    /*std::vector<int> flags; //a vector of constants to indicate flag is set
    enum Flags
    {
        FLAG_FRIENDLY,FLAG_NEUTRAL,FLAG_AGGRESSIVE,FLAG_QUEST,FLAG_TRAIN,FLAG_GUILD
    };
    struct flag_type
    {
        int flag;
        std::string flag_name;
    };
    static flag_type flag_table[];*/

    int id;
    std::string name;
    int itemLevel;
    int charLevel;
	int binds;
    int equipLocation;
	int type; //for armor: cloth leather mail plate, 
              //for weapon: sword dagger mace axe polearm staff, 
              //for equip_none: container food("eat/drink") consumable("use")
    int quest;     //will drop for all members of a party with appropriate quest only
    int quality;
	int unique;
	int armor;
	int durability;
	int damageLow;
	int damageHigh;
	double speed;
	int value;
    std::list<Item *> contents; //TODO

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;
    std::map<std::string, double*> doubleTable;

    bool changed;

    void Save();

    static const char * equip_strings[];
    static const char * quality_strings[];
    static const char * type_strings[];
    static const char * bind_strings[];

private:

   
};

#endif