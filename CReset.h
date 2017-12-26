#ifndef CRESET_H
#define CRESET_H

class Character;

class Reset : public Listener
{
public:
    Reset();
    ~Reset();

    int id;
    int type; //npc, item
    int npcID;
    Character * npc;
    int interval; //WorldUpdate only checks resets every 30 seconds
    int wanderDistance;
    int leashDistance;

    double lastReset; //timestamp
    bool removeme;

    void Notify(ListenerManager *);
private:
};

#endif