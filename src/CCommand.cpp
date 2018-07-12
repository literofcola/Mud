#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CClient.h"
#include "CItem.h"
#include "CSkill.h"
#include "CClass.h"
#include "CExit.h"
#include "CReset.h"
#include "CArea.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "CCommand.h"
#include "mud.h"
#include "utils.h"

/*
 * Command table
 */
//Immortal commands start at -1 (Where absolute value == immlevel required)
//see Command.h for default values
class Command cmd_table[] =
{
	// Common movement commands.
	//name, function, level, isMovement, whileCorpse, whileGhost, position
	{ "north",		cmd_north,		1, 1, 0, 1, Character::POSITION_STANDING },
	{ "east",		cmd_east,		1, 1, 0, 1, Character::POSITION_STANDING },
	{ "south",		cmd_south,		1, 1, 0, 1, Character::POSITION_STANDING },
	{ "west",		cmd_west,		1, 1, 0, 1, Character::POSITION_STANDING },
	{ "ne",			cmd_northeast,	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "se",			cmd_southeast, 	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "sw",			cmd_southwest, 	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "nw",			cmd_northwest, 	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "up",			cmd_up,			1, 1, 0, 1, Character::POSITION_STANDING },
	{ "down",		cmd_down,		1, 1, 0, 1, Character::POSITION_STANDING },
	{ "northeast",	cmd_northeast,	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "southeast",  cmd_southeast,	1, 1, 0, 1, Character::POSITION_STANDING },
	{ "southwest",  cmd_southwest,  1, 1, 0, 1 },
	{ "northwest",  cmd_northwest,  1, 1, 0, 1 },

	//Common other commands.
	//Placed here so one and two letter abbreviations work.
	{ "cast",		cmd_cast,		1, 0, 0, 0, Character::POSITION_STANDING },
    { "cancel",     cmd_cancel,     1, 0, 0, 0, Character::POSITION_ANY },
    /*{ "auction",    cmd_auction,	POS_RESTING,	1 },
    { "channels",   cmd_channels,	POS_DEAD,       1,		LOG_NORMAL, 1 },*/
    { "goto",       cmd_goto,		-1, 0, 1, 1, Character::POSITION_ANY },
    { "inventory",	cmd_inventory,	1, 0, 1, 1, Character::POSITION_ANY },
    { "attack",     cmd_attack,     1, 0, 0, 0, Character::POSITION_STANDING },
    { "kill",		cmd_attack,		1, 0, 0, 0, Character::POSITION_STANDING },
    { "look",		cmd_look,		1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "gt",		cmd_clantalk,	POS_DEAD,		1,		LOG_NORMAL, 1 },
    { "guild",		cmd_clan,		POS_DEAD,		1,		LOG_NORMAL, 1 },*/
    { "learn",      cmd_learn,	    -1, 0, 0, 0, Character::POSITION_ANY },
	{ "rest",		cmd_sit,		1, 0, 0, 1, Character::POSITION_ANY },
    { "sit",		cmd_sit,		1, 0, 0, 1, Character::POSITION_ANY },
    { "sockets",    cmd_sockets,	-3, 0, 1, 1, Character::POSITION_ANY },
    { "stand",		cmd_stand,		1, 0, 0, 1, Character::POSITION_ANY },
	{ "target",		cmd_target,		1, 0, 0, 1, Character::POSITION_ANY },
    { "tell",		cmd_tell, 1, 0, 1, 1, Character::POSITION_ANY },
    { "track",      cmd_track, 1, 0, 0, 0, Character::POSITION_STANDING },

