#ifndef CROOM_H
#define CROOM_H

class Character;
class Server;

class Room
{
public:
    Room(int id_);
    Room(int id_, std::string name_, std::string description_);
    ~Room();

    int id;
    std::string name;
    std::string description;
    int area;
    int pvp; //TODO: overrides area.pvp
    std::list<Character *> characters;
    Exit * exits[Exit::DIR_LAST];
    std::map<int, Reset *> resets;
    std::map<int, Trigger> triggers;
	std::list<Item *> items;

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;

    bool changed;

    /*enum TriggerType //Moved to CTrigger.h
    {
        ENTER_CHAR, ENTER_NPC, ENTER_PC, EXIT_CHAR, EXIT_NPC, EXIT_PC, TRIG_LAST
    };*/

	std::vector<int> flags; //a vector of constants to indicate flag is set

	enum Flags
    {
        FLAG_RECALL, FLAG_NOPVP
    };
    struct flag_type
    {
        int flag;
        std::string flag_name;
    };
    static flag_type flag_table[];

    void Save();
    void AddReset(Reset * reset);
    Reset * GetReset(int id);
    void AddTrigger(Trigger & trig);
    Trigger * GetTrigger(int startid, int type = -1);
	void Message(const std::string & text);
	bool HasLivingCharacters();

    bool operator<(const Room& r)const{return id<r.id;}


private:

};

#endif