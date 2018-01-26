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
    changed = false;
    targetType = Skill::TARGET_SELF;

    intTable["id"] = &id;
    doubleTable["casttime"] = &castTime;
    doubleTable["cooldown"] = &cooldown;
    stringTable["name"] = &name;
    stringTable["long_name"] = &long_name;
    stringTable["function_name"] = &function_name;
    stringTable["affectdescription"] = &affectDescription;
}

Skill::Skill(int id_, std::string long_name_)
{
    id = id_;
    long_name = long_name_;
    cooldown = 0;
    castTime = 0;
    changed = false;
    targetType = Skill::TARGET_SELF;

    intTable["id"] = &id;
    doubleTable["castTime"] = &castTime;
    doubleTable["cooldown"] = &cooldown;
    stringTable["name"] = &name;
    stringTable["long_name"] = &long_name;
    stringTable["function_name"] = &function_name;
    stringTable["affectDescription"] = &affectDescription;
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

    string skillsql = "INSERT INTO skills (id, name, cast_script, cast_time, function_name, apply_script, ";
    skillsql += "tick_script, remove_script, target_type, affect_desc, long_name, cooldown, cost_script) values ";
    skillsql += "(" + Utilities::itos(id) + ", '" + fixname + "', '" + Utilities::SQLFixQuotes(castScript) + "', ";
    skillsql += Utilities::dtos(castTime, 2) + ",'" + fixfname + "', '";
    skillsql += Utilities::SQLFixQuotes(applyScript) + "', '" + Utilities::SQLFixQuotes(tickScript) + "', '";
    skillsql += Utilities::SQLFixQuotes(removeScript) + "'," + Utilities::itos(targetType) + ",'" + Utilities::SQLFixQuotes(affectDescription);
    skillsql += "','" + Utilities::SQLFixQuotes(long_name) + "'," + Utilities::dtos(cooldown, 2);
    skillsql += ", '" + Utilities::SQLFixQuotes(costFunction) + "')";

    skillsql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), cast_script=VALUES(cast_script), cast_time=VALUES(cast_time), ";
    skillsql += "function_name=VALUES(function_name), apply_script=VALUES(apply_script), tick_script=VALUES(tick_script), ";
    skillsql += "remove_script=VALUES(remove_script), target_type=VALUES(target_type), affect_desc=VALUES(affect_desc), long_name=VALUES(long_name), ";
    skillsql += "cooldown=VALUES(cooldown), cost_script=VALUES(cost_script)";

    Server::sqlQueue->Write(skillsql);

    changed = false;
}