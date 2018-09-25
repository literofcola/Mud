//#include "CUser.h"
#include "CExit.h"
#include "CCharacter.h"
#include <string>

class User;
class Player;
extern class Game * thegame;

//std::string.cpp
namespace StringEdit
{
void string_add(User * user, std::string argument);
void string_append(User * user, std::string * pString);
void string_edit(User * user, std::string * pString);
}

//cmd.cpp
void cmd_attack(Player * ch, std::string argument);
void cmd_cancel(Player * ch, std::string argument);
void cmd_look(Player * ch, std::string argument);
void cmd_target(Player * ch, std::string argument);
void cmd_commands(Player * ch, std::string argument);
void cmd_score(Player * ch, std::string argument);
void cmd_scan(Player * ch, std::string argument);
void cmd_who(Player * ch, std::string argument);
void cmd_title(Player * ch, std::string argument);
void cmd_group(Player * ch, std::string argument);
void cmd_class(Player * ch, std::string argument);
void cmd_levels(Player * ch, std::string argument);
void cmd_help(Player * ch, std::string argument);
void cmd_quest(Player * ch, std::string argument);
void cmd_quit(Player * ch, std::string argument);
bool cmd_quit_Query(Player * ch, std::string argument);
bool releaseSpiritQuery(Player * ch, std::string argument);
bool acceptResQuery(Player * ch, std::string argument);
bool returnToGYQuery(Player * ch, std::string argument);
bool questCompleteQuery(Player * ch, std::string argument);
bool cmd_groupQuery(Player *ch, std::string argument);
void cmd_alias(Player * ch, std::string argument);
void cmd_unalias(Player * ch, std::string argument);

//cmd_comm.cpp
void cmd_shout(Player * ch, std::string argument);
void cmd_say(Player * ch, std::string argument);
void cmd_tell(Player * ch, std::string argument);
void cmd_reply(Player * ch, std::string argument);
void cmd_yell(Player * ch, std::string argument);
void cmd_set(Player * ch, std::string argument);

//cmd_imm.cpp
void cmd_advance(Player * ch, std::string argument);
void cmd_threat(Player * ch, std::string argument);
void cmd_goto(Player * ch, std::string argument);
void cmd_restore(Player * ch, std::string argument);
void cmd_sockets(Player * ch, std::string argument);
void cmd_disconnect(Player * ch, std::string argument);
void cmd_shutdown(Player * ch, std::string argument);
void cmd_load(Player * ch, std::string argument);
void cmd_transfer(Player * ch, std::string argument);
void cmd_sql(Player * ch, std::string argument);
void cmd_systeminfo(Player * ch, std::string argument);
void cmd_purge(Player * ch, std::string argument);
void cmd_search(Player * ch, std::string argument);
void cmd_peace(Player * ch, std::string argument);

//cmd_item.cpp
void cmd_inventory(Player * ch, std::string argument);
void cmd_equipment(Player * ch, std::string argument);
void cmd_remove(Player * ch, std::string argument);
void cmd_wear(Player * ch, std::string argument);
void cmd_drop(Player * ch, std::string argument);
bool cmd_drop_Query(Player * ch, std::string argument);
void cmd_eat(Player * ch, std::string argument);
void cmd_drink(Player * ch, std::string argument);
void cmd_take(Player * ch, std::string argument);
void cmd_takeCallback(Character::DelayData delayData);
void cmd_loot(Player * ch, std::string argument);

//cmd_move.cpp
void cmd_north(Player * ch, std::string argument);
void cmd_east(Player * ch, std::string argument);
void cmd_south(Player * ch, std::string argument);
void cmd_west(Player * ch, std::string argument);
void cmd_northeast(Player * ch, std::string argument);
void cmd_southeast(Player * ch, std::string argument);
void cmd_southwest(Player * ch, std::string argument);
void cmd_northwest(Player * ch, std::string argument);
void cmd_up(Player * ch, std::string argument);
void cmd_down(Player * ch, std::string argument);
void cmd_sit(Player * ch, std::string argument);
void cmd_stand(Player * ch, std::string argument);
void cmd_sleep(Player * ch, std::string argument);
void cmd_wake(Player * ch, std::string argument);
void cmd_recall(Player * ch, std::string argument);
bool cmd_recall_Query(Player * ch, std::string argument);

