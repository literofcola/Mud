using namespace std;

extern Game * thegame;

//string.cpp
namespace StringEdit
{
void string_add(User * user, string argument);
void string_append(User * user, string * pString);
void string_edit(User * user, string * pString);
}

//cmd.cpp
void cmd_attack(Character * ch, string argument);
void cmd_cancel(Character * ch, string argument);
void cmd_look(Character * ch, string argument);
void cmd_target(Character * ch, string argument);
void cmd_commands(Character * ch, string argument);
void cmd_score(Character * ch, string argument);
void cmd_who(Character * ch, string argument);
void cmd_title(Character * ch, string argument);
void cmd_group(Character * ch, string argument);
void cmd_class(Character * ch, string argument);
void cmd_levels(Character * ch, string argument);
void cmd_help(Character * ch, string argument);
void cmd_quest(Character * ch, string argument);
void cmd_quit(Character * ch, string argument);
bool cmd_quit_Query(Character * ch, string argument);
bool releaseSpiritQuery(Character * ch, string argument);

//cmd_comm.cpp
void cmd_shout(Character * ch, string argument);
void cmd_say(Character * ch, string argument);
void cmd_tell(Character * ch, string argument);
void cmd_reply(Character * ch, string argument);
void cmd_yell(Character * ch, string argument);

//cmd_imm.cpp
void cmd_advance(Character * ch, string argument);
void cmd_goto(Character * ch, string argument);
void cmd_restore(Character * ch, string argument);
void cmd_sockets(Character * ch, string argument);
void cmd_disconnect(Character * ch, string argument);
void cmd_shutdown(Character * ch, string argument);
void cmd_load(Character * ch, string argument);
void cmd_transfer(Character * ch, string argument);
void cmd_sql(Character * ch, string argument);
void cmd_systeminfo(Character * ch, string argument);
void cmd_purge(Character * ch, string argument);
void cmd_search(Character * ch, string argument);
void cmd_peace(Character * ch, string argument);

//cmd_item.cpp
void cmd_inventory(Character * ch, string argument);
void cmd_equipment(Character * ch, string argument);
void cmd_remove(Character * ch, string argument);
void cmd_wear(Character * ch, string argument);
void cmd_drop(Character * ch, string argument);
bool cmd_drop_Query(Character * ch, string argument);

//cmd_move.cpp
void cmd_north(Character * ch, string argument);
void cmd_east(Character * ch, string argument);
void cmd_south(Character * ch, string argument);
void cmd_west(Character * ch, string argument);
void cmd_northeast(Character * ch, string argument);
void cmd_southeast(Character * ch, string argument);
void cmd_southwest(Character * ch, string argument);
void cmd_northwest(Character * ch, string argument);
void cmd_up(Character * ch, string argument);
void cmd_down(Character * ch, string argument);
void cmd_sit(Character * ch, string argument);
void cmd_stand(Character * ch, string argument);
void cmd_sleep(Character * ch, string argument);
void cmd_wake(Character * ch, string argument);
void cmd_recall(Character * ch, string argument);
bool cmd_recall_Query(Character * ch, string argument);

//cmd_skill.cpp
void cmd_castCallback(Character::DelayData delayData);
void cmd_cast(Character * ch, string argument);
void cmd_do(Character * ch, string argument);
void cmd_skills(Character * ch, string argument);
void cmd_cooldowns(Character * ch, string argument);
void cmd_affects(Character * ch, string argument);
void cmd_learn(Character * ch, string argument);
bool cmd_learn_Query(Character * ch, string argument);
void cmd_train(Character * ch, string argument);

//cmd_track.cpp
void cmd_track(Character * ch, string argument);
Exit::Direction FindDirection(Character * ch, Character * target, int depth);

