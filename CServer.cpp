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

//Op codes for IOCP
#define OP_READ     0
#define OP_WRITE    1

//Time out interval for wait calls
#define WAIT_TIMEOUT_INTERVAL 100

//CRITICAL_SECTION Server::critical_section;
lua_State * Server::luaState;
mySQLQueue * Server::sqlQueue;

Server::Server(Game * g, int port) : nPort(port), mygame(g)
{
	hShutdownEvent = NULL;
	nThreads = 1;
	//*phWorkerThreads = NULL;
	hAcceptThread = NULL;
	hIOCompletionPort = NULL;
}

Server::~Server()
{
   
}

bool Server::Initialize()
{
	//Find out number of processors and threads
	SYSTEM_INFO si;
	GetSystemInfo(&si);
	nThreads = si.dwNumberOfProcessors;

	//Allocate memory to store thread handles
	phWorkerThreads = new HANDLE[nThreads];

	InitializeCriticalSection(&critical_section);

	//Create shutdown event
	hShutdownEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	// Initialize Winsock
	WSADATA wsaData;

	int nResult;
	nResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

	if (NO_ERROR != nResult)
	{
		//printf("\nError occurred while executing WSAStartup().");
		return false; //error
	}

	//Create I/O completion port
	hIOCompletionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0);

	if(NULL == hIOCompletionPort)
	{
		//printf("\nError occurred while creating IOCP: %d.", WSAGetLastError());
		return false;
	}

	//Create worker threads
	DWORD nThreadID;
	for (int ii = 0; ii < nThreads; ii++)
	{
		phWorkerThreads[ii] = CreateThread(0, 0, WorkerThread, this, 0, &nThreadID);
	}

	//Overlapped I/O follows the model established in Windows and can be performed only on 
	//sockets created through the WSASocket function 
	ListenSocket = WSASocketW(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);

	if (INVALID_SOCKET == ListenSocket)
	{
		//printf("\nError occurred while opening socket: %d.", WSAGetLastError());
		closesocket(ListenSocket);
		return false;
	}

	//Cleanup and Init with 0 the ServerAddress
	ZeroMemory((char *)&ServerAddress, sizeof(ServerAddress));

	//Fill up the address structure
	ServerAddress.sin_family = AF_INET;
	ServerAddress.sin_addr.s_addr = INADDR_ANY; //WinSock will supply address
	ServerAddress.sin_port = htons(nPort);   

	//Assign local address and port number
	if(SOCKET_ERROR == bind(ListenSocket, (struct sockaddr *) &ServerAddress, sizeof(ServerAddress)))
	{
		closesocket(ListenSocket);
		//printf("\nError occurred while binding.");
		return false;
	}

	//Make the socket a listening socket
	if(SOCKET_ERROR == listen(ListenSocket, SOMAXCONN))
	{
		closesocket(ListenSocket);
		//printf("\nError occurred while listening.");
		return false;
	}

	hAcceptEvent = WSACreateEvent();

	if(WSA_INVALID_EVENT == hAcceptEvent)
	{
		//printf("\nError occurred while WSACreateEvent().");
		return false;
	}

	if (SOCKET_ERROR == WSAEventSelect(ListenSocket, hAcceptEvent, FD_ACCEPT))
	{
		//printf("\nError occurred while WSAEventSelect().");
		WSACloseEvent(hAcceptEvent);
		return false;
	}

	sqlQueue = new mySQLQueue();

	return true;
}

void Server::Start()
{
	DWORD nThreadID;
	hAcceptThread = CreateThread(0, 0, AcceptThread, (void *)this, 0, &nThreadID);
}

