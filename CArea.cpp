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
#include "mud.h"
#include "utils.h"

Area::Area(int id_)
{
    id = id_;
    changed = true;
}

Area::Area(int id_, std::string name_, int pvp_, int level_range_low_, int level_range_high_)
{
    id = id_;
    name = name_;
    pvp = pvp_;
    level_range_low = level_range_low_;
    level_range_high = level_range_high_;
    changed = true;
}

Area::~Area()
{

}

const int Area::GetID() const
{
    return id;
}

void Area::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO areas (id, name, pvp, level_range_low, level_range_high) values (";
    sql += Utilities::itos(id) + ",'" + Utilities::SQLFixQuotes(name) + "'," + Utilities::itos(pvp);
    sql += "," + Utilities::itos(level_range_low) + "," + Utilities::itos(level_range_high) + ")";
    
    sql += " ON DUPLICATE KEY UPDATE areas.id=VALUES(areas.id), areas.name=VALUES(areas.name), areas.pvp=VALUES(areas.pvp), ";
    sql += "areas.level_range_low=VALUES(areas.level_range_low), areas.level_range_high=VALUES(areas.level_range_high)";

    Server::sqlQueue->Write(sql);

    changed = false;
}