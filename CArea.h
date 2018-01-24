#ifndef CAREA_H
#define CAREA_H

class Area
{
public:
    Area(int id_);
    Area(int id_, std::string name_, /*int pvp_,*/ int level_range_low_, int level_range_high_);
    ~Area();

    const int GetID() const;
    void Save();

    std::string name;
    int pvp;
    int level_range_low;
    int level_range_high;
    bool changed;

private:
    int id;
};

#endif