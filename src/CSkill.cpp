#include "stdafx.h"
#include "CSkill.h"
#include "CServer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CLogFile.h"
#include "utils.h"

Skill::flag_type Skill::flag_table[] =
{
	{ Skill::FLAG_NOGCD, "nogcd" },
	{ Skill::FLAG_GCDIMMUNE, "gcdimmune" },
	{ -1, "" }
};

/*Skill::Skill()
{
    cooldown = 0;
    castTime = 0;
	interruptFlags.set(Skill::Interrupt::INTERRUPT_MOVE);
    changed = false;
    targetType = Skill::TARGET_SELF;

    intTable["id"] = &id;
    doubleTable["casttime"] = &castTime;
    doubleTable["cooldown"] = &cooldown;
    stringTable["name"] = &name;
    stringTable["long_name"] = &long_name;
    stringTable["function_name"] = &function_name;
    stringTable["description"] = &description;
}*/

Skill::Skill(int id_, std::string long_name_)
{
    id = id_;
    long_name = long_name_;
    cooldown = 0;
    castTime = 0;
    changed = false;
    targetType = Skill::TARGET_SELF;
	interruptFlags.set(Skill::Interrupt::INTERRUPT_MOVE);

    intTable["id"] = &id;
    doubleTable["castTime"] = &castTime;
    doubleTable["cooldown"] = &cooldown;
    stringTable["name"] = &name;
    stringTable["long_name"] = &long_name;
    stringTable["function_name"] = &function_name;
    stringTable["description"] = &description;
	stringTable["cost_script"] = &costScript;
	stringTable["cast_script"] = &castScript;
	stringTable["apply_script"] = &applyScript;
	stringTable["tick_script"] = &tickScript;
	stringTable["remove_script"] = &removeScript;
}

Skill::~Skill()
{

}

int Skill::CallLuaCost(Character * caster, Character * target)
{
    std::string cost_func = function_name + "_cost";
    int lua_ret = 0;
    try
    {
        sol::function lua_cost_func = Server::lua[cost_func.c_str()];
        sol::protected_function_result result = lua_cost_func(caster, target, this);
        if (!result.valid())
        {
            // Call failed
            sol::error err = result;
            std::string what = err.what();
            LogFile::Log("error", "_cost call failed, sol::error::what() is: " + what);
        }
        else
        {
            lua_ret = result;
        }
    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
    return lua_ret;
}

void Skill::CallLuaCast(Character * caster, Character * target)
{
    std::string func = function_name + "_cast";
    try
    {
        sol::function lua_cast_func = Server::lua[func.c_str()];
        sol::protected_function_result result = lua_cast_func(caster, target, this);
        if (!result.valid())
        {
            // Call failed
            sol::error err = result;
            std::string what = err.what();
            LogFile::Log("error", "_cast call failed, sol::error::what() is: " + what);
        }

    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
}

void Skill::CallLuaApply(Character * caster, Character * target, SpellAffect * affect)
{
    std::string func = function_name + "_apply";
    try
    {
        sol::function lua_apply_func = Server::lua[func.c_str()];
        sol::protected_function_result result = lua_apply_func(caster, target, affect);
        if (!result.valid())
        {
            // Call failed
            sol::error err = result;
            std::string what = err.what();
            LogFile::Log("error", "_apply call failed, sol::error::what() is: " + what);
        }
    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
}

void Skill::CallLuaTick(Character * caster, Character * target, SpellAffect * affect)
{
    std::string func = function_name + "_tick";
    try
    {
        sol::function lua_tick_func = Server::lua[func.c_str()];
        sol::protected_function_result result = lua_tick_func(caster, target, affect);
        if (!result.valid())
        {
            // Call failed
            sol::error err = result;
            std::string what = err.what();
            LogFile::Log("error", "_tick call failed, sol::error::what() is: " + what);
        }
    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
}

void Skill::CallLuaRemove(Character * caster, Character * target, SpellAffect * affect)
{
    std::string func = function_name + "_remove";
    try
    {
        sol::function lua_remove_func = Server::lua[func.c_str()];
        sol::protected_function_result result = lua_remove_func(caster, target, affect);
        if (!result.valid())
        {
            // Call failed
            sol::error err = result;
            std::string what = err.what();
            LogFile::Log("error", "_remove call failed, sol::error::what() is: " + what);
        }
    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
}

void Skill::Save()
{
    if(!changed)
        return;

	std::string fixname = Utilities::SQLFixQuotes(name);
	std::string fixfname = Utilities::SQLFixQuotes(function_name);

	std::string skillsql = "INSERT INTO skills (id, name, cast_script, cast_time, interrupt_flags, function_name, apply_script, ";
    skillsql += "tick_script, remove_script, target_type, description, cost_description, long_name, cooldown, cost_script, flags) values ";
    skillsql += "(" + Utilities::itos(id) + ", '" + fixname + "', '" + Utilities::SQLFixQuotes(castScript) + "', ";
	skillsql += Utilities::dtos(castTime, 2) + ",'";
	
	for (int i = 0; i < (int)interruptFlags.size(); ++i)
	{
		if(interruptFlags[i])
			skillsql += Utilities::itos(i) + ";";
	}

    skillsql += "','" + fixfname + "', '" + Utilities::SQLFixQuotes(applyScript) + "', '" + Utilities::SQLFixQuotes(tickScript) + "', '";
    skillsql += Utilities::SQLFixQuotes(removeScript) + "'," + Utilities::itos(targetType) + ",'" + Utilities::SQLFixQuotes(description);
    skillsql += "','" + Utilities::SQLFixQuotes(costDescription) + "','" + Utilities::SQLFixQuotes(long_name) + "'," + Utilities::dtos(cooldown, 2);
	skillsql += ", '" + Utilities::SQLFixQuotes(costScript) + "','";

	std::vector<int>::iterator flagiter;
	for (flagiter = flags.begin(); flagiter != flags.end(); ++flagiter)
	{
		skillsql += Utilities::itos((*flagiter)) + ";";
	}
	skillsql += "')";

    skillsql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), cast_script=VALUES(cast_script), cast_time=VALUES(cast_time), ";
    skillsql += "interrupt_flags=VALUES(interrupt_flags), function_name=VALUES(function_name), apply_script=VALUES(apply_script), tick_script=VALUES(tick_script), ";
	skillsql += "remove_script=VALUES(remove_script), target_type=VALUES(target_type), description=VALUES(description), ";
	skillsql += "cost_description = VALUES(cost_description), long_name = VALUES(long_name), ";
    skillsql += "cooldown=VALUES(cooldown), cost_script=VALUES(cost_script), flags=VALUES(flags)";

    Server::sqlQueue->Write(skillsql);

    changed = false;
}

bool Skill::CompareSkillToString::operator()(const std::pair<int, Skill*>& elem) const
{
	return !Utilities::str_cmp(val_, elem.second->long_name);
}
