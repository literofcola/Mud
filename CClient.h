#ifndef CCLIENT_H
#define CCLIENT_H

#define NETWORK_BUFFER_SIZE 2048

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
		Client(SOCKET s);
		~Client();

		char * receiveBuffer;       
		std::string inputBuffer;    //Dump the receive buffer here, then parse into commandQueue
		std::deque<std::string> commandQueue;
		bool disconnect;

		//Get/Set calls
		/*void SetOpCode(int n);
		int GetOpCode(int op_id);
		void SetTotalBytes(int n, int op_id);
		int GetTotalBytes(int op_id);
		void SetSentBytes(int n, int op_id);
		void IncrSentBytes(int n, int op_id);
		int GetSentBytes(int op_id);*/
		void SetSocket(SOCKET s);
		SOCKET Socket();
		/*void SetWSABUFLength(int nLength);
		int GetWSABUFLength();
		WSABUF* GetWSABUFPtr();
		OVERLAPPED* GetOVERLAPPEDPtr();
		void ResetWSABUF();*/
		OVERLAPPEDEXPtr NewOperationData(int op_type);
		void FreeOperationData(OVERLAPPEDEX * ol);
		/*void SetBuffer(char *szBuffer);
		void SetBufferLength(int len);
		void GetBuffer(char *szBuffer);
		WSABUF * GetWSABUFPtr(OVERLAPPEDEXPtr ol);*/
		//OVERLAPPED * GetOVERLAPPEDPtr();
		CRITICAL_SECTION overlapped_cs; //not sure if this is necessary
		CRITICAL_SECTION command_cs; //for access to the Client::commandQueue

	private:

		SOCKET socket_; //accepted socket

		

		std::list<OVERLAPPEDEXPtr> overlappedData;

		//OVERLAPPED		*m_pol;
		//WSABUF            *m_pwbuf;

		//int               m_nTotalBytes;
		//int               m_nSentBytes;
		//int id_count;
};

#endif