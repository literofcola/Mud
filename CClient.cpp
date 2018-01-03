#include "stdafx.h"
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

using namespace std;

Client::Client(SOCKET s) : socket_(s)
{
    receiveBuffer = new char[NETWORK_BUFFER_SIZE];
    commandQueue.clear();
    inputBuffer.clear();
	disconnect = false;
    ZeroMemory(receiveBuffer, NETWORK_BUFFER_SIZE);
	InitializeCriticalSection(&overlapped_cs);
	InitializeCriticalSection(&command_cs);
}

Client::~Client()
{
    delete[] receiveBuffer;
	//Wait for the pending operations to complete
	CancelIo((HANDLE)socket_);
	SleepEx(0, TRUE); // the completion will be called here
	closesocket(socket_);

	//shutdown(socket_, SD_BOTH);

	DeleteCriticalSection(&overlapped_cs);
	DeleteCriticalSection(&command_cs);
	overlappedData.clear();
}

//Get/Set calls
/*void Client::SetOpCode(int n, int op_id)
{
	overlappedData[op_id]->opCode = n;
}

int Client::GetOpCode(int op_id)
{
	return overlappedData[op_id]->opCode;
}

void Client::SetTotalBytes(int n, int op_id)
{
	overlappedData[op_id]->totalBytes = n;
}

int Client::GetTotalBytes(int op_id)
{
	return overlappedData[op_id]->totalBytes;
}

void Client::SetSentBytes(int n, int op_id)
{
	overlappedData[op_id]->sentBytes = n;
}

void Client::IncrSentBytes(int n, int op_id)
{
	overlappedData[op_id]->sentBytes += n;
}

int Client::GetSentBytes(int op_id)
{
	return overlappedData[op_id]->sentBytes;
}*/

void Client::SetSocket(SOCKET s)
{
	socket_ = s;
}

SOCKET Client::Socket()
{
	return socket_;
}
/*
typedef boost::shared_ptr<OVERLAPPEDEX> OVERLAPPEDEXPtr;
std::vector<OVERLAPPEDEXPtr> overlappedData;
*/
/*
struct OVERLAPPEDEX : OVERLAPPED 
{
	//base OVERLAPPED: 
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
*/

OVERLAPPEDEXPtr Client::NewOperationData(int op_type)
{
	OVERLAPPEDEXPtr ol(new OVERLAPPEDEX);
	ZeroMemory(ol.get(), sizeof(OVERLAPPED));
	ol->opCode = op_type;

	EnterCriticalSection(&overlapped_cs); 
	overlappedData.push_back(ol);
	LeaveCriticalSection(&overlapped_cs);

	return ol;
}

void Client::FreeOperationData(OVERLAPPEDEX *  ol)
{
	//not sure if we need these
	EnterCriticalSection(&overlapped_cs); 
	std::list<OVERLAPPEDEXPtr>::iterator iter;
	for(iter = overlappedData.begin(); iter != overlappedData.end(); iter++)
	{
		if(iter->get() == ol)
		{
			overlappedData.erase(iter);
			break;
		}
	}
	LeaveCriticalSection(&overlapped_cs);
}


/*WSABUF * Client::GetWSABUFPtr(OVERLAPPEDEXPtr ol)
{
	return &ol->wsabuf;
}*/

/*OVERLAPPED * Client::GetOVERLAPPEDPtr(int op_id)
{
	return &(OVERLAPPED)overlappedData[op_id]->overlapped;
}*/

/*void Client::SetBufferLength(int len, OVERLAPPEDEXPtr ol)
{
	
	overlappedData[op_id]->wsabuf.len = len;
}*/

/*void Client::SetBuffer(char *theBuffer, int op_id)
{
	strcpy_s(overlappedData[op_id]->wsabuf.buf, NETWORK_BUFFER_SIZE, theBuffer);
}*/

/*void Client::GetBuffer(char *theBuffer, int op_id)
{
	strcpy_s(theBuffer, NETWORK_BUFFER_SIZE, overlappedData[op_id]->wsabuf.buf);
}*/
