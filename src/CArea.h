#ifndef CAREA_H
#define CAREA_H

#include <string>

class Area
{
public:
    Area(int id_);
    Area(int id_, std::string name_, int pvp_, int death_room_, int level_range_low_, int level_range_high_);
    ~Area();

    const int GetID() const;
    void Save();

    std::string name;
    int pvp;
	int death_room;
    int level_range_low;
    int level_range_high;
    bool changed;

private:
    int id;
};

#endif