    /*
     * Informational commands.
     */
    { "affects",	cmd_affects, 1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "ban",		cmd_ban,			POS_DEAD,		L1, LOG_ALWAYS, 1 },
	{ "bank",		cmd_bank,		POS_RESTING,	1,	LOG_NORMAL, 1 },
    { "bug",		cmd_bug,			POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "changes",	cmd_changes,		POS_DEAD,		1,  LOG_NORMAL, 1 },*/
    { "commands",	cmd_commands,	1, 0, 1, 1, Character::POSITION_ANY },
    //{ "compare",	cmd_compare,		POS_RESTING,	1,  LOG_NORMAL, 1 },
    { "cooldowns",	cmd_cooldowns,	1, 0, 1, 1, Character::POSITION_ANY },
    { "equipment",	cmd_equipment,	1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "examine",	cmd_examine,		POS_RESTING,	1,  LOG_NORMAL, 1 },*/
    { "help",		cmd_help,		1, 0, 1, 1, Character::POSITION_ANY },
	{ "levels",     cmd_levels,		1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "motd",		cmd_motd,		POS_DEAD,       1,  LOG_NORMAL, 1 },
    { "news",		cmd_news,		POS_DEAD,		1,  LOG_NORMAL, 1 },*/
    { "quest",		cmd_quest, 1, 0, 0, 0, Character::POSITION_ANY },
    /*{ "read",		cmd_read,		POS_RESTING,	1,  LOG_NORMAL, 1 },
    { "report",		cmd_report,		POS_RESTING,	1,  LOG_NORMAL, 1 },*/
    { "score",		cmd_score,		1, 0, 1, 1, Character::POSITION_ANY },
    { "scan",		cmd_scan,		1, 0, 1, 1, Character::POSITION_ANY },
    { "skills",		cmd_skills,		1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "socials",	cmd_socials,		POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "time",		cmd_time,		POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "typo",		cmd_typo,		POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "weather",	cmd_weather,		POS_RESTING,	1,  LOG_NORMAL, 1 },*/
    { "who",		cmd_who,		1, 0, 1, 1, Character::POSITION_ANY },

    /*
     * Configuration commands.
     */
    /*{ "alia",		cmd_alia,		POS_DEAD,		1,  LOG_NORMAL, 0 },
    { "alias",		cmd_alias,		POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "chat", 		cmd_chat, 		POS_STANDING,	1,  LOG_NORMAL, 1 },
    { "description",cmd_description,	POS_DEAD,		1,  LOG_NORMAL, 1 },*/
    { "set",		cmd_set,		1, 0, 1, 1, Character::POSITION_ANY },
    { "title",		cmd_title,		1, 0, 1, 1, Character::POSITION_ANY },
    /*{ "unalias",	cmd_unalias,		POS_DEAD,		1,  LOG_NORMAL, 1 },*/

    /*
     * Communication commands.
     */
    /*{ "afk",		cmd_afk,			POS_DEAD,		1,  LOG_NORMAL, 1 },
    { "emote",		cmd_emote,		POS_RESTING,	1,  LOG_NORMAL, 1 },*/
    { ".",			cmd_shout,		1, 0, 0, 1, Character::POSITION_ANY },
    /*{ ",",			cmd_emote,		POS_RESTING,	1,  LOG_NORMAL, 0 },
	*/
	{ "group",		cmd_group,		1, 0, 1, 1, Character::POSITION_ANY },
	/*
    { ";",		  	cmd_formtalk,	POS_DEAD,		1,  LOG_NORMAL, 0 },
    { "notify",		cmd_notify,		POS_DEAD,		1, 	LOG_NORMAL, 1 },
    { "note",		cmd_note,		POS_SLEEPING,	1,  LOG_NORMAL, 1 },
    { "pray",		cmd_pray,		POS_DEAD,		1,	LOG_NORMAL, 1 },*/
    { "reply",		cmd_reply,		1, 0, 1, 1, Character::POSITION_ANY },
    { "say",		cmd_say,		1, 0, 0, 0, Character::POSITION_ANY },
    { "'",		    cmd_say,		1, 0, 0, 0, Character::POSITION_ANY },
    { "shout",		cmd_shout,		1, 0, 0, 1, Character::POSITION_ANY },
    { "yell",		cmd_yell,		1, 0, 0, 0, Character::POSITION_ANY },

