#include "stdafx.h"
#include "CLogFile.h"
#include "utils.h"

using namespace std;

namespace Utilities
{

string itos(const int & i)
{
    thread_local stringstream ss;
    ss.str("");
    ss.clear();
    ss << i;
    return ss.str();
}

string dtos(const double & i, int precision)
{
	thread_local stringstream ss;
	ss.str("");
	ss.clear();
	ss << setprecision(precision) << setiosflags(ios::fixed);
	ss << i;
	return ss.str();
}

string i64tos(const __int64 & i)
{
	thread_local stringstream ss;
	ss.str("");
	ss.clear();
	ss << i;
	return ss.str();
}

int GetNoOfProcessors()
{
	static int nProcessors = 0;

	if (0 == nProcessors)
	{
		SYSTEM_INFO si;

		GetSystemInfo(&si);

		nProcessors = si.dwNumberOfProcessors;
	}

	return nProcessors;
}

char * TimeStamp()
{
	thread_local char time_buffer[40];
	TimeStamp(time_buffer);
	return time_buffer;
}

void TimeStamp(char * time_buffer)
{
    struct tm tm;
    size_t len;
    time_t now;

    now = time ( NULL );
    if(localtime_s (&tm, &now ) != 0)
    {
        LogFile::Log("error", "Utilities::TimeStamp; localtime_s failed");
    }

    len = strftime ( time_buffer, 40, "%x %X", &tm );
}

double GetTime()
{
	thread_local _timeb time;

	_ftime64_s(&time);

	return (double)time.time;
}

/*
 * Pick off one argument from a string and return the rest.
 * Understands quotes.
 */
string one_argument(const string & argument, string & one_arg)
{
	//remove leading spaces
	int index;
	for(index = 0; index < (int)argument.length(); index++)
	{
		if(!iswspace(argument[index]))
			break;
	}
	char end = ' ';
	if(argument[index] == '\'' || argument[index] == '"')
		end = argument[index++];

	int temp = (int)argument.find(end, index);
	if(temp == -1)
	{
		one_arg = argument.substr(index, argument.length() - index);
		return "";
	}

	one_arg = argument.substr(index, temp - index);
	index += temp;
	
	while(index < (int)argument.length() && iswspace(argument[index]))
		index++;
	return argument.substr(index, argument.length() - index);
}

/*
 * Given a string like 14.foo, return 14 and 'foo'
 */
int number_argument(string & arg)
{
	int number;

	for(int i = 0; i < (int)arg.length(); i++)
	{
		if(arg[i] == '.')
		{
			number = atoi(arg.substr(0, i).c_str());
			arg = arg.substr(i+1, arg.length()-i);
			return number;
		}
	}
	return 1;
}

/*
 * Return true if an argument is completely numeric.
 */
bool IsNumber(string arg)
{
    if(arg.empty())
        return false;
 
	int index = 0;
    if(arg[index] == '+' || arg[index] == '-')
        index++;
 
    bool decimal = false;
	for(; index < (int)arg.length(); index++)
    {
        if(!iswdigit(arg[index]))
        {
            if(arg[index] == '.' && decimal == false)
                decimal = true;
            else
                return false;
        }
    }
    return true;
}

bool IsAlpha(string arg)
{
    if(arg.empty())
        return false;

    for(int index = 0; index < (int)arg.length(); index++)
    {
        if(!iswalpha(arg[index]))
            return false;
    }
    return true;
}

/*
 * Compare strings, case INSENSITIVE.
 * Return TRUE if different
 *   (compatibility with historical functions).
 */
bool str_cmp(const string & astr, const string & bstr)
{
	if(astr.length() != bstr.length())
		return true;
    for(int i = 0; i < (int)astr.length() && i < (int)bstr.length(); i++)
    {
		if(LOWER(astr[i]) != LOWER(bstr[i]))
			return true;
    }
    return false;
}

/*
 * Compare strings, case insensitive, for prefix matching.
 * Return TRUE if astr not a prefix of bstr
 *   (compatibility with historical functions).
 */
bool str_prefix(const string & astr, const string & bstr)
{
	if(astr.empty() || bstr.empty())
	{
		LogFile::Log("error", "str_prefix: astr.empty() || bstr.empty()");
		return true;
	}
	for(int i = 0; i < (int)astr.length(); i++)
	{
		if(LOWER(astr[i]) != LOWER(bstr[i]))
			return true;
	}
	return false;
}

/*
 * Substring search, case INSENSITIVE.
 * Return TRUE if astr not found in bstr
 *   (compatibility with historical functions).
 */
bool str_str(const string & astr, const string & bstr)
{
    if(astr.empty() || bstr.empty())
    {
        //LogFile::Log("error", "str_str: astr.empty() || bstr.empty()");
        return true;
    }
    
    for(int i = 0; i < (int)bstr.length(); i++)
    {
        bool found = true;
        for(int j = 0; j < (int)astr.length(); j++)
        {
            if(LOWER(astr[j]) != LOWER(bstr[i+j]))
            {
                found = false;
                break;
            }
        }
        if(found == true)
            return false;
    }
	return true;
}

/*
 * See if a string is one of the names of an object. (ie. is str found in namelist)
 */
bool IsName(string str, string namelist)
{
	if(namelist.empty() || str.empty())
		return false;

	string part;

	namelist = one_argument(namelist, part);
	while(!part.empty())
	{
		if(!str_cmp(str, part.substr(0, str.length())))
			return true;
		namelist = one_argument(namelist, part);
	}

	return false;
}

/*char * RemoveCR(const char * str)
{
	static char strfix[MAX_STRING_LENGTH * 2];
    int i;
    int o;

    if(str == NULL)
        return '\0';

    for(o = i = 0; str[i+o] != '\0'; i++ )
    {
        if (str[i+o] == '\r')
            o++;
        strfix[i] = str[i+o];
    }
    strfix[i] = '\0';
    return strfix;
}

char * AddCR(const char * str)
{
	static char strfix[MAX_STRING_LENGTH * 2];
    int i, j;

    if(str == NULL)
        return '\0';

    for(i = j = 0; str[i] != '\0'; i++, j++)
    {
		if(str[i] == '\n' && str[i+1] != '\r')
		{
			strfix[j] = '\n';
			strfix[++j] = '\r';
		}
		else
			strfix[j] = str[i];
    }
    strfix[j] = '\0';
    return strfix;
}*/

int atoi(const string & str)
{
	return ::atoi(str.c_str());
}

double atof(const string & str)
{
	return ::atof(str.c_str());
}

string ColorString(char type)
{
    switch(type)
    {
	    default:
	        return CLEAR;
	    case 'x':
	        return CLEAR;
	    case 'b':
	        return C_BLUE;
	    case 'c':
	        return C_CYAN;
	    case 'g':
	        return C_GREEN;
	    case 'm':
	        return C_MAGENTA;
	    case 'r':
	        return C_RED;
	    case 'w':
	        return C_WHITE;
	    case 'y':
	        return C_YELLOW;
	    case 'B':
	        return C_B_BLUE;
	    case 'C':
	        return C_B_CYAN;
	    case 'G':
	        return C_B_GREEN;
	    case 'M':
	        return C_B_MAGENTA;
	    case 'R':
	        return C_B_RED;
	    case 'W':
	        return C_B_WHITE;
	    case 'Y':
	        return C_B_YELLOW;
	    case 'D':
	        return C_D_GREY;
	    case 'I':
		    return "\033[7m";
	    case '|':
	        return "|";
    }
}

string SQLFixQuotes(string replace)
{
    string ret = "";
    for(int i = 0; i < (int)replace.length(); i++)
    {
        if(replace[i] == '\'')
            ret += "''";
        else if(replace[i] == '\\')
            ret += "\\\\";
        else
            ret += replace[i];
    }
    return ret;
}

string ToLower(string arg)
{
    for(int i = 0; i < (int)arg.length(); i++)
    {
        arg[i] = LOWER(arg[i]);
    }
    return arg;
}

string ToUpper(string arg)
{	
	for(int i = 0; i < (int)arg.length(); i++)
	{
		arg[i] = UPPER(arg[i]);
	}
	return arg;
}

bool FlagIsSet(std::vector<int> & flags, const int flag)
{
    std::vector<int>::iterator iter;
    for(iter = flags.begin(); iter != flags.end(); ++iter)
    {
        if((*iter) == flag)
            return true;
    }
    return false;
}

bool FlagSet(std::vector<int> & flags, const int flag)
{
    if(!FlagIsSet(flags, flag))
    {
        flags.push_back(flag);
        return true;
    }
    return false;
}

bool FlagUnSet(std::vector<int> & flags, const int flag)
{
    std::vector<int>::iterator iter;
	//todo: there's almost certainly a more efficient way to do this but do we really care
    for(iter = flags.begin(); iter != flags.end(); ++iter) 
    {
        if((*iter) == flag)
        {
            flags.erase(iter);
            return true;
        }
    }
    return false;
}



//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
std::string GetLastErrorAsString()
{
    //Get the error message, if any.
    DWORD errorMessageID = ::WSAGetLastError();
    if(errorMessageID == 0)
        return std::string(); //No error message has been recorded

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

    std::string message(messageBuffer, size);

    //Free the buffer.
    LocalFree(messageBuffer);

    return message;
}

} //namespace Utilities