void Server::DeInitialize()
{
	closesocket(ListenSocket);

	//Ask all threads to start shutting down
	SetEvent(hShutdownEvent);

	//Let Accept thread go down
	WaitForSingleObject(hAcceptThread, INFINITE);

    std::list<User *>::iterator iter;
    iter = mygame->users.begin();
	while(iter != mygame->users.end())
	{
		User * user = (*iter);
        if(user->GetClient())
            closesocket(user->GetClient()->Socket());
		user->Disconnect(); //just sets client to NULL

        //Save user/player
        if(user->character)
        {
            if(user->connectedState == User::CONN_PLAYING) //don't save fresh characters
                user->character->Save();
            //user->character->NotifyListeners();
            mygame->characters.remove(user->character);
            //RemoveCharacter(user->character);
        }
        delete user;
        //users.remove(user);
        iter = mygame->users.erase(iter);
    }

	for(int i = 0; i < nThreads; i++)
	{
		//Help threads get out of blocking - GetQueuedCompletionStatus()
		PostQueuedCompletionStatus(hIOCompletionPort, 0, (DWORD)NULL, NULL);
	}

	//Let Worker Threads shutdown
	WaitForMultipleObjects(nThreads, phWorkerThreads, TRUE, INFINITE);

	//We are done with this event
	WSACloseEvent(hAcceptEvent);

	//Cleanup dynamic memory allocations, if there are any.
	//CleanClientList();

	DeleteCriticalSection(&critical_section);

	//Cleanup IOCP.
	CloseHandle(hIOCompletionPort);

	//Clean up the event.
	CloseHandle(hShutdownEvent);

	//Clean up memory allocated for the storage of thread handles
	delete[] phWorkerThreads;

	//Cleanup Winsock
	WSACleanup();

    if(!clients.empty())
        LogFile::Log("error", "server client list is NOT empty at shutdown");
}

//This thread will look for accept event
DWORD WINAPI Server::AcceptThread(void * lParam)
{
	Server * theserver = (Server *)lParam;

	WSANETWORKEVENTS WSAEvents;

	//Accept thread will be around to look for accept event, until a Shutdown event is not Signaled.
	while (WAIT_OBJECT_0 != WaitForSingleObject(theserver->hShutdownEvent, 0))
	{
		if (WSA_WAIT_TIMEOUT != WSAWaitForMultipleEvents(1, &theserver->hAcceptEvent, FALSE, WAIT_TIMEOUT_INTERVAL, FALSE))
		{
			WSAEnumNetworkEvents(theserver->ListenSocket, theserver->hAcceptEvent, &WSAEvents);
			if ((WSAEvents.lNetworkEvents & FD_ACCEPT) && (0 == WSAEvents.iErrorCode[FD_ACCEPT_BIT]))
			{
				//Process it
				theserver->AcceptConnection(theserver->ListenSocket);
			}
		}
	}

	return 0;
}

void Server::AcceptConnection(SOCKET ListenSocket)
{
	sockaddr_in ClientAddress;
	int nClientLength = sizeof(ClientAddress);

	//Accept remote connection attempt from the client
	SOCKET Socket = accept(ListenSocket, (sockaddr*)&ClientAddress, &nClientLength);

	if (INVALID_SOCKET == Socket)
	{
		LogFile::Log("error", "accept(): " + Utilities::GetLastErrorAsString());
	}

	//Set TCP_NODELAY (Disable Nagle)
	int value = 1;
	setsockopt(Socket, IPPROTO_TCP, TCP_NODELAY, (const char *)&value, sizeof(value)); 

	//Display Client's IP
	char ipstr[INET6_ADDRSTRLEN];
	//LogFile::Log("error", "Client connected from: " + inet_ntop(AF_INET, &ClientAddress.sin_addr, ipstr, sizeof(ipstr)));
	std::string addr = inet_ntoa(ClientAddress.sin_addr);

	LogFile::Log("error", "Client connected from: " + addr);

	//Create a new ClientContext for this newly accepted client
	Client * pClientContext = new Client(Socket, addr);

    clients.push_back(pClientContext);
	mygame->NewUser(pClientContext);

	if (true == AssociateWithIOCP(pClientContext))
	{
		OVERLAPPEDEX * operationData = pClientContext->NewOperationData(OP_READ);
		OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(operationData);

		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		//Post initial Recv
		int err = WSARecv(pClientContext->Socket(), &operationData->wsabuf, 1, NULL, &dwFlags, base_overlapped, NULL);

		if ((SOCKET_ERROR == err) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			//LogFile::Log("error", "WSARecv(): " + Utilities::GetLastErrorAsString());
			if(pClientContext)
            {
                LogFile::Log("status", "Disconnect from " + pClientContext->GetIPAddress());
                pClientContext->FreeOperationData(operationData);
                closesocket(pClientContext->Socket());
				mygame->RemoveUser(pClientContext);
            }
		}
	}
}

bool Server::AssociateWithIOCP(Client * pClientContext)
{
	//Associate the socket with IOCP
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pClientContext->Socket(), hIOCompletionPort, (ULONG_PTR)pClientContext, 0);

	if (NULL == hTemp)
	{
		LogFile::Log("error", "CreateIoCompletionPort(): " + Utilities::GetLastErrorAsString());

		//Let's not work with this client
		if(pClientContext)
        {
            LogFile::Log("status", "Disconnect from " + pClientContext->GetIPAddress());
            closesocket(pClientContext->Socket());
			mygame->RemoveUser(pClientContext);
        }
		return false;
	}

	return true;
}