    /*
     * Object manipulation commands.
     */
	{ "drink",		cmd_drink,		1, 0, 0, 0, Character::POSITION_ANY },
    { "drop",		cmd_drop,		1, 0, 0, 0, Character::POSITION_ANY },
    { "eat",		cmd_eat,		1, 0, 0, 0, Character::POSITION_ANY },
    { "hold",		cmd_wear,		1, 0, 0, 0, Character::POSITION_ANY },
    { "remove",		cmd_remove,		1, 0, 0, 0, Character::POSITION_ANY },
    { "wear",		cmd_wear,		1, 0, 0, 0, Character::POSITION_ANY },
	{ "get",		cmd_take,		1, 0, 0, 0, Character::POSITION_ANY },
	{ "take",		cmd_take,		1, 0, 0, 0, Character::POSITION_ANY },
	{ "gather",		cmd_take,		1, 0, 0, 0, Character::POSITION_ANY },

    /*
     * Miscellaneous commands.
     */
    { "class",      cmd_class,		1, 0, 0, 0, Character::POSITION_ANY },
    /*{ "enter", 		cmd_enter, 		POS_STANDING,	1,  LOG_NORMAL, 1 },
    { "follow",		cmd_follow,		POS_STANDING,	1,  LOG_NORMAL, 1 },
    //{ "freport",    cmd_freport,	POS_RESTING,    1,  LOG_NORMAL, 1 },
    { "hide",		cmd_hide,		POS_STANDING,	1,  LOG_NORMAL, 1 },
    { "play",		cmd_play,		POS_STANDING,	1,  LOG_NORMAL, 1 },*/
    { "quit",		cmd_quit,		1, 0, 0, 1, Character::POSITION_ANY },
    { "recall",		cmd_recall,		1, 0, 0, 0, Character::POSITION_ANY },
    /*{ "/",			cmd_chat,		POS_STANDING,	1,  LOG_NORMAL, 0 },
    { "save",		cmd_save,		POS_DEAD,	 	IM, LOG_NORMAL, 1 },*/
    { "search",		cmd_search,		-2, 1, 1, 1, Character::POSITION_ANY },
    /*
    { "sleep",		cmd_sleep,		1, 0, 0, 1 },
	*/
	{ "train",		cmd_train,		1, 0, 0, 0, Character::POSITION_ANY },
    { "wake",		cmd_wake,		1, 0, 0, 1, Character::POSITION_ANY },
    /*{ "where",		cmd_where,		POS_RESTING,	1,  LOG_NORMAL, 1 },
    { "write",		cmd_write,		POS_RESTING,	1,	LOG_NORMAL, 1 },*/

