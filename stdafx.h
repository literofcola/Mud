#include "asio.hpp"
#include "zlib.h"
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/any.hpp>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <iostream>
#include <list>
#include <fstream>
#include <iomanip>
#include <sstream>

#include <mysql++.h>
#include <windows.h>
#include <process.h>
#include <sys/timeb.h>
#include <time.h>
#include <conio.h>

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include "luabind/luabind.hpp"