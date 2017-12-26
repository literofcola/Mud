#include "stdafx.h"
#include "utils.h"
#include "CLogFile.h"

using namespace std;

map<const char *, ofstream*> LogFile::files;
CRITICAL_SECTION LogFile::critical_section;
bool LogFile::init = false;

void LogFile::Log(const char * logName, const char * logString)
{
    if(!init)
    {
        InitializeCriticalSection(&critical_section);
        init = true;
    }

    EnterCriticalSection(&critical_section);
    if(files.find(logName) == files.end()) //Check for logName in the open files
    {
        //Not found, open it
        ofstream * of = new ofstream();
        files[logName] = of;
        string path = "logs\\";
        path += logName;
        path += ".txt";
        files[logName]->open(path.c_str(), ios_base::ate|ios_base::app);
    }
    *files[logName] << Utilities::TimeStamp() << " :: " << logString << endl;
    cout << Utilities::TimeStamp() << " :: " << logString << endl;
    LeaveCriticalSection(&critical_section);
}

void LogFile::Close(const char * logName)
{
    EnterCriticalSection(&critical_section);
    if(files.find(logName) != files.end())
    {
        files[logName]->close();
        delete files[logName];
        files.erase(logName);
    }
    LeaveCriticalSection(&critical_section);
}

void LogFile::CloseAll()
{
    EnterCriticalSection(&critical_section);
    map<const char *, ofstream*>::const_iterator iter;
    for (iter=files.begin(); iter != files.end(); ++iter) 
    {
        iter->second->close();
        delete iter->second;
    }
    files.clear();
    LeaveCriticalSection(&critical_section);
    if(init)
    {
        DeleteCriticalSection(&critical_section);
        init = false;
    }
}

LogFile::LogFile()
{
}
LogFile::LogFile(const LogFile & l)
{
}
LogFile::~LogFile()
{
}
