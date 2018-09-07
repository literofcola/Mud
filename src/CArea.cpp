#include "CArea.h"
#include "CServer.h"
#include "utils.h"
#include <string>

Area::Area(int id_)
{
    id = id_;
    changed = true;
}

Area::Area(int id_, std::string name_, int pvp_, int death_room_, int level_range_low_, int level_range_high_)
{
    id = id_;
    name = name_;
	pvp = pvp_;
	death_room = death_room_;
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

    std::string sql = "INSERT INTO areas (id, name, pvp, death_room, level_range_low, level_range_high) values (";
    sql += Utilities::itos(id) + ",'" + Utilities::SQLFixQuotes(name) + "'," + Utilities::itos(pvp) + "," + Utilities::itos(death_room) + ",";
    sql += Utilities::itos(level_range_low) + "," + Utilities::itos(level_range_high) + ")";
    
    sql += " ON DUPLICATE KEY UPDATE areas.id=VALUES(areas.id), areas.name=VALUES(areas.name), areas.pvp=VALUES(areas.pvp),";
    sql += "areas.death_room=VALUES(areas.death_room),areas.level_range_low=VALUES(areas.level_range_low), areas.level_range_high=VALUES(areas.level_range_high)";

    Server::sqlQueue->Write(sql);

    changed = false;
}