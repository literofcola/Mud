#include "stdafx.h"
#include "CSkill.h"
#include "CServer.h"
#include "utils.h"

Skill::flag_type Skill::flag_table[] =
{
	{ Skill::FLAG_NOGCD, "nogcd" },
	{ Skill::FLAG_GCDIMMUNE, "gcdimmune" },
	{ -1, "" }
};

Skill::Skill()
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
}

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
}

Skill::~Skill()
{

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
	skillsql += ", '" + Utilities::SQLFixQuotes(costFunction) + "','";

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
