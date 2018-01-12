#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
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
#include "utils.h"
#include "mud.h"

Class::Class()
{
    id = 0;
    name = "";
    color = "";
    items = "";
    agilityIncrease = 0;
    intelligenceIncrease = 0;
    strengthIncrease = 0;
    vitalityIncrease = 0;
    wisdomIncrease = 0;

    changed = false;
}

Class::~Class()
{
    classSkills.clear();
}

void Class::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO classes (id, name, color, agility_increase, int_increase, strength_increase, vitality_increase,";
    sql += " wisdom_increase,skills) values (";
    sql += Utilities::itos(id) + ",'" + Utilities::SQLFixQuotes(name) + "','" + Utilities::SQLFixQuotes(color);
    sql += "'," + Utilities::itos(agilityIncrease) + "," + Utilities::itos(intelligenceIncrease);
    sql += "," + Utilities::itos(strengthIncrease) + "," + Utilities::itos(vitalityIncrease) + ",";
    sql += Utilities::itos(wisdomIncrease) + ",'";

    std::list<SkillData>::iterator iter;
    for(iter = classSkills.begin(); iter != classSkills.end(); ++iter)
    {
        sql += Utilities::itos((*iter).skill->id) + "," + Utilities::itos((*iter).level) + ",";
        sql += Utilities::itos((*iter).learnCost) + ";";
    }
    sql += "') ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), color=VALUES(color), agility_increase=VALUES(agility_increase), ";
    sql += "int_increase=VALUES(int_increase), strength_increase=VALUES(strength_increase), vitality_increase=VALUES(vitality_increase)";

    Server::sqlQueue->Write(sql);

    changed = false;
}