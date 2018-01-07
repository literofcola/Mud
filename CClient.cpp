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
	disconnect = false;
    ZeroMemory(receiveBuffer, NETWORK_BUFFER_SIZE);
	InitializeCriticalSection(&overlapped_cs);
	InitializeCriticalSection(&command_cs);
}

Client::~Client()
{
    delete[] receiveBuffer;
	//cancel pending operations
	CancelIo((HANDLE)socket_);
	SleepEx(0, TRUE); // the completion will be called here
	closesocket(socket_);

	//shutdown(socket_, SD_BOTH);

	DeleteCriticalSection(&overlapped_cs);
	DeleteCriticalSection(&command_cs);

	std::list<OVERLAPPEDEX *>::iterator iter = overlappedData.begin();
	while(iter != overlappedData.end())
	{
		if(*iter)
			delete (*iter); //THIS OVERLAPPED DATA IS BEING WRITTEN TO AFTER DELETION!!!
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

