namespace Utilities
{
    std::string itos(const int & i);
    std::string dtos(const double & i, int precision);
	std::string i64tos(const __int64 & i);
    int atoi(const std::string & str);
    double atof(const std::string & str);
    int GetNoOfProcessors();
    char * TimeStamp();
    void TimeStamp(char * time_buffer);
	double GetTime();
    bool IsAlpha(std::string arg);
    bool IsNumber(std::string arg);
    bool IsName(std::string str, std::string namelist);
    bool str_cmp(const std::string & astr, const std::string & bstr);
    std::string SQLFixQuotes(std::string replace);
    std::string ToLower(std::string arg);
	std::string ToUpper(std::string arg);
    //int ColorString(char type, char *string);
    std::string ColorString(char type);
    int number_argument(std::string & arg);
    bool FlagIsSet(std::vector<int> & flags, const int flag);
    bool FlagSet(std::vector<int> & flags, const int flag);
    bool FlagUnSet(std::vector<int> & flags, const int flag);
	std::string one_argument(const std::string & argument, std::string & one_arg);
	bool str_prefix(const std::string & astr, const std::string & bstr);
    bool str_str(const std::string & astr, const std::string & bstr);
    std::string GetLastErrorAsString();
	std::string SideBySideString(std::string left, std::string right);
	int StringLengthWithoutColor(std::string str);

    /*
	int UMIN(int a, int b);
	int UMAX(int a, int b);
	double UMIN(double a, double b);
	double UMAX(double a, double b);
	int URANGE(int a, int b, int c);
	char LOWER(char c);
	char UPPER(char c);
	int IS_SET(const long & flag, const long & bit);
	void SET_BIT(long & var, const long & bit);
	void UNSET_BIT(long & var, const long & bit);
	bool BETWEEN(int min, int num, int max);*/
        
    /*
     * Utility macros.
     */
    inline int MIN(int a, int b)
    {	return a < b ? a : b;    }

    inline int MAX(int a, int b)
    {	return a > b ? a : b;    }

    inline double MIN(double a, double b)
    {	return a < b ? a : b;    }

    inline double MAX(double a, double b)
    {	return a > b ? a : b;    }

    inline int RANGE(int a, int b, int c)
    {
	    return b < a ? a : b > c ? c : b;
    }

    inline char LOWER(char c)
    {
	    return (c >= 'A' && c <= 'Z') ? (c+'a'-'A') : (c);
    }

    inline char UPPER(char c)
    {
	    return c >= 'a' && c <= 'z' ? c+'A'-'a' : c;
    }

    inline int IS_SET(const long & flag, const long & bit)
    {
	    return (flag & bit);
    }

    inline void SET_BIT(long & var, const long & bit)
    {
	    var |= bit;
    }

    inline void UNSET_BIT(long & var, const long & bit)
    {
	    var &= ~bit;
    }

    inline bool BETWEEN(int min, int num, int max)
    {
	    return min < num && num < max;
    }

    /*
	Color stuff
    */
    //\x1B == \033
    #define CLEAR		"\033[0m"		/* Resets Color	*/
    #define C_RED		"\033[0;31m"	/* Normal Colors	*/
    #define C_GREEN		"\033[0;32m"
    #define C_YELLOW	"\033[0;33m"
    #define C_BLUE		"\033[0;34m"
    #define C_MAGENTA	"\033[0;35m"
    #define C_CYAN		"\033[0;36m"
    #define C_WHITE		"\033[0;37m"
    #define C_D_GREY	"\033[1;30m"  	/* Light Colors		*/
    #define C_B_RED		"\033[1;31m"
    #define C_B_GREEN	"\033[1;32m"
    #define C_B_YELLOW	"\033[1;33m"
    #define C_B_BLUE	"\033[1;34m"
    #define C_B_MAGENTA	"\033[1;35m"
    #define C_B_CYAN	"\033[1;36m"
    #define C_B_WHITE	"\033[1;37m"

}