#ifndef CCLIENT_H
#define CCLIENT_H

class User;

#define NETWORK_BUFFER_SIZE 16384

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

		char * receiveBuffer;       
		std::string inputBuffer;    //Dump the receive buffer here, then parse into commandQueue
		std::deque<std::string> commandQueue;
		//bool disconnect;

		void SetSocket(SOCKET s);
		SOCKET Socket();
        //void CloseSocketAndSleep();
        std::string GetIPAddress();
		//User * GetUser();
		//void SetUser(User * u);
		void DisconnectServer();
		void DisconnectGame();
		bool IsConnected();
		void RefCountAdjust(const int & i);
		int GetRefCount();

		OVERLAPPEDEX * NewOperationData(int op_type);
		void FreeOperationData(OVERLAPPEDEX * ol);

		CRITICAL_SECTION overlapped_cs; //synchronize access to overlappedData in worker threads, not sure if this is necessary
		CRITICAL_SECTION command_cs;	//for access to the Client::commandQueue between game & server threads
		CRITICAL_SECTION disconnect_cs; //synchronize access to disconnect flags between game & server threads
		CRITICAL_SECTION refcount_cs;	//synchronize access to IOCPReferenceCount between game & iocp worker threads
		bool disconnectFromServer;
		bool disconnectFromGame;
		

	private:

		SOCKET socket_; //accepted socket
		int IOCPReferenceCount;
		std::string ipaddress_;
		User * user_;
		std::list<OVERLAPPEDEX *> overlappedData;
};

#endif