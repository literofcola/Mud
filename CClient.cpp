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

Client::Client(SOCKET s, std::string ipaddress) : socket_(s), ipaddress_(ipaddress)
{
    receiveBuffer = new char[NETWORK_BUFFER_SIZE];
    commandQueue.clear();
    inputBuffer.clear();
	//disconnect = false;
	//user_ = NULL;
    ZeroMemory(receiveBuffer, NETWORK_BUFFER_SIZE);
	InitializeCriticalSection(&overlapped_cs);
	InitializeCriticalSection(&command_cs);
	InitializeCriticalSection(&disconnect_cs);
	InitializeCriticalSection(&refcount_cs);
	disconnectFromServer = false;
	disconnectFromGame = false;
	IOCPReferenceCount = 0;
}

Client::~Client()
{
	//user_ = NULL;
    delete[] receiveBuffer;
	//cancel pending operations
    //closesocket(socket_);	//this is no good, we need to make sure the socket is well and truly closed and all pending IOCP operations are done well before we get here
	//CancelIoEx((HANDLE)socket_, NULL);
	//SleepEx(0, TRUE); // the completion will be called here

	//shutdown(socket_, SD_BOTH);

	DeleteCriticalSection(&overlapped_cs);
	DeleteCriticalSection(&command_cs);
	DeleteCriticalSection(&disconnect_cs);
	DeleteCriticalSection(&refcount_cs);

	std::list<OVERLAPPEDEX *>::iterator iter = overlappedData.begin();
	while(iter != overlappedData.end())
	{
		if(*iter)
			delete (*iter);
		iter = overlappedData.erase(iter);
	}
	overlappedData.clear();
}

std::string Client::GetIPAddress()
{
    return ipaddress_;
}

void Client::SetSocket(SOCKET s)
{
	socket_ = s;
}

SOCKET Client::Socket()
{
	return socket_;
}

/*User * Client::GetUser()
{
	return user_;
};*/

/*void Client::SetUser(User * u)
{
	user_ = u;
};*/

OVERLAPPEDEX * Client::NewOperationData(int op_type)
{
	OVERLAPPEDEX * ol = new OVERLAPPEDEX();
	//OVERLAPPEDEXPtr ol(new OVERLAPPEDEX);
	//ZeroMemory(ol.get(), sizeof(OVERLAPPED));
	ZeroMemory(ol, sizeof(OVERLAPPED));
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
	std::list<OVERLAPPEDEX *>::iterator iter;
	/*for(iter = overlappedData.begin(); iter != overlappedData.end(); iter++)
	{
		if(iter->get() == ol)
		{
			overlappedData.erase(iter);
			break;
		}
	}*/
	for(iter = overlappedData.begin(); iter != overlappedData.end(); iter++)
	{
		if((*iter) == ol)
		{
			delete (*iter);
			(*iter) = NULL;
			overlappedData.erase(iter);
			break;
		}
	}
	LeaveCriticalSection(&overlapped_cs);
}

void Client::DisconnectGame()
{
	EnterCriticalSection(&disconnect_cs);
	disconnectFromGame = true;
	LeaveCriticalSection(&disconnect_cs);
}

void Client::DisconnectServer()
{
	EnterCriticalSection(&disconnect_cs);
	disconnectFromServer = true;
	LeaveCriticalSection(&disconnect_cs);
}

bool Client::IsConnected()
{
	EnterCriticalSection(&disconnect_cs);
	bool ret = (!disconnectFromGame && !disconnectFromServer);
	LeaveCriticalSection(&disconnect_cs);
	return ret;
}

void Client::RefCountAdjust(const int & i)
{
	EnterCriticalSection(&refcount_cs);
	IOCPReferenceCount += i;
	LeaveCriticalSection(&refcount_cs);
}

int Client::GetRefCount()
{
	int ret = 0;
	EnterCriticalSection(&refcount_cs);
	ret = IOCPReferenceCount;
	LeaveCriticalSection(&refcount_cs);
	return ret;
}