#ifndef CCLIENT_H
#define CCLIENT_H

#include <list>
#include <deque>
#include <memory>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>

class User;

#define NETWORK_BUFFER_SIZE 16384 //allow for rather large sends server->client
#define MAX_COMMAND_LENGTH 512	  //truncate incoming commands at this length
#define MAX_INPUT_BUFFER 1024	  //disconnect clients if the input buffer gets this big

struct OVERLAPPEDEX : OVERLAPPED
{
	WSABUF			wsabuf;
	char			buffer[NETWORK_BUFFER_SIZE];
	int				totalBytes;
	int				sentBytes;
	int				opCode;

	OVERLAPPEDEX()
	{
		wsabuf.buf = buffer;
		wsabuf.len = NETWORK_BUFFER_SIZE;
		totalBytes = sentBytes = opCode = 0;
		ZeroMemory(buffer, NETWORK_BUFFER_SIZE);
	};
};

class Client
{
	public:
		Client(SOCKET s, std::string ipaddress);
		~Client();
 
		std::string inputBuffer;    //Dump the client receive buffer here, then parse into commandQueue
		std::deque<std::string> commandQueue;

		void SetSocket(SOCKET s);
		SOCKET Socket();
        std::string GetIPAddress();
		void DisconnectServer();
		void DisconnectGame();
		bool IsConnected();
		void RefCountAdjust(const int & i);
		int GetRefCount();

		OVERLAPPEDEX * NewOperationData(int op_type);
		void FreeOperationData(OVERLAPPEDEX * ol);

		CRITICAL_SECTION overlapped_cs; //synchronize access to overlappedData in worker threads, probably necessary, not sure
		CRITICAL_SECTION command_cs;	//for access to the Client::commandQueue between game & server threads
		CRITICAL_SECTION disconnect_cs; //synchronize access to disconnect flags between game & server threads
		CRITICAL_SECTION refcount_cs;	//synchronize access to IOCPReferenceCount between game (Server::deliver()) & iocp worker threads
		bool disconnectFromServer;
		bool disconnectFromGame;

	private:

		SOCKET socket_;
		int IOCPReferenceCount;
		std::string ipaddress_;
		std::list<OVERLAPPEDEX *> overlappedData;
};

#endif