    /*
     * Immortal commands.
     */
    /*
    { "at",         cmd_at,			POS_DEAD,       L6 },*/
    { "advance",	cmd_advance,	-9, 0, 1, 1, Character::POSITION_ANY },
    /*
    { "disable",    cmd_disable,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "dump",		cmd_dump,		POS_DEAD,	ML,  LOG_ALWAYS, 0 },
    { "trust",		cmd_trust,		POS_DEAD,	ML,  LOG_ALWAYS, 1 },
    { "violate",	cmd_violate,		POS_DEAD,	ML,  LOG_ALWAYS, 1 },

    { "allow",		cmd_allow,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },
    { "ban",		cmd_ban,			POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "deny",		cmd_deny,		POS_DEAD,	ML,  LOG_ALWAYS, 1 },*/
    { "disconnect",	cmd_disconnect,	-3, 0, 1, 1, Character::POSITION_ANY },
    /*{ "flag",		cmd_flag,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "freeze",		cmd_freeze,		POS_DEAD,	L4,  LOG_ALWAYS, 1 },
    { "permban",	cmd_permban,		POS_DEAD,	ML,  LOG_ALWAYS, 1 },
    { "protect",	cmd_protect,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },*/
    { "shutdown",	cmd_shutdown,	-10, 0, 1, 1, Character::POSITION_ANY },
    /*{ "wizlock",	cmd_wizlock,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },

    { "force",		cmd_force,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },*/
    { "load",		cmd_load,		-6, 0, 1, 1, Character::POSITION_ANY },
    /*{ "newlock",	cmd_newlock,		POS_DEAD,	L2,  LOG_ALWAYS, 1 },
    { "nochannels",	cmd_nochannels,	POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "noemote",	cmd_noemote,		POS_DEAD,	L3,  LOG_ALWAYS, 1 },
    { "noshout",	cmd_noshout,		POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "notell",		cmd_notell,		POS_DEAD,	L3,  LOG_ALWAYS, 1 },
    { "nopray",		cmd_nopray,		POS_DEAD,	L6,	 LOG_ALWAYS, 1 },
    { "pecho",		cmd_pecho,		POS_DEAD,	L4,  LOG_ALWAYS, 1 }, */
    { "purge",		cmd_purge,		-2,	0, 1, 1, Character::POSITION_ANY },
    { "restore",	cmd_restore,	-2, 0, 1, 1, Character::POSITION_ANY },
    /*{ "sla",		cmd_sla,			POS_DEAD,	L3,  LOG_NORMAL, 0 },
    { "slay",		cmd_slay,		POS_DEAD,	L4,  LOG_ALWAYS, 1 },*/
    { "transfer",	cmd_transfer,	-2, 0, 1, 1, Character::POSITION_ANY },

    /*{ "poofin",		cmd_bamfin,		POS_DEAD,	L8,  LOG_NORMAL, 1 },
    { "poofout",	cmd_bamfout,		POS_DEAD,	L8,  LOG_NORMAL, 1 },
    { "gecho",		cmd_echo,		POS_DEAD,	L4,  LOG_ALWAYS, 1 },
    { "incognito",	cmd_incognito,	POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "log",		cmd_log,			POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "memory",		cmd_memory,		POS_DEAD,	IM,  LOG_NORMAL, 1 },*/
    { "peace",		cmd_peace,		-1,	0, 1, 1, Character::POSITION_ANY },
    /*{ "penalty",	cmd_penalty,		POS_DEAD,	L7,  LOG_NORMAL, 1 },
    { "echo",		cmd_recho,		POS_DEAD,	L6,  LOG_ALWAYS, 1 },
    { "return",     cmd_return,		POS_DEAD,   L1,  LOG_NORMAL, 1 },
    { "snoop",		cmd_snoop,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "string",		cmd_string,		POS_DEAD,	L5,  LOG_ALWAYS, 1 },
    { "switch",		cmd_switch,		POS_DEAD,	L1,  LOG_ALWAYS, 1 },
    { "wizinvis",	cmd_invis,		POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "zecho",		cmd_zecho,		POS_DEAD,	L5,  LOG_ALWAYS, 1 },

    { "clone",		cmd_clone,		POS_DEAD,	L6,  LOG_ALWAYS, 1 },

    { "wiznet",		cmd_wiznet,		POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "immtalk",	cmd_immtalk,		POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "imotd",      cmd_imotd,		POS_DEAD,   IM,  LOG_NORMAL, 1 },
    { ":",			cmd_immtalk,		POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "smote",		cmd_smote,		POS_DEAD,	IM,  LOG_NORMAL, 1 },
    { "prefi",		cmd_prefi,		POS_DEAD,	IM,  LOG_NORMAL, 0 },
    { "prefix",		cmd_prefix,		POS_DEAD,	IM,  LOG_NORMAL, 1 },*/
    /*
     * OLC
     */
    { "edit",		cmd_edit,		-1, 0, 1, 1, Character::POSITION_ANY },

    { "systeminfo", cmd_systeminfo, -1, 0, 1, 1, Character::POSITION_ANY },
    { "sql",        cmd_sql,        -10, 0, 1, 1, Character::POSITION_ANY },

