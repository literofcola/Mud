#ifndef CTRIGGER_H
#define CTRIGGER_H

#include <string>

class Trigger
{
public:
    Trigger();

    int id;
    bool removeme;

    void SetType(int newtype);
    void SetArgument(std::string newargument);
    void SetScript(std::string newscript);
    void SetFunction(std::string function);
    void StartTimer();
    
    int GetType();
    std::string & GetArgument();
    std::string & GetScript();
    std::string & GetFunction();
    bool IsTimerExpired();

    enum ParentTypes
    {
        PARENT_ROOM, PARENT_NPC, PARENT_ITEM
    };

    enum TriggerType
    {
        ENTER_CHAR, ENTER_NPC, ENTER_PC, EXIT_CHAR, EXIT_NPC, EXIT_PC, TIMER, TRIG_LAST
    };

private:
    int triggerType;
    std::string triggerArgument;
    std::string script;
    std::string function_name;
    double timer_timestamp;

    double GetTimeStamp();
};

#endif