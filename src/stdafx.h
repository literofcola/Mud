#include "json.hpp"

#include <map>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <memory>
#include <list>
#include <random>
#include <deque>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include <sys/timeb.h>
#include <time.h>

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define SOL_CHECK_ARGUMENTS
#define SOL_PRINT_ERRORS
#include <sol.hpp>
