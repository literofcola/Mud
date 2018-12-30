#include "stdafx.h"
#include "CItem.h"
#include "CPlayer.h"
#include "CServer.h"
#include "CSkill.h"
#include "CGame.h"
#include "utils.h"

//These tables must match the order of enums in class Item
const char * Item::equip_strings[] = 
{
    "", "Head", "Neck", "Shoulder", "Back", "Chest", "Wrist", "Hands", "Waist", "Legs", "Feet", 
    "Finger", "Trinket", "Off Hand", "Main Hand", "One Hand", "Two Hand", ""
};
/*
EQUIP_NONE, EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER, EQUIP_TRINKET,
EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_ONEHAND, EQUIP_TWOHAND, EQUIP_LAST
*/

const char * Item::quality_strings[] =
{
    "|D", "|W", "|G", "|B", "|M", "|Y", "|C", ""
};
//QUALITY_POOR, QUALITY_COMMON, QUALITY_UNCOMMON, QUALITY_RARE, QUALITY_EPIC, QUALITY_LEGENDARY, QUALITY_ARTIFACT, QUALITY_LAST
    
const char * Item::bind_strings[] =
{
    "", "Binds when picked up", "Binds when equipped", ""
};
//BIND_NONE, BIND_PICKUP, BIND_EQUIP, BIND_LAST

const char * Item::type_strings[] =
{
    "Cloth", "Leather", "Mail", "Plate", "Sword", "Dagger", "Mace", "Axe", "Polearm", "Staff", "Container", "Food", "Consumable", "", "Shield", "Drink", ""
};
/*
ARMOR_CLOTH, ARMOR_LEATHER, ARMOR_MAIL, ARMOR_PLATE, WEAPON_SWORD, WEAPON_DAGGER, WEAPON_MACE, 
WEAPON_AXE, WEAPON_POLEARM, WEAPON_STAFF, CONTAINER, FOOD, CONSUMABLE, TYPE_MISC, TYPE_SHIELD, TYPE_DRINK, TYPE_LAST
*/

Item::flag_type Item::flag_table[] =
{
	{ Item::FLAG_ROOMONLY, "room_only" },
	{ -1, "" }
};

Item::Item(std::string name_, int id_)
{
    id = id_;
    name = name_;

	itemLevel = 1;
	charLevel = 1;
	equipLocation = Item::EQUIP_NONE;
	quality = Item::QUALITY_POOR;
	binds = Item::BIND_NONE;
	type = Item::TYPE_MISC;
	useSkillID = 0;
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
	stringTable["keywords"] = &keywords;
	stringTable["inroom_name"] = &inroom_name;
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
    intTable["skillid"] = &useSkillID;
}

Item::~Item()
{

}

std::string Item::FormatItemInfo(Player * ch)
{
	std::string itemstring;

	itemstring = GetColoredName() + "|X";
    if (ch->IsImmortal())
    {
        itemstring += " |Y[Item ID: " + Utilities::itos(id) + "]|X";
    }
    itemstring += "\r\n";

	if (binds != Item::BIND_NONE)
	{
		itemstring += ((std::string)Item::bind_strings[binds] + "\r\n");
	}
	if (quest)
	{
		itemstring += "Quest Item\r\n";
	}
	if (unique)
	{
		itemstring += "Unique\r\n";
	}

	if (equipLocation != Item::EQUIP_NONE)
	{
		itemstring += Item::equip_strings[equipLocation];
		if (type != Item::TYPE_MISC)
		{
			int armorlevel = 0;
			//TYPE_ARMOR_CLOTH, TYPE_ARMOR_LEATHER, TYPE_ARMOR_MAIL, TYPE_ARMOR_PLATE,
			if (ch && !ch->CanWearArmor(type))
			{
				itemstring += "|R";
			}
			itemstring += "     " + (std::string)Item::type_strings[type] + "|X\r\n";
		}
		else
		{
			itemstring += "\r\n";
		}
	}
	else if (equipLocation == Item::EQUIP_NONE && type != Item::TYPE_MISC)
	{
		itemstring += (std::string)Item::type_strings[type] + "\r\n";
	}

	if (armor > 0)
	{
		itemstring += "Armor " + Utilities::itos(armor) + "\r\n";
	}
	if (damageHigh > 0 && speed > 0)
	{
		itemstring += Utilities::itos(damageLow) + " - " + Utilities::itos(damageHigh) + " Damage";
		itemstring += "    Speed " + Utilities::dtos(speed, 2) + "\r\n";
		double dps = ((damageLow + damageHigh) / 2.0) / speed;
		itemstring += "(" + Utilities::dtos(dps, 2) + " damage per second)\r\n";
	}

	if (agility > 0)
		itemstring += "+" + Utilities::itos(agility) + " Agility\r\n";
	if (intellect > 0)
		itemstring += "+" + Utilities::itos(intellect) + " Intellect\r\n";
	if (strength > 0)
		itemstring += "+" + Utilities::itos(strength) + " Strength\r\n";
	if (stamina > 0)
		itemstring += "+" + Utilities::itos(stamina) + " Stamina\r\n";
	if (wisdom > 0)
		itemstring += "+" + Utilities::itos(wisdom) + " Wisdom\r\n";
	if (spirit > 0)
		itemstring += "+" + Utilities::itos(spirit) + " Spirit\r\n";

	if (durability)
	{
		itemstring += "Durability " + Utilities::itos(durability) + "\r\n";
	}
	if (charLevel > 0)
	{
		if (ch && ch->GetLevel() < charLevel)
			itemstring += "|R";
		itemstring += "Requires Level " + Utilities::itos(charLevel) + "|X\r\n";
	}
	if (itemLevel > 0)
		itemstring += "Item Level " + Utilities::itos(itemLevel) + "\r\n";
	if (value > 0)
		itemstring += "Sell Price: " + Utilities::itos(value) + "\r\n";

    if (useSkillID != 0)
    {
        Skill * on_use = Game::GetGame()->GetSkill(useSkillID);
        if (on_use != nullptr)
        {
            itemstring += "|GUse: " + on_use->description + "|X\r\n";
        }
    }

	return itemstring;
}

