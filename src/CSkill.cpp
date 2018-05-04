#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
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
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "utils.h"

using namespace std;

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

    string fixname = Utilities::SQLFixQuotes(name);
    string fixfname = Utilities::SQLFixQuotes(function_name);

    string skillsql = "INSERT INTO skills (id, name, cast_script, cast_time, interrupt_flags, function_name, apply_script, ";
    skillsql += "tick_script, remove_script, target_type, description, cost_description, long_name, cooldown, cost_script) values ";
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
    skillsql += ", '" + Utilities::SQLFixQuotes(costFunction) + "')";

    skillsql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), cast_script=VALUES(cast_script), cast_time=VALUES(cast_time), ";
    skillsql += "interrupt_flags=VALUES(interrupt_flags), function_name=VALUES(function_name), apply_script=VALUES(apply_script), tick_script=VALUES(tick_script), ";
	skillsql += "remove_script=VALUES(remove_script), target_type=VALUES(target_type), description=VALUES(description), ";
	skillsql += "cost_description = VALUES(cost_description), long_name = VALUES(long_name), ";
    skillsql += "cooldown=VALUES(cooldown), cost_script=VALUES(cost_script)";

    Server::sqlQueue->Write(skillsql);

    changed = false;
}

bool Skill::CompareSkillToString::operator()(const std::pair<int, Skill*>& elem) const
{
	return !Utilities::str_cmp(val_, elem.second->long_name);
}
