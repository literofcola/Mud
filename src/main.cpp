#include "stdafx.h"
//#include "vld.h"


extern void Lua_DefineClasses(sol::state * lua);
extern void Lua_DefineFunctions(sol::state * lua);

Game	* thegame;
Server	* theserver;

int main(int argc, char * argv[])
{
	Server::rand.seed((unsigned)time(NULL));

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

	//todo: turn logfile into a multithreaded queue?
	LogFile::Log("status", "Server listening on port 4000");
	
	//todo: consider reordering here to load game from db prior to server accept

    theserver->Start();
    thegame->GameLoop(theserver);

	theserver->DeInitialize();
    thegame->SaveGameStats();
    if(theserver->sqlQueue != NULL)
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
