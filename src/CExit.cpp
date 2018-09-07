#include "stdafx.h"
#include "CExit.h"
#include <string>

std::string Exit::exitNames[Exit::DIR_LAST] = {
    "north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest", "up", "down" };
std::string Exit::exitNamesShort[Exit::DIR_LAST] = {
	"n", "ne", "e", "se", "s", "sw", "w", "nw", "u", "d" };
std::string Exit::reverseExitNames[Exit::DIR_LAST] = {
    "south", "southwest", "west", "northwest", "north", "northeast", "east", "southeast", "below", "above" };
int Exit::exitOpposite[Exit::DIR_LAST] = { 4, 5, 6, 7, 0, 1, 2, 3, 9, 8 };

Exit::Exit()
{
    from = to = nullptr;
    direction = DIR_LAST;
    removeme = false;
}

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