#include "stdafx.h"
#include "vld.h"

#include "CGame.h"
#include "CServer.h"
#include "CHighResTimer.h"
#include "CLogFile.h"

extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#define SOL_CHECK_ARGUMENTS
#define SOL_PRINT_ERRORS
#include <sol.hpp>

extern void Lua_DefineClasses(sol::state * lua);
extern void Lua_DefineFunctions(sol::state * lua);

Game	* thegame;
Server	* theserver;

int main(int argc, char * argv[])
{
	Server::rand.seed((unsigned)time(nullptr));

    //Init timer
    if(!HighResTimer::Init())
    {
        LogFile::Log("error", "main; Could not init HighResTimer");
        LogFile::CloseAll();
        return 0;
    }

	//Init Lua
	// open some common libraries
	Server::lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::string);
	Server::lua.script_file("lua_constants.lua");
	Lua_DefineClasses(&Server::lua);
	Lua_DefineFunctions(&Server::lua);

	thegame = new Game();
	theserver = new Server(thegame, 4000);

	if(!theserver->Initialize())
	{
		LogFile::CloseAll();
        //lua_close(theserver->luaState);
		delete thegame;
		delete theserver;
		return 0;
	}

	//Connect to mySQL server
    if(!theserver->sqlQueue->Connect("mud", "localhost", "root"))
    {
        LogFile::Log("error", "main; Could not connect to mySQL server 'mud'");
        LogFile::CloseAll();
        //lua_close(theserver->luaState);
		delete thegame;
		delete theserver;
        return 0;
    }

	LogFile::Log("status", "Server listening on port 4000");
	
	//todo: consider reordering here to load game from db prior to server accept

    theserver->Start();
    thegame->GameLoop(theserver);

	theserver->DeInitialize();
    thegame->SaveGameStats();
    if(theserver->sqlQueue != nullptr)
    {
        theserver->sqlQueue->Close();
		theserver->sqlQueue->Disconnect();
        delete theserver->sqlQueue;
    }
	
    LogFile::CloseAll();

	delete thegame;
	delete theserver;

    return 0;
}