//Worker thread will service IOCP requests
DWORD WINAPI Server::WorkerThread(void * lpParam)
{
	Server * thisserver = (Server*)lpParam;

	void *lpContext = NULL;
	OVERLAPPED       *pOverlapped = NULL;
	Client   *pClientContext = NULL;
	DWORD            dwBytesTransfered = 0;
	int nBytesRecv = 0;
	int nBytesSent = 0;
	DWORD             dwBytes = 0, dwFlags = 0;

	//Worker thread will be around to process requests, until a Shutdown event is not Signaled.
	while (WAIT_OBJECT_0 != WaitForSingleObject(thisserver->hShutdownEvent, 0))
	{
		BOOL bReturn = GetQueuedCompletionStatus(
			thisserver->hIOCompletionPort,
			&dwBytesTransfered,
			(PULONG_PTR)&lpContext,
			&pOverlapped,
			INFINITE);

        LogFile::Log("status", "GQCP worker awake # " + Utilities::itos(GetCurrentThreadId()));


		if (NULL == lpContext)
		{
			//We are shutting down
			break;
		}

		//Get the client context
		pClientContext = (Client *)lpContext;
		//Get the extended overlapped structure
		OVERLAPPEDEX * pOverlappedEx = static_cast<OVERLAPPEDEX*>(pOverlapped);

        if(bReturn && WSAGetLastError() == ERROR_SUCCESS && pClientContext != NULL && dwBytesTransfered == 0)
        {
            LogFile::Log("error", "GetQueuedCompletionStatus(): " + Utilities::GetLastErrorAsString());
            thisserver->clients.remove(pClientContext);
            continue;
        }

		if(!bReturn)
		{
			//LogFile::Log("error", "GetQueuedCompletionStatus(): " + Utilities::GetLastErrorAsString());
			if(pClientContext)
            {
                if(pOverlapped == NULL)
                    LogFile::Log("status", "(!bReturn) && pOverlapped == NULL");
                //LogFile::Log("status", "(!bReturn) Disconnect from " + pClientContext->GetIPAddress());
                //closesocket(pClientContext->Socket());
                //pClientContext->CloseSocketAndSleep();
				//thisserver->mygame->RemoveUser(pClientContext);
                thisserver->clients.remove(pClientContext);
            }
			continue;
		}

		switch (pOverlappedEx->opCode)
		{
			case OP_WRITE:
				pOverlappedEx->sentBytes += dwBytesTransfered;

				//Write operation was finished, see if all the data was sent.
				//Else post another write.
				if(pOverlappedEx->sentBytes < pOverlappedEx->totalBytes)
				{
					pOverlappedEx->opCode = (OP_WRITE);

					pOverlappedEx->wsabuf.buf += pOverlappedEx->sentBytes;
					pOverlappedEx->wsabuf.len = pOverlappedEx->totalBytes - pOverlappedEx->sentBytes;

					dwFlags = 0;

					//Overlapped send
					//nBytesSent = WSASend(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, &dwBytes, dwFlags, pOverlapped, NULL);
					nBytesSent = WSASend(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, NULL, 0, pOverlapped, NULL);

					if ((SOCKET_ERROR == nBytesSent) && (WSA_IO_PENDING != WSAGetLastError()))
					{
						//LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
						if(pClientContext)
                        {
                            LogFile::Log("status", "(WSASend: OP_WRITE) Disconnect from " + pClientContext->GetIPAddress());
                            closesocket(pClientContext->Socket());
						    thisserver->mygame->RemoveUser(pClientContext);
                        }
					}
				}
				else
				{
					pClientContext->FreeOperationData(pOverlappedEx);
				}
				break;

			case OP_READ:

				char localBuffer[NETWORK_BUFFER_SIZE];
				strcpy_s(localBuffer, NETWORK_BUFFER_SIZE, pOverlappedEx->buffer);

				//LogFile::Log("network", "Thread ?: The following message was received: " + std::string(localBuffer));

				//append the new input
				std::string append = localBuffer;
				pClientContext->inputBuffer += append;
				//search for \n \r
				size_t cr_pos = pClientContext->inputBuffer.find('\r');
				size_t nl_pos = pClientContext->inputBuffer.find('\n');
				while(cr_pos != std::string::npos && nl_pos != std::string::npos)
				{
					//Copy a single command
					if(cr_pos < nl_pos) //telnet sends carriage return first... 
					{
						EnterCriticalSection(&pClientContext->command_cs);
						pClientContext->commandQueue.push_back(pClientContext->inputBuffer.substr(0, cr_pos));
						LeaveCriticalSection(&pClientContext->command_cs);
						if(nl_pos != std::string::npos)
							pClientContext->inputBuffer.erase(0, nl_pos+1);
						else
							pClientContext->inputBuffer.erase(0, cr_pos+1);
					}
					else if(nl_pos < cr_pos) //...is it ever done any other way?
					{
						EnterCriticalSection(&pClientContext->command_cs);
						pClientContext->commandQueue.push_back(pClientContext->inputBuffer.substr(0, nl_pos));
						LeaveCriticalSection(&pClientContext->command_cs);
						if(cr_pos != std::string::npos)
							pClientContext->inputBuffer.erase(0, cr_pos+1);
						else
							pClientContext->inputBuffer.erase(0, nl_pos+1);
					}
					cr_pos = pClientContext->inputBuffer.find('\r');
					nl_pos = pClientContext->inputBuffer.find('\n');
				}
				ZeroMemory(pClientContext->receiveBuffer, NETWORK_BUFFER_SIZE);

				pOverlappedEx->opCode = (OP_READ);
				ZeroMemory(pOverlappedEx->buffer, NETWORK_BUFFER_SIZE);
				ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
				pOverlappedEx->totalBytes = pOverlappedEx->sentBytes = 0;

				int err = WSARecv(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, NULL, &dwFlags, pOverlapped, NULL);

				if ((SOCKET_ERROR == err) && (WSA_IO_PENDING != WSAGetLastError()))
				{
					//LogFile::Log("error", "WSARecv(): " + Utilities::GetLastErrorAsString());
					if(pClientContext)
                    {
                        LogFile::Log("status", "(WSARecv: OP_READ) Disconnect from " + pClientContext->GetIPAddress());
                        closesocket(pClientContext->Socket());
						thisserver->mygame->RemoveUser(pClientContext);
                    }
				}

				break;
			
			//default:
				//We should never be reaching here, under normal circumstances.
			//	break;
		} // switch
	} // while

	return 0;
}