    /*
     * End of list.
     */
    { "",			0,				0, 0, 0, 0, Character::POSITION_ANY }
};

// "show" needs to be the FIRST entry
const class Command roomEditCmd_table[] =
{
	{ "show",		roomEditCmd_show },
    { "north",		roomEditCmd_north },
    { "east",		roomEditCmd_east },
    { "south",		roomEditCmd_south },
    { "west",		roomEditCmd_west },
    { "northeast",	roomEditCmd_northeast },
    { "southeast",	roomEditCmd_southeast },
    { "southwest",	roomEditCmd_southwest },
    { "northwest",	roomEditCmd_northwest },
    { "up",		    roomEditCmd_up },
    { "down",		roomEditCmd_down },
    { "name",		roomEditCmd_name },
	{ "flag",		roomEditCmd_flag },
    { "description",roomEditCmd_description },
    { "reset",      roomEditCmd_reset },
    { "trigger",    roomEditCmd_trigger },
    { "create",     roomEditCmd_create },
    { "area",       roomEditCmd_area },
	{ "",			0, 0, 0, 1, 1				}
};

const class Command itemEditCmd_table[] =
{
	{ "show",		    itemEditCmd_show, 0, 0, 1, 1 },
    { "name",		    itemEditCmd_name, 0, 0, 1, 1 },
	{ "keywords",		itemEditCmd_keywords, 0, 0, 1, 1 },
    { "item_level",		itemEditCmd_item_level, 0, 0, 1, 1 },
    { "char_level",		itemEditCmd_char_level, 0, 0, 1, 1 },
    { "quality",		itemEditCmd_quality, 0, 0, 1, 1 },
    { "equip_location",	itemEditCmd_equip_location, 0, 0, 1, 1 },
    { "binds",	        itemEditCmd_binds, 0, 0, 1, 1 },
    { "type",	        itemEditCmd_type, 0, 0, 1, 1 },
	{ "skillid",	    itemEditCmd_skillid, 0, 0, 1, 1 },
    { "quest",	        itemEditCmd_quest, 0, 0, 1, 1 },
    { "unique",	        itemEditCmd_unique, 0, 0, 1, 1 },
    { "armor",	        itemEditCmd_armor, 0, 0, 1, 1 },
    { "durability",	    itemEditCmd_durability, 0, 0, 1, 1 },
    { "damage_low",	    itemEditCmd_damageLow, 0, 0, 1, 1 },
    { "damage_high",	itemEditCmd_damageHigh, 0, 0, 1, 1 },
    { "speed",	        itemEditCmd_speed, 0, 0, 1, 1 },
    { "value",	        itemEditCmd_value, 0, 0, 1, 1 },
	{ "",			    0, 0, 0, 1, 1 }
};

const class Command skillEditCmd_table[] =
{
	{ "show",		    skillEditCmd_show, 0, 0, 1, 1	},
    { "name",		    skillEditCmd_name, 0, 0, 1, 1	},
    { "long_name",	    skillEditCmd_long_name, 0, 0, 1, 1	},
    { "function_name",	skillEditCmd_function_name, 0, 0, 1, 1	},
    { "target_type",    skillEditCmd_target_type, 0, 0, 1, 1 },
    { "cast_script",    skillEditCmd_cast_script, 0, 0, 1, 1	},
    { "apply_script",	skillEditCmd_apply_script, 0, 0, 1, 1	},
    { "tick_script",	skillEditCmd_tick_script, 0, 0, 1, 1	},
    { "remove_script",	skillEditCmd_remove_script, 0, 0, 1, 1	},
    { "description",    skillEditCmd_description, 0, 0, 1, 1 },
	{ "cost_description",skillEditCmd_cost_description, 0, 0, 1, 1 },
    { "cast_time",		skillEditCmd_cast_time, 0, 0, 1, 1	},
	{ "interrupt_flags",skillEditCmd_interrupt_flags, 0, 0, 1, 1 },
    { "cooldown",       skillEditCmd_cooldown, 0, 0, 1, 1 },
    { "cost_function",  skillEditCmd_cost_function, 0, 0, 1, 1 },
    { "reload",         skillEditCmd_reload, 0, 0, 1, 1 },
	{ "",			0, 0, 0, 1, 1				}
};

