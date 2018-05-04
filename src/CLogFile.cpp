#include "stdafx.h"
#include "utils.h"
#include "CLogFile.h"

using namespace std;

map<const char *, ofstream*> LogFile::files;
CRITICAL_SECTION LogFile::logFileCS;
bool LogFile::init = false;

void LogFile::Log(const char * logName, const char * logString)
{
    if(!init)
    {
        InitializeCriticalSection(&logFileCS);
        init = true;
    }

    EnterCriticalSection(&logFileCS);
    if(files.find(logName) == files.end()) //Check for logName in the open files
    {
        //Not found, open it
        ofstream * of = new ofstream();
        files[logName] = of;
        string path = "logs\\";
        path += logName;
        path += ".txt";
        files[logName]->open(path.c_str(), fstream::out|ios_base::ate|ios_base::app);
    }
    *files[logName] << Utilities::TimeStamp() << " :: " << logString << endl;
    cout << Utilities::TimeStamp() << " :: " << logString << endl;
    LeaveCriticalSection(&logFileCS);
}

void LogFile::Close(const char * logName)
{
    EnterCriticalSection(&logFileCS);
    if(files.find(logName) != files.end())
    {
        files[logName]->close();
        delete files[logName];
        files.erase(logName);
    }
    LeaveCriticalSection(&logFileCS);
}

void LogFile::CloseAll()
{
    EnterCriticalSection(&logFileCS);
    map<const char *, ofstream*>::const_iterator iter;
    for (iter=files.begin(); iter != files.end(); ++iter) 
    {
        iter->second->close();
        delete iter->second;
    }
    files.clear();
    LeaveCriticalSection(&logFileCS);
    if(init)
    {
        DeleteCriticalSection(&logFileCS);
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