//cmd_skill.cpp
void cmd_castCallback(Character::DelayData delayData);
void cmd_cast(Player * ch, std::string argument);
void cmd_skills(Player * ch, std::string argument);
void cmd_cooldowns(Player * ch, std::string argument);
void cmd_affects(Player * ch, std::string argument);
//void cmd_learn(Player * ch, std::string argument);
//bool cmd_learn_Query(Player * ch, std::string argument);
void cmd_train(Player * ch, std::string argument);

//cmd_track.cpp
void cmd_track(Player * ch, std::string argument);
Exit::Direction FindDirection(Character * ch, Character * target, int depth);
Exit::Direction FindDirection(Room * start, Room * end, int depth);
int FindDistance(Room * start, Room * end, int maxSearchDist);

//cmd_edit.cpp
void cmd_edit(Player * ch, std::string argument);
void roomEditCmd_show(Player * ch, std::string argument);
void roomEditCmd_name(Player * ch, std::string argument);
void roomEditCmd_area(Player * ch, std::string argument);
void roomEditCmd_flag(Player * ch, std::string argument);
void roomEditCmd_description(Player * ch, std::string argument);
void roomEditCmd_reset(Player * ch, std::string argument);
void roomEditCmd_trigger(Player * ch, std::string argument);
void roomEditCmd_create(Player * ch, std::string argument);
void roomEditCmd_north(Player * ch, std::string argument);
void roomEditCmd_east(Player * ch, std::string argument);
void roomEditCmd_south(Player * ch, std::string argument);
void roomEditCmd_west(Player * ch, std::string argument);
void roomEditCmd_northeast(Player * ch, std::string argument);
void roomEditCmd_southeast(Player * ch, std::string argument);
void roomEditCmd_southwest(Player * ch, std::string argument);
void roomEditCmd_northwest(Player * ch, std::string argument);
void roomEditCmd_up(Player * ch, std::string argument);
void roomEditCmd_down(Player * ch, std::string argument);
void skillEditCmd_show(Player * ch, std::string argument);
void skillEditCmd_name(Player * ch, std::string argument);
void skillEditCmd_long_name(Player * ch, std::string argument);
void skillEditCmd_function_name(Player * ch, std::string argument);
void skillEditCmd_target_type(Player * ch, std::string argument);
void skillEditCmd_cast_script(Player * ch, std::string argument);
void skillEditCmd_apply_script(Player * ch, std::string argument);
void skillEditCmd_tick_script(Player * ch, std::string argument);
void skillEditCmd_remove_script(Player * ch, std::string argument);
void skillEditCmd_cast_time(Player * ch, std::string argument);
void skillEditCmd_interrupt_flags(Player * ch, std::string argument);
void skillEditCmd_cooldown(Player * ch, std::string argument);
void skillEditCmd_flag(Player * ch, std::string argument);
void skillEditCmd_cost_script(Player * ch, std::string argument);
void skillEditCmd_reload(Player * ch, std::string argument);
void skillEditCmd_import(Player * ch, std::string argument);
void skillEditCmd_export(Player * ch, std::string argument);
void skillEditCmd_description(Player * ch, std::string argument);
void skillEditCmd_cost_description(Player * ch, std::string argument);
void npcEditCmd_show(Player * ch, std::string argument);
void npcEditCmd_name(Player * ch, std::string argument);
void npcEditCmd_title(Player * ch, std::string argument);
void npcEditCmd_flag(Player * ch, std::string argument);
void npcEditCmd_drop(Player * ch, std::string argument);
void npcEditCmd_level(Player * ch, std::string argument);
void npcEditCmd_gender(Player * ch, std::string argument);
void npcEditCmd_health(Player * ch, std::string argument);
void npcEditCmd_mana(Player * ch, std::string argument);
void npcEditCmd_armor(Player * ch, std::string argument);
void npcEditCmd_attackSpeed(Player * ch, std::string argument);
void npcEditCmd_damageLow(Player * ch, std::string argument);
void npcEditCmd_damageHigh(Player * ch, std::string argument);
void npcEditCmd_speechText(Player * ch, std::string argument);
void npcEditCmd_trigger(Player * ch, std::string argument);
void itemEditCmd_show(Player * ch, std::string argument);
void itemEditCmd_name(Player * ch, std::string argument);
void itemEditCmd_keywords(Player * ch, std::string argument);
void itemEditCmd_inroom_name(Player * ch, std::string argument);
void itemEditCmd_item_level(Player * ch, std::string argument);
void itemEditCmd_char_level(Player * ch, std::string argument);
void itemEditCmd_quality(Player * ch, std::string argument);
void itemEditCmd_equip_location(Player * ch, std::string argument);
void itemEditCmd_binds(Player * ch, std::string argument);
void itemEditCmd_type(Player * ch, std::string argument);
void itemEditCmd_skillid(Player * ch, std::string argument);
void itemEditCmd_quest(Player * ch, std::string argument);
void itemEditCmd_unique(Player * ch, std::string argument);
void itemEditCmd_armor(Player * ch, std::string argument);
void itemEditCmd_durability(Player * ch, std::string argument);
void itemEditCmd_damageLow(Player * ch, std::string argument);
void itemEditCmd_damageHigh(Player * ch, std::string argument);
void itemEditCmd_speed(Player * ch, std::string argument);
void itemEditCmd_value(Player * ch, std::string argument);
void itemEditCmd_stats(Player * ch, std::string argument);
void questEditCmd_show(Player * ch, std::string argument);
void questEditCmd_objective(Player * ch, std::string argument);
void questEditCmd_name(Player * ch, std::string argument);
void questEditCmd_shortdesc(Player * ch, std::string argument);
void questEditCmd_longdesc(Player * ch, std::string argument);
void questEditCmd_progressmsg(Player * ch, std::string argument);
void questEditCmd_completionmsg(Player * ch, std::string argument);
void questEditCmd_level(Player * ch, std::string argument);
void questEditCmd_levelrequirement(Player * ch, std::string argument);
void questEditCmd_questrequirement(Player * ch, std::string argument);
void questEditCmd_questrestriction(Player * ch, std::string argument);
void questEditCmd_start(Player * ch, std::string argument);
void questEditCmd_end(Player * ch, std::string argument);
void questEditCmd_expreward(Player * ch, std::string argument);
void questEditCmd_moneyreward(Player * ch, std::string argument);
void questEditCmd_itemreward(Player * ch, std::string argument);
void questEditCmd_shareable(Player * ch, std::string argument);
void classEditCmd_show(Player * ch, std::string argument);
void classEditCmd_skill(Player * ch, std::string argument);
void helpEditCmd_show(Player * ch, std::string argument);
void helpEditCmd_text(Player * ch, std::string argument);
void helpEditCmd_name(Player * ch, std::string argument);
void helpEditCmd_delete(Player * ch, std::string argument);
void playerEditCmd_show(Player * ch, std::string argument);
void playerEditCmd_name(Player * ch, std::string argument);
void playerEditCmd_level(Player * ch, std::string argument);
void playerEditCmd_exp(Player * ch, std::string argument);
void areaEditCmd_show(Player * ch, std::string argument);
void areaEditCmd_name(Player * ch, std::string argument);
void areaEditCmd_pvp(Player * ch, std::string argument);
void areaEditCmd_death_room(Player * ch, std::string argument);
void areaEditCmd_levelRangeLow(Player * ch, std::string argument);
void areaEditCmd_levelRangeHigh(Player * ch, std::string argument);