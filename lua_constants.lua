-- CCharacter.h enum MessageType
MSG_CHAR,
MSG_CHAR_VICT,
MSG_ROOM,
MSG_ROOM_NOTCHAR,
MSG_ROOM_NOTVICT,
MSG_ROOM_NOTCHARVICT,
MSG_GROUP,
MSG_GROUP_NOTCHAR = 0, 1, 2, 3, 4, 5, 6, 7;

-- CCharacter.h enum ResourceType
RESOURCE_HEALTH = 1;
RESOURCE_MANA = 2;
RESOURCE_ENERGY = 3;
RESOURCE_RAGE = 4;
RESOURCE_COMBO = 5;

-- CCharacter.h enum Flags
CHAR_FLAG_FRIENDLY = 0;
CHAR_FLAG_NEUTRAL = 1;
CHAR_FLAG_AGGRESSIVE = 2;
CHAR_FLAG_GUILD = 3;
CHAR_FLAG_VENDOR = 4;
CHAR_FLAG_REPAIR = 5;
CHAR_FLAG_TRAINER = 6;

-- CRoom.h enum Flags
ROOM_FLAG_RECALL = 0;
ROOM_FLAG_NOPVP = 1;

-- CSkill.h enum TargetType
TARGET_SELF = 0;
TARGET_OTHER = 1;
TARGET_HOSTILE = 2;
TARGET_ANY = 3;
TARGET_FRIENDLY = 4;
TARGET_NONE = 5;
TARGET_PASSIVE = 6;

-- CCharacter.h enum Races
RACE_NONE,
RACE_HUMAN,
RACE_ELF,
RACE_DWARF,
RACE_ORC,
RACE_GNOME,
RACE_GOBLIN,
RACE_UNDEAD,
RACE_MINOTAUR,
RACE_TROLL = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9;

-- CCharacter.h enum EditState
ED_NONE = 0;
ED_ROOM = 1;
ED_SKILL = 2;
ED_NPC = 3;
ED_ITEM = 4;
ED_QUEST = 5;
ED_CLASS = 6;
ED_PLAYER = 7;
ED_HELP = 8;
ED_AREA = 9;

-- CSpellAffect.h enum Categorys
AFFECT_NONE = 0
AFFECT_MAGIC = 1
AFFECT_CURSE = 2
AFFECT_POISON = 3
AFFECT_DISEASE = 4
AFFECT_BLEED = 5
AFFECT_FRENZY = 6
AFFECT_LAST = 7

-- CSpellAffect.h enum Auras
AURA_MOVE_SPEED = 1
AURA_RESOURCE_COST = 2
AURA_EATING = 3

-- CExit.h enum Direction
DIR_NORTH = 0;
DIR_NORTHEAST = 1;
DIR_EAST = 2;
DIR_SOUTHEAST = 3;
DIR_SOUTH = 4;
DIR_SOUTHWEST = 5;
DIR_WEST = 6;
DIR_NORTHWEST = 7;
DIR_UP = 8;
DIR_DOWN = 9;
DIR_LAST = 10;

--CItem.h enum EquipLocation
EQUIP_NONE, EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER, EQUIP_TRINKET,
EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_ONEHAND, EQUIP_TWOHAND, EQUIP_LAST = 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18;

	--[[
--enum EquipmentSlot
{ 
    EQUIP_HEAD, EQUIP_NECK, EQUIP_SHOULDER, EQUIP_BACK, EQUIP_CHEST, EQUIP_WRIST, EQUIP_HANDS,
    EQUIP_WAIST, EQUIP_LEGS, EQUIP_FEET, EQUIP_FINGER1, EQUIP_FINGER2, EQUIP_TRINKET1, EQUIP_TRINKET2,
    EQUIP_OFFHAND, EQUIP_MAINHAND, EQUIP_LAST 
};

--enum Quality
{
    QUALITY_POOR, QUALITY_COMMON, QUALITY_UNCOMMON, QUALITY_RARE, QUALITY_EPIC, QUALITY_LEGENDARY, QUALITY_ARTIFACT, QUALITY_LAST
};
--enum Binding
{
	BIND_NONE, BIND_PICKUP, BIND_EQUIP, BIND_LAST
};
--enum Type
{
    TYPE_ARMOR_CLOTH, TYPE_ARMOR_LEATHER, TYPE_ARMOR_MAIL, TYPE_ARMOR_PLATE,TYPE_WEAPON_SWORD, TYPE_WEAPON_DAGGER, TYPE_WEAPON_MACE, 
    TYPE_WEAPON_AXE, TYPE_WEAPON_POLEARM, TYPE_WEAPON_STAFF, TYPE_CONTAINER, TYPE_FOOD, TYPE_CONSUMABLE, TYPE_MISC, TYPE_LAST
};

--CTrigger.h
--enum ParentTypes
{
    PARENT_ROOM, PARENT_NPC, PARENT_ITEM
};

--enum TriggerType
{
    ENTER_CHAR, ENTER_NPC, ENTER_PC, EXIT_CHAR, EXIT_NPC, EXIT_PC, TIMER, TRIG_LAST
};

-- CQuest.h
enum ObjectiveType { OBJECTIVE_ROOM, OBJECTIVE_VISITNPC, OBJECTIVE_KILLNPC, OBJECTIVE_ITEM };
]]