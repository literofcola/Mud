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

extern void Lua_DefineClasses(lua_State * ls);
extern void Lua_DefineFunctions(lua_State * ls);

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
    Server::luaState = lua_open();
    luaL_openlibs(Server::luaState);
	if(Server::luaState == NULL)
	{
        LogFile::Log("error", "main; Could not init Lua state");
        LogFile::CloseAll();
		return 0;
	}
    // Connect LuaBind to this lua state
    luabind::open(Server::luaState);

    Lua_DefineClasses(Server::luaState);
    Lua_DefineFunctions(Server::luaState);
    luaL_dofile(Server::luaState, "some_lua.lua"); //Test scripts

	thegame = new Game();
	theserver = new Server(thegame, 4000);

	if(!theserver->Initialize())
	{
		LogFile::CloseAll();
        lua_close(theserver->luaState);
		delete thegame;
		delete theserver;
		return 0;
	}

	//Connect to mySQL server
    if(!theserver->sqlQueue->Connect("mud", "localhost", "root"))
    {
        LogFile::Log("error", "main; Could not connect to mySQL server 'mud'");
        LogFile::CloseAll();
        lua_close(theserver->luaState);
		delete thegame;
		delete theserver;
        return 0;
    }

	LogFile::Log("status", "Server listening on port 4000");

    theserver->Start();
    thegame->GameLoop(theserver);
    //theserver->Stop();
	theserver->sqlQueue->Disconnect();
	lua_close(Server::luaState);
	theserver->DeInitialize();
    LogFile::CloseAll();

	delete thegame;
	delete theserver;

    return 0;
}