void Server::deliver(Client * c, const std::string msg)
{
	OVERLAPPEDEX * olptr = c->NewOperationData(OP_WRITE);
	memcpy(olptr->buffer, msg.c_str(), msg.length());
	olptr->wsabuf.len = (DWORD)msg.length();
	olptr->totalBytes = (DWORD)msg.length();
	OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(olptr);

	int wsaerr = WSASend(c->Socket(), &olptr->wsabuf, 1, NULL, 0, base_overlapped, NULL);

	if(wsaerr == 0)
	{
		//immediate success
		LogFile::Log("status", "Immediate sent bytes: " + Utilities::itos(base_overlapped->InternalHigh));
        c->FreeOperationData(olptr);
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//queued
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		//error
        LogFile::Log("status", "(WSASend: deliver immediate) Disconnect from " + c->GetIPAddress());
		//LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
        closesocket(c->Socket());
        c->FreeOperationData(olptr);
		mygame->RemoveUser(c);
	}
}

void Server::deliver(Client * c, const unsigned char * msg, int length)
{
	OVERLAPPEDEX * olptr = c->NewOperationData(OP_WRITE);
	memcpy(olptr->buffer, msg, length);
	olptr->wsabuf.len = length;
	olptr->totalBytes = length;
	OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(olptr);

	int wsaerr = WSASend(c->Socket(), &olptr->wsabuf, 1, NULL, 0, base_overlapped, NULL);

	if(wsaerr == 0)
	{
		//immediate success
		//LogFile::Log("status", "Immediate sent bytes: " + Utilities::itos(base_overlapped->InternalHigh));
        c->FreeOperationData(olptr);
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//queued
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		//error
        LogFile::Log("status", "(WSASend: deliver immediate) Disconnect from " + c->GetIPAddress());
		//LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
        closesocket(c->Socket());
		c->FreeOperationData(olptr);
        mygame->RemoveUser(c);
	}
}

void Server::DisconnectAllClients()
{
	LogFile::Log("network", "Server::remove_all_clients");
	if(mygame->users.empty())
		return;

	std::list<User *>::iterator iter;
	for(iter = mygame->users.begin(); iter != mygame->users.end(); iter++)
	{
		User * u = (*iter);
		if(u->IsConnected())
		{
			u->Disconnect();
		}
	}
}

void Server::Stop()
{
    DisconnectAllClients();
}