const class Command npcEditCmd_table[] =
{
    { "show",       npcEditCmd_show, 0, 0, 1, 1 },
    { "name",       npcEditCmd_name, 0, 0, 1, 1 },
    { "title",      npcEditCmd_title, 0, 0, 1, 1 },
    { "flag",       npcEditCmd_flag, 0, 0, 1, 1 },
    { "drop",       npcEditCmd_drop, 0, 0, 1, 1 },
    { "level",      npcEditCmd_level, 0, 0, 1, 1 },
    { "gender",     npcEditCmd_gender, 0, 0, 1, 1 },
    { "agility",    npcEditCmd_agility, 0, 0, 1, 1 },
    { "intellect",  npcEditCmd_intellect, 0, 0, 1, 1 },
    { "strength",   npcEditCmd_strength, 0, 0, 1, 1 },
    { "stamina",   npcEditCmd_stamina, 0, 0, 1, 1 },
    { "wisdom",     npcEditCmd_wisdom, 0, 0, 1, 1 },
	{ "spirit",     npcEditCmd_spirit, 0, 0, 1, 1 },
    { "health",     npcEditCmd_health, 0, 0, 1, 1 },
    { "mana",       npcEditCmd_mana, 0, 0, 1, 1 },
    { "attack_speed",npcEditCmd_attackSpeed, 0, 0, 1, 1 },
    { "damage_low",  npcEditCmd_damageLow, 0, 0, 1, 1 },
    { "damage_high", npcEditCmd_damageHigh, 0, 0, 1, 1 },
	{ "speechtext", npcEditCmd_speechText, 0, 0, 1, 1 },
    { "skill",      npcEditCmd_skill, 0, 0, 1, 1 },
    { "trigger",    npcEditCmd_trigger, 0, 0, 1, 1 },
    { "",			0, 0, 0, 1, 1 }
};

const class Command questEditCmd_table[] =
{
    { "show",           questEditCmd_show, 0, 0, 1, 1 },
    { "name",           questEditCmd_name, 0, 0, 1, 1 },
    { "objective",      questEditCmd_objective, 0, 0, 1, 1 },
    { "short_desc",     questEditCmd_shortdesc, 0, 0, 1, 1 },
    { "long_desc",      questEditCmd_longdesc, 0, 0, 1, 1 },
    { "progress_msg",   questEditCmd_progressmsg, 0, 0, 1, 1 },
    { "completion_msg", questEditCmd_completionmsg, 0, 0, 1, 1 },
    { "level",          questEditCmd_level, 0, 0, 1, 1 },
    { "level_requirement", questEditCmd_levelrequirement, 0, 0, 1, 1 },
    { "quest_requirement", questEditCmd_questrequirement, 0, 0, 1, 1 },
    { "quest_restriction", questEditCmd_questrestriction, 0, 0, 1, 1 },
    { "start",          questEditCmd_start, 0, 0, 1, 1 },
    { "end",            questEditCmd_end, 0, 0, 1, 1 },
    { "exp_reward",     questEditCmd_expreward, 0, 0, 1, 1 },
    { "money_reward",   questEditCmd_moneyreward, 0, 0, 1, 1 },
	{ "item_reward",    questEditCmd_itemreward, 0, 0, 1, 1 },
    { "shareable",      questEditCmd_shareable, 0, 0, 1, 1 },
    { "",               0, 0, 0, 1, 1 }
};

const class Command classEditCmd_table[] =
{
    { "show",       classEditCmd_show, 0, 0, 1, 1  },
    { "",			0,  0, 0, 1, 1                 }
};

