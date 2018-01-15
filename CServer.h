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

    void deliver(Client * client, const std::string msg);
	void deliver(Client * c, const unsigned char * msg, int length);

	//void DisconnectAllClients();

	void AddClient(std::shared_ptr<Client> client);
	void RemoveClient(std::shared_ptr<Client> client);
	void RemoveClient(Client * client);

	SOCKET ListenSocket;
	struct sockaddr_in ServerAddress;
	HANDLE hShutdownEvent;
	int nThreads;
	HANDLE *phWorkerThreads;
	HANDLE hAcceptThread;
	WSAEVENT hAcceptEvent;
	HANDLE hIOCompletionPort;
	//std::vector<Client *> g_Clients;
    CRITICAL_SECTION clientListCS;

	static DWORD WINAPI AcceptThread(void * lParam);
	void AcceptConnection(SOCKET ListenSocket);
	bool AssociateWithIOCP(Client * pClientContext);
	static DWORD WINAPI WorkerThread(void * arg);

    static mySQLQueue * sqlQueue;
	static sol::state lua;

private:

	int nPort;
	Game * mygame;
    //std::list<Client *> clients; //server owns the clients, user->client points into this list
	std::list<std::shared_ptr<Client>> clients;
	;
};

#endif //CSERVER_H