void Item::Save()
{
    if(!changed)
        return;

	std::string sql = "INSERT INTO items (items.id, items.name, items.keywords, items.inroom_name, items.item_level, items.char_level, items.equip_location, items.quality,";
	sql += " items.binds, items.type, items.skill_id, items.quest, items.armor, items.durability, items.unique, items.damage_low, items.damage_high, items.value, items.speed,";
	sql += " items.agility, items.intellect, items.strength, items.stamina, items.wisdom, items.spirit) values (";
    sql += Utilities::itos(id) + ", '" + Utilities::SQLFixQuotes(name) + "', '" + Utilities::SQLFixQuotes(keywords) + "', '" + Utilities::SQLFixQuotes(inroom_name) + "', " + Utilities::itos(itemLevel);
    sql += ", " + Utilities::itos(charLevel) + ", " + Utilities::itos(equipLocation);
    sql += ", " + Utilities::itos(quality) + ", " + Utilities::itos(binds) + ", " + Utilities::itos(type) + ", " + Utilities::itos(useSkillID) + ", " + (quest ? "1" : "0");
    sql += ", " + Utilities::itos(armor) + ", " + Utilities::itos(durability) + ", " + (unique ? "1" : "0");
    sql += ", " + Utilities::itos(damageLow) + ", " + Utilities::itos(damageHigh) + ", " + Utilities::itos(value);
	sql += ", " + Utilities::dtos(speed, 1) + ", " + Utilities::itos(agility) + ", " + Utilities::itos(intellect) + ", " + Utilities::itos(strength) + ", " + Utilities::itos(stamina);
	sql += ", " + Utilities::itos(wisdom) + ", " + Utilities::itos(spirit) + ")";

    sql += " ON DUPLICATE KEY UPDATE items.id=VALUES(items.id), items.name=VALUES(items.name), items.keywords=VALUES(items.keywords),items.inroom_name=VALUES(items.inroom_name),items.item_level=VALUES(items.item_level), ";
    sql += "items.char_level=VALUES(items.char_level), items.equip_location=VALUES(items.equip_location), items.quality=VALUES(items.quality), ";
    sql += "items.binds=VALUES(items.binds), items.type=VALUES(items.type), items.skill_id=VALUES(items.skill_id),items.quest=VALUES(items.quest), items.armor=VALUES(items.armor), ";
    sql += "items.durability=VALUES(items.durability), items.unique=VALUES(items.unique), items.damage_low=VALUES(items.damage_low), ";
    sql += "items.damage_high=VALUES(items.damage_high), items.value=VALUES(items.value), items.speed=VALUES(items.speed), items.agility=VALUES(items.agility), ";
	sql += "items.intellect=VALUES(items.intellect), items.strength=VALUES(items.strength), items.stamina=VALUES(items.stamina), items.wisdom=VALUES(items.wisdom), items.spirit=VALUES(items.spirit)";
    
    Server::sqlQueue->Write(sql);

    changed = false;
}