const class Command helpEditCmd_table[] =
{
    { "show",       helpEditCmd_show, 0, 0, 1, 1  },
	{ "text",		helpEditCmd_text, 0, 0, 1, 1  },
    { "delete",		helpEditCmd_delete, 0, 0, 1, 1  },
    { "",			0,  0, 0, 1, 1                 }
};

const class Command playerEditCmd_table[] =
{
    { "show",       playerEditCmd_show, 0, 0, 1, 1  },
    { "name",       playerEditCmd_name, 0, 0, 1, 1  },
    { "level",      playerEditCmd_level,0, 0, 1, 1  },
    { "experience", playerEditCmd_exp,  0, 0, 1, 1  },
    { "",			0,  0, 0, 1, 1                  }
};

const class Command areaEditCmd_table[] = 
{
    { "show",             areaEditCmd_show, 0, 0, 1, 1  },
    { "name",             areaEditCmd_name, 0, 0, 1, 1  },
    { "pvp",              areaEditCmd_pvp,0, 0, 1, 1  },
	{ "death_room",       areaEditCmd_death_room,0, 0, 1, 1 },
    { "level_range_low",  areaEditCmd_levelRangeLow,  0, 0, 1, 1  },
    { "level_range_high", areaEditCmd_levelRangeHigh,  0, 0, 1, 1  },
    { "", 0,  0, 0, 1, 1 }
};

Command * Command::GetCurrentCmdTable(Character * ch)
{
	if(ch->editState != Character::ED_NONE)
    {
        switch(ch->editState)
        {
            case Character::ED_ROOM:
                return (class Command *)&roomEditCmd_table;
                break;
            case Character::ED_SKILL:
                return (class Command *)&skillEditCmd_table;
                break;
            case Character::ED_NPC:
                return (class Command *)&npcEditCmd_table;
                break;
            case Character::ED_ITEM:
                return (class Command *)&itemEditCmd_table;
                break;
            case Character::ED_QUEST:
                return (class Command *)&questEditCmd_table;
                break;
            case Character::ED_CLASS:
                return (class Command *)&classEditCmd_table;
                break;
			case Character::ED_HELP:
                return (class Command *)&helpEditCmd_table;
                break;
            case Character::ED_AREA:
                return (class Command *)&areaEditCmd_table;
                break;
            case Character::ED_PLAYER:
                return (class Command *)&playerEditCmd_table;
                break;
            default:
                LogFile::Log("error", "Character::GetCurrentCmdTable, bad user->editState");
                return NULL;
        }
	}
	else
	{
		return (class Command *)&cmd_table;
	}
    return NULL;
}

/*
 * The main entry point for executing commands.
 */
