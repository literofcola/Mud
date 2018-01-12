#ifndef CSERVER_H
#define CSERVER_H

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Server
{
public:
	Server(Game * g, int port);
	~Server();

	bool Initialize();
	void Start();
	void DeInitialize();
    void Stop();

    //static unsigned int __stdcall Run(void * lpParam);

    //void handle_accept(Client_ptr client, const asio::error_code& error);
    void deliver(Client * client, const std::string msg);
	void deliver(Client * c, const unsigned char * msg, int length);

    //void start_client(Client_ptr c);
	//void remove_client(Client_ptr client);
	void DisconnectAllClients();
    //void handle_read(Client_ptr client, const asio::error_code & error);
    //void handle_write(Client_ptr client, const asio::error_code& error);

    //void GameLoop(Server * server);
    //void ShutdownGame();

	//std::list<Client_ptr> clients;

	SOCKET ListenSocket;
	struct sockaddr_in ServerAddress;
	HANDLE hShutdownEvent;
	int nThreads;
	HANDLE *phWorkerThreads;
	HANDLE hAcceptThread;
	WSAEVENT hAcceptEvent;
	HANDLE hIOCompletionPort;
	//std::vector<Client *> g_Clients;
    CRITICAL_SECTION critical_section;

	static DWORD WINAPI AcceptThread(void * lParam);
	void AcceptConnection(SOCKET ListenSocket);
	bool AssociateWithIOCP(Client * pClientContext);
	static DWORD WINAPI WorkerThread(void * arg);

    static mySQLQueue * sqlQueue;
    static lua_State * luaState;

private:

	int nPort;
	Game * mygame;
    std::list<Client *> clients; //server owns the clients, user->client points into this list
};

#endif //CSERVER_H