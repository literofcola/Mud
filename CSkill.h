#ifndef CSKILL_H
#define CSKILL_H

class Server;
typedef boost::shared_ptr<Server> Server_ptr;

class Skill
{
public:
    Skill(int id_, std::string long_name_); 
    ~Skill();

    int id;
    std::string name; //keyword name for cmd_cast, unique per character
    std::string long_name; //global unique name
    std::string function_name;
    std::string castScript;
    std::string applyScript;
    std::string tickScript;
    std::string removeScript;
    std::string affectDescription;
    enum TargetType { TARGET_SELF, TARGET_OTHER, TARGET_HOSTILE, TARGET_ANY, TARGET_FRIENDLY, TARGET_NONE };
    TargetType targetType;
    double castTime;
    double cooldown;
    std::string costFunction;
    //bool ignoreGlobal; //TODO, flag to ignore global cooldowns
    bool changed;
    void Save();

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;
    std::map<std::string, double*> doubleTable;

private:
    Skill();
};

#endif //CSKILL_H