bool Command::Interpret(Character * ch, string argument)
{
	string command;
    int cmd;
    bool found = false;

	if(ch->HasQuery())
	{
		if((ch->GetQueryFunc())(ch, argument))
			return true;
		//if the question wasn't answered, interpret normally
	}

    // Strip leading spaces.
	int index = 0;
    while(index < (int)argument.length() && iswspace(argument[index]))
		index++;
	argument = argument.substr(index, argument.length() - index);
    if(argument.empty() && ch->editState == Character::ED_NONE)
		return true;

     //Grab the command word.
     //Special parsing so ' can be a command,
     //also no spaces needed after punctuation.
    if(!argument.empty() && !iswalpha(argument[0]) && !iswdigit(argument[0]))
    {
		command = argument[0];
		argument = argument.substr(1, argument.length()-1);

		index = 0;
		while(index < (int)argument.length() && iswspace(argument[index]))
			index++;
		argument = argument.substr(index, argument.length() - index);
    }
    else
    {
		argument = Utilities::one_argument(argument, command);
    }
    command = Utilities::ToLower(command);

    void (*whichFunction)(Character * ch, string argument) = NULL;
    class Command * whichTable;

	whichTable = GetCurrentCmdTable(ch);
	if(ch->editState != Character::ED_NONE && command.empty())
    {
        found = true;
        whichFunction = whichTable[0].cmd_func; //default is "show"
    }

    // Look for command in command table.
    for(cmd = 0; !found && whichTable[cmd].name.length() > 0; cmd++)
    {
        //Check for matching command name and level requirements
	    if(command[0] == whichTable[cmd].name[0] && !Utilities::str_prefix(command.c_str(), whichTable[cmd].name.c_str())
            && ((whichTable[cmd].level >= 0 && whichTable[cmd].level <= ch->level) || //mortal
            (ch->player->IMMORTAL()          							                      //immortal
            && (whichTable[cmd].level >= 0 || (abs(whichTable[cmd].level) <= ch->player->immlevel)))))
	    {	
		    found = true;
            whichFunction = whichTable[cmd].cmd_func;
		    break;
	    }
    }
        
    //If we don't find it in an edit table, check again in the default cmd_table
    if(!found && whichTable != (class Command *)&cmd_table)
    {
        whichTable = (class Command *)&cmd_table;
        // Look for command in command table.
        for(cmd = 0; !found && whichTable[cmd].name.length() > 0; cmd++)
        {
            //Check for matching command name and level requirements
	        if(command[0] == whichTable[cmd].name[0] && !Utilities::str_prefix(command.c_str(), whichTable[cmd].name.c_str())
                && ((whichTable[cmd].level >= 0 && whichTable[cmd].level <= ch->level) || //mortal
                (ch->player->IMMORTAL()          							                      //immortal
                && (whichTable[cmd].level >= 0 || (abs(whichTable[cmd].level) <= ch->player->immlevel)))))
	        {	
		        found = true;
                whichFunction = whichTable[cmd].cmd_func;
		        break;
	        }
        }
    }

     //Log and snoop.
    /*if ( cmd_table[cmd].log == LOG_NEVER )
		strcpy( logline, "" );

    if ( ( !IS_NPC(ch) && IS_SET(ch->act, PLR_LOG) )
    ||   fLogAll
    ||   cmd_table[cmd].log == LOG_ALWAYS )
    {
	sprintf( log_buf, "Log %s: %s", ch->name, logline );
	wiznet(log_buf,ch,NULL,WIZ_SECURE,0,get_trust(ch));
	log_string( log_buf );
    }

    if ( ch->desc != NULL && ch->desc->snoop_by != NULL )
    {
	write_to_buffer( ch->desc->snoop_by, "% ",    2 );
	write_to_buffer( ch->desc->snoop_by, logline, 0 );
	write_to_buffer( ch->desc->snoop_by, "\n\r",  2 );
    }*/

    /*if ( !found )
    {
	//Look for command in socials table.
	if ( !check_social( ch, command, argument ) )
	{
	    //send_to_char( "Huh?\n\r", ch );
	    not_found(ch); // MG style!
	}
	return;
    }*/

	if(!found)
	{
		ch->Send("Invalid command. Type \"commands\" for a list of valid commands.\n\r");
		return true;
	}

    if((ch->IsCorpse() && !whichTable[cmd].whileCorpse)
     ||(ch->IsGhost() && !whichTable[cmd].whileGhost))
    {
        ch->Send("You can't do that while dead!\n\r");
        return true;
    }

	if (whichTable[cmd].position == Character::Position::POSITION_STANDING 
		&& ch->position < whichTable[cmd].position)
	{
		ch->Stand();
	}

    //For movement commands..
    if(whichTable[cmd].isMovement)
    {		
        //if we can't move yet, add to the movement queue
        if(!ch->CanMove() || !ch->movementQueue.empty())
        {
            ch->movementQueue.push_back(whichFunction);
            return false;
        }
        else
        {
            //Otherwise mark that movement happened and run the command
            ch->lastMoveTime = Game::GetGame()->currentTime;
        }
    }

    //Dispatch the command.
    (*whichFunction) (ch, argument);

    return true;
}
