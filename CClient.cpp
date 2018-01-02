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
    receiveBuffer = new char[MAX_INPUT_LENGTH];
    commandQueue.clear();
    inputBuffer.clear();
	disconnect = false;
    ZeroMemory(receiveBuffer, MAX_INPUT_LENGTH);

	//m_pol = new OVERLAPPED;
	//m_pwbuf = new WSABUF;

	//ZeroMemory(m_pol, sizeof(OVERLAPPED));

	//m_pwbuf->buf = NULL;
	//m_pwbuf->len = 0;

	//m_nOpCode = 0;
	//m_nTotalBytes = 0;
	//m_nSentBytes = 0;
	InitializeCriticalSection(&critical_section);
}

Client::~Client()
{
    delete[] receiveBuffer;

	//Wait for the pending operations to complete
	/*while (!HasOverlappedIoCompleted(m_pol))
	{
		Sleep(0);
	}*/

	closesocket(socket_);
	DeleteCriticalSection(&critical_section);

	//Cleanup
	//delete m_pol;
	//delete m_pwbuf;
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
	char			buffer[MAX_INPUT_LENGTH];
	int				totalBytes;
	int				sentBytes;
	int				opCode;

	OVERLAPPEDEX()
	{
		wsabuf.buf = buffer;
		wsabuf.len = MAX_INPUT_LENGTH;
		totalBytes = sentBytes = opCode = 0;
		ZeroMemory(buffer, MAX_INPUT_LENGTH);
	};
};
*/

OVERLAPPEDEXPtr Client::NewOperationData(int op_type)
{
	OVERLAPPEDEXPtr ol(new OVERLAPPEDEX);
	ZeroMemory(ol.get(), sizeof(OVERLAPPED));
	ol->opCode = op_type;

	EnterCriticalSection(&critical_section); 
	overlappedData.push_back(ol);
	LeaveCriticalSection(&critical_section);

	return ol;
}

void Client::FreeOperationData(OVERLAPPEDEX *  ol)
{
	//not sure if we need these
	EnterCriticalSection(&critical_section); 
	std::list<OVERLAPPEDEXPtr>::iterator iter;
	for(iter = overlappedData.begin(); iter != overlappedData.end(); iter++)
	{
		if(iter->get() == ol)
		{
			overlappedData.erase(iter);
			break;
		}
	}
	//overlappedData.remove(ol);
	//magic vector erase by value
	//overlappedData.erase(std::remove(overlappedData.begin(), overlappedData.end(), ol), overlappedData.end());
	LeaveCriticalSection(&critical_section);
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
	strcpy_s(overlappedData[op_id]->wsabuf.buf, MAX_INPUT_LENGTH, theBuffer);
}*/

/*void Client::GetBuffer(char *theBuffer, int op_id)
{
	strcpy_s(theBuffer, MAX_INPUT_LENGTH, overlappedData[op_id]->wsabuf.buf);
}*/
