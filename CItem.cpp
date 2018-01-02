#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CItem.h"
#include "CSkill.h"
#include "CClass.h"
#include "CExit.h"
#include "CReset.h"
#include "CArea.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "utils.h"
#include "mud.h"

//These tables must match the order of enums in class Item
char * Item::equip_strings[] = 
{
    "", "Head", "Neck", "Shoulder", "Back", "Chest", "Wrist", "Hands", "Waist", "Legs", "Feet", 
    "Finger", "Trinket", "Off Hand", "Main Hand", "One Hand", "Two Hand", ""
};
/*
EQUIP_NONE, EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER, EQUIP_TRINKET,
EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_ONEHAND, EQUIP_TWOHAND, EQUIP_LAST
*/

char * Item::quality_strings[] = 
{
    "|D", "|W", "|G", "|B", "|M", "|Y", "|C", ""
};
//QUALITY_POOR, QUALITY_COMMON, QUALITY_UNCOMMON, QUALITY_RARE, QUALITY_EPIC, QUALITY_LEGENDARY, QUALITY_ARTIFACT, QUALITY_LAST
    
char * Item::bind_strings[] = 
{
    "", "Binds when picked up", "Binds when equipped", ""
};
//BIND_NONE, BIND_PICKUP, BIND_EQUIP, BIND_LAST

char * Item::type_strings[] =
{
    "Cloth", "Leather", "Mail", "Plate", "Sword", "Dagger", "Mace", "Axe", "Polearm", "Staff", "Container", "Food", "Consumable", "", ""
};
/*
ARMOR_CLOTH, ARMOR_LEATHER, ARMOR_MAIL, ARMOR_PLATE, WEAPON_SWORD, WEAPON_DAGGER, WEAPON_MACE, 
WEAPON_AXE, WEAPON_POLEARM, WEAPON_STAFF, CONTAINER, FOOD, CONSUMABLE, TYPE_MISC, TYPE_LAST
*/


Item::Item()
{
    SetDefaults();
}

Item::Item(std::string name_, int id_)
{
    SetDefaults();
    id = id_;
    name = name_;
}

Item::Item(const Item & copy)
{
    SetDefaults();
    id = copy.id;
    name = copy.name;
    itemLevel = copy.itemLevel;
    charLevel = copy.charLevel;
    quality = copy.quality;
    equipLocation = copy.equipLocation;
    binds = copy.binds;
    type = copy.type;
    armor = copy.armor;
    quest = copy.quest;
    unique = copy.unique;
    durability = copy.durability;
    damageLow = copy.damageLow;
    damageHigh = copy.damageHigh;
    speed = copy.speed;
    value = copy.value;
}

void Item::SetDefaults()
{
    id = 0;
    itemLevel = 0;
    charLevel = 0;
    equipLocation = Item::EQUIP_NONE;
    quality = Item::QUALITY_POOR;
    binds = Item::BIND_NONE;
    type = Item::TYPE_MISC;
    armor = 0;
    quest = false;
    unique = false;
    durability = 0;
    damageLow = 0;
    damageHigh = 0;
    speed = 0.0;
    value = 0;

    changed = false;

    intTable["id"] = &id;
    stringTable["name"] = &name;
    intTable["itemLevel"] = &itemLevel;
    intTable["charLevel"] = &charLevel;
	intTable["binds"] = &binds;
    intTable["equipLocation"] = &equipLocation;
	intTable["type"] = &type;
    intTable["quest"] = &quest;
    intTable["quality"] = &quality;
	intTable["unique"] = &unique;
	intTable["armor"] = &armor;
	intTable["durability"] = &durability;
	intTable["damageLow"] = &damageLow;
	intTable["damageHigh"] = &damageHigh;
	doubleTable["speed"] = &speed;
	intTable["value"] = &value;
}

Item::~Item()
{

}

void Item::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO items (items.id, items.name, items.item_level, items.char_level, items.equip_location, items.quality,";
    sql += " items.binds, items.type, items.quest, items.armor, items.durability, items.unique, items.damage_low, items.damage_high, items.value, items.speed) values (";
    sql += Utilities::itos(id) + ", '" + Utilities::SQLFixQuotes(name) + "', " + Utilities::itos(itemLevel);
    sql += ", " + Utilities::itos(charLevel) + ", " + Utilities::itos(equipLocation);
    sql += ", " + Utilities::itos(quality) + ", " + Utilities::itos(binds) + ", " + Utilities::itos(type) + ", " + (quest ? "1" : "0");
    sql += ", " + Utilities::itos(armor) + ", " + Utilities::itos(durability) + ", " + (unique ? "1" : "0");
    sql += ", " + Utilities::itos(damageLow) + ", " + Utilities::itos(damageHigh) + ", " + Utilities::itos(value);
    sql += ", " + Utilities::dtos(speed, 1) + ")";

    sql += " ON DUPLICATE KEY UPDATE items.id=VALUES(items.id), items.name=VALUES(items.name), items.item_level=VALUES(items.item_level), ";
    sql += "items.char_level=VALUES(items.char_level), items.equip_location=VALUES(items.equip_location), items.quality=VALUES(items.quality), ";
    sql += "items.binds=VALUES(items.binds), items.type=VALUES(items.type), items.quest=VALUES(items.quest), items.armor=VALUES(items.armor), ";
    sql += "items.durability=VALUES(items.durability), items.unique=VALUES(items.unique), items.damage_low=VALUES(items.damage_low), ";
    sql += "items.damage_high=VALUES(items.damage_high), items.value=VALUES(items.value), items.speed=VALUES(items.speed)";
    
    Server::sqlQueue->Write(sql);

    changed = false;
}