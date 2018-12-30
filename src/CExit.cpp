#include "stdafx.h"
#include "CExit.h"

//DIR_NORTH, DIR_NORTHEAST, DIR_EAST, DIR_SOUTHEAST, DIR_SOUTH, DIR_SOUTHWEST, DIR_WEST, DIR_NORTHWEST, DIR_UP, DIR_DOWN, DIR_LAST
std::string Exit::exitNames[Exit::DIR_LAST] = 
{
    "north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest", "up", "down" 
};

std::string Exit::exitNamesShort[Exit::DIR_LAST] = 
{
	"n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d" 
};

std::string Exit::reverseExitNames[Exit::DIR_LAST] =
{
    "south", "southwest", "west", "northwest", "north", "northeast", "east", "southeast", "below", "above" 
};

int Exit::exitOpposite[Exit::DIR_LAST] = 
{ DIR_SOUTH, DIR_SOUTHWEST, DIR_WEST, DIR_NORTHWEST, DIR_NORTH, DIR_NORTHEAST, DIR_EAST, DIR_SOUTHEAST, DIR_DOWN, DIR_UP };

Exit::Exit(Room * from_, Room * to_, int dir)
{
    from = from_;
    to = to_;
    direction = (Exit::Direction)dir;
    removeme = false;
}

Exit::~Exit()
{

}