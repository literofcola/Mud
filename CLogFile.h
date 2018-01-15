#ifndef CLOGFILE_H
#define CLOGFILE_H

class LogFile
{
public:
    static inline void Log(const char * logName, const std::string & logString) {Log(logName, logString.c_str());}
	static void Log(const char * logName, const char * logString);
    static void Close(const char * logName);
    static void CloseAll();

private:
    LogFile();
    LogFile(const LogFile & l);
    ~LogFile();
    static CRITICAL_SECTION logFileCS;
    static bool init;


    static std::map<const char *, std::ofstream*> files;
};

#endif //CLOGFILE_H