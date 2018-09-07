#ifndef CEXIT_H
#define CEXIT_H

#include <string>

class Room;

class Exit
{
public:

    Exit();
    Exit(Room * from_, Room * to_, int dir);
    ~Exit();


    enum Direction
    {
        DIR_NORTH, DIR_NORTHEAST, DIR_EAST, DIR_SOUTHEAST, DIR_SOUTH, DIR_SOUTHWEST, DIR_WEST, DIR_NORTHWEST, DIR_UP, DIR_DOWN, DIR_LAST
    };
    
    static std::string exitNames[Exit::DIR_LAST];
	static std::string exitNamesShort[Exit::DIR_LAST];
    static std::string reverseExitNames[Exit::DIR_LAST];
    static int exitOpposite[Exit::DIR_LAST];

    Room * from;
    Room * to;
    Direction direction;
    bool removeme; //set to delete this exit on next save

private:
};

#endif