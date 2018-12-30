#include "stdafx.h"
#include "CClient.h"
#include "CCommand.h"

Client::Client(SOCKET s, std::string ipaddress) : socket_(s), ipaddress_(ipaddress)
{
    commandQueue.clear();
    inputBuffer.clear();
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
	EnterCriticalSection(&overlapped_cs);
	std::list<OVERLAPPEDEX *>::iterator iter = overlappedData.begin();
	while(iter != overlappedData.end())
	{
		if(*iter)
			delete (*iter);
		iter = overlappedData.erase(iter);
	}
	overlappedData.clear();
	LeaveCriticalSection(&overlapped_cs);

	DeleteCriticalSection(&overlapped_cs);
	DeleteCriticalSection(&command_cs);
	DeleteCriticalSection(&disconnect_cs);
	DeleteCriticalSection(&refcount_cs);

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

OVERLAPPEDEX * Client::NewOperationData(int op_type)
{
	OVERLAPPEDEX * ol = new OVERLAPPEDEX();
	ZeroMemory(ol, sizeof(OVERLAPPED));
	ol->opCode = op_type;

	EnterCriticalSection(&overlapped_cs); 
	overlappedData.push_back(ol);
	LeaveCriticalSection(&overlapped_cs);

	return ol;
}

void Client::FreeOperationData(OVERLAPPEDEX *  ol)
{
	EnterCriticalSection(&overlapped_cs);  //not sure if we need these
	std::list<OVERLAPPEDEX *>::iterator iter;
	for(iter = overlappedData.begin(); iter != overlappedData.end(); iter++)
	{
		if((*iter) == ol)
		{
			delete (*iter);
			(*iter) = nullptr;
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
	//bool ret = (!disconnectFromGame && !disconnectFromServer);
	bool ret = !disconnectFromServer;
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