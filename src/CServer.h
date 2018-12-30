#ifndef CSERVER_H
#define CSERVER_H

#include "CmySQLQueue.h"

#define MXP_OPEN        "\033[0z" //Open 	Only open commands (eg. <B>) will be interpreted.
#define MXP_SECURE      "\033[1z" //Secure 	All MXP tags are interpreted.
#define MXP_LOCKED      "\033[2z" //Locked 	No MXP tags or entities will be interpreted.
#define MXP_RESET       "\033[3z" //close all open tags.  Set mode to Open.  Set text color and properties to default.
#define MXP_TEMPSECURE  "\033[4z" //set secure mode for the next tag only.  Must be immediately followed by a < character to start a tag.  Remember to set secure mode when closing the tag also.
#define MXP_LOCKOPEN    "\033[5z" //set open mode.  Mode remains in effect until changed.  OPEN mode becomes the new default mode.
#define MXP_LOCKSECURE  "\033[6z" //set secure mode.  Mode remains in effect until changed.  Secure mode becomes the new default mode.
#define MXP_LOCKLOCKED  "\033[7z" //set locked mode.  Mode remains in effect until changed.  Locked mode becomes the new default mode.

#define SPAM_INTERVAL 60
#define SPAM_BANTIME 900						//15 minutes
#define SPAM_MAX_CONNECTIONS_PER_INTERVAL 15	//15 connections over the last 60 seconds = 15 min ban
//#define SPAM_MAX_CONNECTIONS 15				//Total max connections allowed per IP address - On second thought, nah. Would require checking the clientList anyway

struct IPAddressInfo
{
	std::string address;
	std::vector<double> connectTimestamps;
};

struct IPBanInfo
{
	std::string address;
	double banTimestamp;
};

class Game;
class Client;

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
	std::string EncryptDecrypt(std::string toEncrypt);
	std::string SQLSelectPassword(std::string name);

	SOCKET ListenSocket;
	struct sockaddr_in ServerAddress;
	HANDLE hShutdownEvent;
	int nThreads;
	HANDLE *phWorkerThreads;
	HANDLE hAcceptThread;
	WSAEVENT hAcceptEvent;
	HANDLE hIOCompletionPort;
    CRITICAL_SECTION clientListCS;
	bool acceptReady;
	
    static mySQLQueue * sqlQueue;
	static sol::state lua;
	static std::mt19937_64 rand;

	//Defines from telnet protocol we need for MXP, MCCP, etc (getting a little crowded, declare as #define?)
	static const std::string IAC;
	static const std::string WILL;
	static const std::string SB;
	static const std::string SE;
	static const std::string DO;
	static const std::string DONT;

	static const std::string MXP_WILL;
	static const std::string MXP_START;
	static const std::string MXP_DO;
	static const std::string MXP_DONT;

	static const std::string MCCP_WILL;
	static const std::string MCCP_START;
	static const std::string MCCP_DO;
	static const std::string MCCP_DONT;

	static const std::string GMCP_WILL;
	static const std::string GMCP_START;
	static const std::string GMCP_DO;
	static const std::string GMCP_DONT;

	static const std::string TELOPT_MXP;  // (91)
	static const std::string TELOPT_MCCP; //MCCP 2 (86)
	static const std::string TELOPT_GMCP; //201

private:

	int nPort;
	Game * mygame;
	std::list<std::shared_ptr<Client>> clients;	//server owns the clients, shared_ptr because user->client points into this list
	std::vector<struct IPAddressInfo> IPList;	//List of IP addresses and their connection counts for anti connection spam
	std::vector<struct IPBanInfo> tempBanList;  //Ban list for connection spam

	std::string encryptionKey;
};

#endif //CSERVER_H