#ifndef CRESET_H
#define CRESET_H

class Character;

class Reset : public Subscriber
{
public:
    Reset();
	Reset(Room * r_, int type_, int targetid_);
    ~Reset();

    int id;
    int type; //npc, item
    int targetID;
    Character * npc;
	//Item * item; //use inroom->items instead
	Room * inroom;
    int interval; //WorldUpdate only checks resets every 30 seconds
    int wanderDistance;
    int leashDistance;
	int count; //multiple targets per reset a possibility for item resets
	static const int RESET_LEASH_DEFAULT = 4;

    double lastReset; //timestamp
    bool removeme;

    void Notify(SubscriberManager *);
private:
};

#endif