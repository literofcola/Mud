#include "stdafx.h"
#include "CTrigger.h"
#include "utils.h"
#include <string>
#include <sys/timeb.h>
#include <time.h>

Trigger::Trigger()
{
    id = 0;
    triggerType = -1;
    removeme = false;
    StartTimer();
}

void Trigger::SetType(int newtype)
{
    triggerType = newtype;
}

void Trigger::SetArgument(std::string newargument)
{
    triggerArgument = newargument;
}

void Trigger::SetScript(std::string newscript)
{
    script = newscript;
}

void Trigger::SetFunction(std::string function)
{
    function_name = function;
}

void Trigger::StartTimer()
{
    timer_timestamp = GetTimeStamp();
}

int Trigger::GetType()
{
    return triggerType;
}

std::string & Trigger::GetArgument()
{
    return triggerArgument;
}

std::string & Trigger::GetScript()
{
    return script;
}

std::string & Trigger::GetFunction()
{
    return function_name;
}

bool Trigger::IsTimerExpired()
{
    if(timer_timestamp + Utilities::atoi(triggerArgument) <= GetTimeStamp())
        return true;
    return false;
}

double Trigger::GetTimeStamp()
{
    static _timeb time;
    static time_t time_secs;
    static unsigned short time_millis;

    _ftime64_s(&time);
	time_secs = (time_t) time.time;
    time_millis = time.millitm;

    return ((int)time_secs + ((double)time_millis / 1000.0));
}