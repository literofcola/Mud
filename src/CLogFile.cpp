#include "stdafx.h"
#include "CLogFile.h"
#include "utils.h"

using std::string;

std::map<const char *, std::ofstream*> LogFile::files;
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
		std::ofstream * of = new std::ofstream();
        files[logName] = of;
        string path = "logs\\";
        path += logName;
        path += ".txt";
        files[logName]->open(path.c_str(), std::fstream::out| std::ios_base::ate| std::ios_base::app);
    }
    *files[logName] << Utilities::TimeStamp() << " :: " << logString << std::endl;
	std::cout << Utilities::TimeStamp() << " :: " << logString << std::endl;
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
	std::map<const char *, std::ofstream*>::const_iterator iter;
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