//cmd_edit.cpp
void cmd_edit(Character * ch, string argument);
void roomEditCmd_show(Character * ch, string argument);
void roomEditCmd_name(Character * ch, string argument);
void roomEditCmd_area(Character * ch, string argument);
void roomEditCmd_flag(Character * ch, string argument);
void roomEditCmd_description(Character * ch, string argument);
void roomEditCmd_reset(Character * ch, string argument);
void roomEditCmd_trigger(Character * ch, string argument);
void roomEditCmd_create(Character * ch, string argument);
void roomEditCmd_north(Character * ch, string argument);
void roomEditCmd_east(Character * ch, string argument);
void roomEditCmd_south(Character * ch, string argument);
void roomEditCmd_west(Character * ch, string argument);
void roomEditCmd_northeast(Character * ch, string argument);
void roomEditCmd_southeast(Character * ch, string argument);
void roomEditCmd_southwest(Character * ch, string argument);
void roomEditCmd_northwest(Character * ch, string argument);
void roomEditCmd_up(Character * ch, string argument);
void roomEditCmd_down(Character * ch, string argument);
void skillEditCmd_show(Character * ch, string argument);
void skillEditCmd_name(Character * ch, string argument);
void skillEditCmd_long_name(Character * ch, string argument);
void skillEditCmd_function_name(Character * ch, string argument);
void skillEditCmd_target_type(Character * ch, string argument);
void skillEditCmd_cast_script(Character * ch, string argument);
void skillEditCmd_apply_script(Character * ch, string argument);
void skillEditCmd_tick_script(Character * ch, string argument);
void skillEditCmd_remove_script(Character * ch, string argument);
void skillEditCmd_cast_time(Character * ch, string argument);
void skillEditCmd_cooldown(Character * ch, string argument);
void skillEditCmd_cost_function(Character * ch, string argument);
void skillEditCmd_reload(Character * ch, string argument);
void skillEditCmd_affect_desc(Character * ch, string argument);
void npcEditCmd_show(Character * ch, string argument);
void npcEditCmd_name(Character * ch, string argument);
void npcEditCmd_title(Character * ch, string argument);
void npcEditCmd_flag(Character * ch, string argument);
void npcEditCmd_drop(Character * ch, string argument);
void npcEditCmd_level(Character * ch, string argument);
void npcEditCmd_gender(Character * ch, string argument);
void npcEditCmd_agility(Character * ch, string argument);
void npcEditCmd_intellect(Character * ch, string argument);
void npcEditCmd_strength(Character * ch, string argument);
void npcEditCmd_stamina(Character * ch, string argument);
void npcEditCmd_wisdom(Character * ch, string argument);
void npcEditCmd_health(Character * ch, string argument);
void npcEditCmd_mana(Character * ch, string argument);
void npcEditCmd_attackSpeed(Character * ch, string argument);
void npcEditCmd_damageLow(Character * ch, string argument);
void npcEditCmd_damageHigh(Character * ch, string argument);
void npcEditCmd_trigger(Character * ch, string argument);
void npcEditCmd_skill(Character * ch, string argument);
void itemEditCmd_show(Character * ch, string argument);
void itemEditCmd_name(Character * ch, string argument);
void itemEditCmd_item_level(Character * ch, string argument);
void itemEditCmd_char_level(Character * ch, string argument);
void itemEditCmd_quality(Character * ch, string argument);
void itemEditCmd_equip_location(Character * ch, string argument);
void itemEditCmd_binds(Character * ch, string argument);
void itemEditCmd_type(Character * ch, string argument);
void itemEditCmd_quest(Character * ch, string argument);
void itemEditCmd_unique(Character * ch, string argument);
void itemEditCmd_armor(Character * ch, string argument);
void itemEditCmd_durability(Character * ch, string argument);
void itemEditCmd_damageLow(Character * ch, string argument);
void itemEditCmd_damageHigh(Character * ch, string argument);
void itemEditCmd_speed(Character * ch, string argument);
void itemEditCmd_value(Character * ch, string argument);
void questEditCmd_show(Character * ch, string argument);
void questEditCmd_objective(Character * ch, string argument);
void questEditCmd_name(Character * ch, string argument);
void questEditCmd_shortdesc(Character * ch, string argument);
void questEditCmd_longdesc(Character * ch, string argument);
void questEditCmd_progressmsg(Character * ch, string argument);
void questEditCmd_completionmsg(Character * ch, string argument);
void questEditCmd_level(Character * ch, string argument);
void questEditCmd_levelrequirement(Character * ch, string argument);
void questEditCmd_questrequirement(Character * ch, string argument);
void questEditCmd_questrestriction(Character * ch, string argument);
void questEditCmd_start(Character * ch, string argument);
void questEditCmd_end(Character * ch, string argument);
void questEditCmd_expreward(Character * ch, string argument);
void questEditCmd_moneyreward(Character * ch, string argument);
void questEditCmd_shareable(Character * ch, string argument);
void classEditCmd_show(Character * ch, string argument);
void helpEditCmd_show(Character * ch, string argument);
void helpEditCmd_text(Character * ch, string argument);
void helpEditCmd_name(Character * ch, string argument);
void helpEditCmd_delete(Character * ch, string argument);
void playerEditCmd_show(Character * ch, string argument);
void playerEditCmd_name(Character * ch, string argument);
void playerEditCmd_level(Character * ch, string argument);
void playerEditCmd_exp(Character * ch, string argument);
void areaEditCmd_show(Character * ch, string argument);
void areaEditCmd_name(Character * ch, string argument);
void areaEditCmd_pvp(Character * ch, string argument);
void areaEditCmd_levelRangeLow(Character * ch, string argument);
void areaEditCmd_levelRangeHigh(Character * ch, string argument);