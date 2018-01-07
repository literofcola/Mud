#ifndef CCLIENT_H
#define CCLIENT_H

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

typedef boost::shared_ptr<OVERLAPPEDEX> OVERLAPPEDEXPtr;

class Client
{
	public:
		Client(SOCKET s, std::string ipaddress);
		~Client();

		char * receiveBuffer;       
		std::string inputBuffer;    //Dump the receive buffer here, then parse into commandQueue
		std::deque<std::string> commandQueue;
		bool disconnect;

		void SetSocket(SOCKET s);
		SOCKET Socket();
        std::string GetIPAddress();

		OVERLAPPEDEX * NewOperationData(int op_type);
		void FreeOperationData(OVERLAPPEDEX * ol);

		CRITICAL_SECTION overlapped_cs; //not sure if this is necessary
		CRITICAL_SECTION command_cs; //for access to the Client::commandQueue

	private:

		SOCKET socket_; //accepted socket

		std::string ipaddress_;

		std::list<OVERLAPPEDEX *> overlappedData;
};

#endif