#ifndef CSERVER_H
#define CSERVER_H

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#define SPAM_INTERVAL 60
#define SPAM_BANTIME 900						//15 minutes
#define SPAM_MAX_CONNECTIONS_PER_INTERVAL 15	//15 connections over the last 60 seconds = 15 min ban
//#define SPAM_MAX_CONNECTIONS 15				//Total max connections allowed per IP address - On second thought, nah. Would require checking the clientList anyway

struct IPAddressInfo
{
	std::string address;
	std::vector<double> connectTimes;
};

struct IPBanInfo
{
	std::string address;
	double banTime;
};

class Server
{
public:
	Server(Game * g, int port);
	~Server();

	bool Initialize();
	void Start();
	void DeInitialize();

	static DWORD WINAPI AcceptThread(void * lParam);
	void AcceptConnection(SOCKET ListenSocket);
	bool AssociateWithIOCP(Client * pClientContext);
	static DWORD WINAPI WorkerThread(void * arg);
    void deliver(Client * client, const std::string msg);
	void deliver(Client * c, const unsigned char * msg, int length);
	void AddClient(std::shared_ptr<Client> client);
	void RemoveClient(std::shared_ptr<Client> client);
	void RemoveClient(Client * client);
	void UpdateIPList(std::string address);
	bool CheckTempBanList(std::string address);

	SOCKET ListenSocket;
	struct sockaddr_in ServerAddress;
	HANDLE hShutdownEvent;
	int nThreads;
	HANDLE *phWorkerThreads;
	HANDLE hAcceptThread;
	WSAEVENT hAcceptEvent;
	HANDLE hIOCompletionPort;
    CRITICAL_SECTION clientListCS;
	
    static mySQLQueue * sqlQueue;
	static sol::state lua;
	static std::mt19937_64 rand;

private:

	int nPort;
	Game * mygame;
	std::list<std::shared_ptr<Client>> clients;	//server owns the clients, user->client points into this list
	std::vector<struct IPAddressInfo> IPList;	//List of IP addresses and their connection counts for anti connection spam
	std::vector<struct IPBanInfo> tempBanList;
};

#endif //CSERVER_H