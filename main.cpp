#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
typedef boost::shared_ptr<Client> Client_ptr;
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
typedef boost::shared_ptr<Server> Server_ptr;
#include "utils.h"

using asio::ip::tcp;

extern void Lua_DefineClasses(lua_State * ls);
extern void Lua_DefineFunctions(lua_State * ls);

int main(int argc, char * argv[])
{
    srand((unsigned)time(NULL));

    asio::io_service io_service;
    asio::ip::tcp::endpoint endpoint(asio::ip::tcp::v4(), 4000);

    //TODO how can we pass multiple endpoints to the server's acceptor?
    //Need to have a socket object per port
    //instead of accepting on client->Socket(), call accept on a new Server->socket (one per port)
    //  and then pass those accepted sockets to the client->Socket()
    Server_ptr server(new Server(io_service, endpoint));
    LogFile::Log("status", "Server listening on port 4000");
	
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

    //Connect to mySQL server
    if(!server->sqlQueue->Connect("mud", "localhost", "root"))
    {
        LogFile::Log("error", "main; Could not connect to mySQL server 'mud'");
        LogFile::CloseAll();
        lua_close(server->luaState);
        return 0;
    }

    Lua_DefineClasses(Server::luaState);
    Lua_DefineFunctions(Server::luaState);
    luaL_dofile(Server::luaState, "some_lua.lua"); //Test scripts

    server->Start();

    //Start game loop
    server->GameLoop(server);

    server->Stop(); //calls remove_all_clients()

    Sleep(3000); //TODO wait for asio to stop?

	lua_close(Server::luaState);
    server->sqlQueue->Disconnect();
    LogFile::CloseAll();

    return 0;
}
