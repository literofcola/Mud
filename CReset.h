#ifndef CRESET_H
#define CRESET_H

class Character;

class Reset : public Subscriber
{
public:
    Reset();
    ~Reset();

    int id;
    int type; //npc, item
    int npcID;
    Character * npc;
	Room * inroom;
    int interval; //WorldUpdate only checks resets every 30 seconds
    int wanderDistance;
    int leashDistance;
	static const int RESET_LEASH_DEFAULT = 4;

    double lastReset; //timestamp
    bool removeme;

    void Notify(SubscriberManager *);
private:
};

#endif