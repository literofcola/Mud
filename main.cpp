#include "stdafx.h"
//#include "vld.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CItem.h"
#include "CSkill.h"
#include "CClass.h"
#include "CExit.h"
#include "CReset.h"
#include "CArea.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CUser.h"
#include "CGame.h"
#include "CServer.h"
#include "utils.h"

extern void Lua_DefineClasses(sol::state * lua);
extern void Lua_DefineFunctions(sol::state * lua);

Game	* thegame;
Server	* theserver;

int main(int argc, char * argv[])
{
	srand((unsigned)time(NULL));

    //Init timer
    if(!HighResTimer::Init())
    {
        LogFile::Log("error", "main; Could not init HighResTimer");
        LogFile::CloseAll();
        return 0;
    }

	//Init Lua
	// open some common libraries
	Server::lua.open_libraries(sol::lib::base, sol::lib::package);
	Server::lua.script_file("some_lua.lua");
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

    //theserver->Stop();
	
	theserver->DeInitialize();
    thegame->SaveGameStats();
    if(theserver->sqlQueue != NULL)
    {
        theserver->sqlQueue->Close();
		theserver->sqlQueue->Disconnect();
        delete theserver->sqlQueue;
    }
	
    //lua_close(Server::luaState);
    LogFile::CloseAll();

	delete thegame;
	delete theserver;

    return 0;
}
