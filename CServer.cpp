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

	if(sqlQueue != NULL)
    {
        sqlQueue->Close();
        delete sqlQueue;
    }
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


//This function will process the accept event
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

	//Store this object
	mygame->NewUser(pClientContext);

	if (true == AssociateWithIOCP(pClientContext))
	{
		//pClientContext->SetOpCode(OP_WRITE);
		//boost::shared_ptr<OVERLAPPEDEX> operationData = pClientContext->NewOperationData(OP_READ);
		OVERLAPPEDEX * operationData = pClientContext->NewOperationData(OP_READ);
		OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(operationData);

		//Get data.
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		//Post initial Recv
		//This is a right place to post a initial Recv
		//Posting a initial Recv in WorkerThread will create scalability issues.
		int err = WSARecv(pClientContext->Socket(), &operationData->wsabuf, 1, NULL, &dwFlags, base_overlapped, NULL);

		if ((SOCKET_ERROR == err) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			LogFile::Log("error", "WSARecv(): " + Utilities::GetLastErrorAsString());
			if(pClientContext)
				mygame->RemoveUser(pClientContext);
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
			mygame->RemoveUser(pClientContext);
		return false;
	}

	return true;
}

//Worker thread will service IOCP requests
DWORD WINAPI Server::WorkerThread(void * lpParam)
{
	Server * thisserver = (Server*)lpParam;
	//int nThreadNo = (int)lpParam;

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


		if (NULL == lpContext)
		{
			//We are shutting down
			break;
		}

		//Get the client context
		pClientContext = (Client *)lpContext;
		//Get the extended overlapped structure
		OVERLAPPEDEX * pOverlappedEx = static_cast<OVERLAPPEDEX*>(pOverlapped);

		if(!bReturn)
		{
			LogFile::Log("error", "GetQueuedCompletionStatus(): " + Utilities::GetLastErrorAsString());
			if(pClientContext)
				thisserver->mygame->RemoveUser(pClientContext);
			continue;
		}

		/*if ((bReturn == FALSE) )// || ((bReturn != FALSE) && (0 == dwBytesTransfered)))
		{
			//Client connection gone, remove it.
			thisserver->mygame->RemoveUser(pClientContext);
			continue;
		}*/

		switch (pOverlappedEx->opCode)
		{
			case OP_WRITE:
				pOverlappedEx->sentBytes += dwBytesTransfered;
				//pClientContext->IncrSentBytes(dwBytesTransfered);

				//Write operation was finished, see if all the data was sent.
				//Else post another write.
				if(pOverlappedEx->sentBytes < pOverlappedEx->totalBytes)
				//if (pClientContext->GetSentBytes() < pClientContext->GetTotalBytes())
				{
					pOverlappedEx->opCode = (OP_WRITE);
					//pClientContext->SetOpCode(OP_WRITE);

					pOverlappedEx->wsabuf.buf += pOverlappedEx->sentBytes;
					pOverlappedEx->wsabuf.len = pOverlappedEx->totalBytes - pOverlappedEx->sentBytes;
					//p_wbuf->buf += pClientContext->GetSentBytes();
					//p_wbuf->len = pClientContext->GetTotalBytes() - pClientContext->GetSentBytes();

					dwFlags = 0;

					//Overlapped send
					//nBytesSent = WSASend(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, &dwBytes, dwFlags, pOverlapped, NULL);
					nBytesSent = WSASend(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, NULL, 0, pOverlapped, NULL);

					if ((SOCKET_ERROR == nBytesSent) && (WSA_IO_PENDING != WSAGetLastError()))
					{
						//Let's not work with this client
						LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
						if(pClientContext)
							thisserver->mygame->RemoveUser(pClientContext);
					}
				}
				else
				{
					pClientContext->FreeOperationData(pOverlappedEx);
				}

				break;

			case OP_READ:

				char localBuffer[NETWORK_BUFFER_SIZE];

				//pClientContext->GetBuffer(localBuffer);
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
				/*pClientContext->Socket().async_receive(asio::buffer(client->receiveBuffer, client->NETWORK_BUFFER_SIZE), 
								  boost::bind(&Server::handle_read, shared_from_this(), client, asio::placeholders::error));*/

				/*pClientContext->SetOpCode(OP_READ);
				WSABUF *p_wbuf = pClientContext->GetWSABUFPtr();
				OVERLAPPED *p_ol = pClientContext->GetOVERLAPPEDPtr();
				pClientContext->ResetWSABUF();*/

				pOverlappedEx->opCode = (OP_READ);
				ZeroMemory(pOverlappedEx->buffer, NETWORK_BUFFER_SIZE);
				ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
				pOverlappedEx->totalBytes = pOverlappedEx->sentBytes = 0;

				int err = WSARecv(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, NULL, &dwFlags, pOverlapped, NULL);

				if ((SOCKET_ERROR == err) && (WSA_IO_PENDING != WSAGetLastError()))
				{
					LogFile::Log("error", "WSARecv(): " + Utilities::GetLastErrorAsString());
					if(pClientContext)
						thisserver->mygame->RemoveUser(pClientContext);
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
	//OVERLAPPEDEXPtr olptr = c->NewOperationData(OP_WRITE);
	OVERLAPPEDEX * olptr = c->NewOperationData(OP_WRITE);
	memcpy(olptr->buffer, msg.c_str(), msg.length());
	olptr->wsabuf.len = (DWORD)msg.length();
	olptr->totalBytes = (DWORD)msg.length();
	OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(olptr);

	int wsaerr = WSASend(c->Socket(), &olptr->wsabuf, 1, NULL, 0, base_overlapped, NULL);

	if(wsaerr == 0)
	{
		//immediate success
		//LogFile::Log("status", "Immediate sent bytes: " + Utilities::itos(base_overlapped->InternalHigh));
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//queued
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		//error
		LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
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
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() == WSA_IO_PENDING)
	{
		//queued
	}
	else if(wsaerr == SOCKET_ERROR && WSAGetLastError() != WSA_IO_PENDING)
	{
		//error
		LogFile::Log("error", "WSASend(): " + Utilities::GetLastErrorAsString());
		mygame->RemoveUser(c);
	}
}

/*
void Server::handle_read(Client_ptr client, const asio::error_code & error)
{
    if(!client)
		return;

    if (!error)
    {
        if(client->disconnect)
		{
			remove_client(client);
            return;
		}

        //int bytesRead = (int)strlen(client->receiveBuffer);

        //append the new input
        client->inputBuffer += client->receiveBuffer;
        //search for \n \r
        size_t cr_pos = client->inputBuffer.find('\r');
        size_t nl_pos = client->inputBuffer.find('\n');
        while(cr_pos != string::npos && nl_pos != string::npos)
        {
		    //Copy a single command
		    if(cr_pos < nl_pos) //telnet sends carriage return first... 
		    {
			    //EnterCriticalSection(&Server::critical_section);
                EnterCriticalSection(&critical_section);
			    client->commandQueue.push_back(client->inputBuffer.substr(0, cr_pos));
			    //LeaveCriticalSection(&Server::critical_section);
                LeaveCriticalSection(&critical_section);
			    if(nl_pos != std::string::npos)
				    client->inputBuffer.erase(0, nl_pos+1);
			    else
				    client->inputBuffer.erase(0, cr_pos+1);
		    }
		    else if(nl_pos < cr_pos) //...is it ever done any other way?
		    {
			    //EnterCriticalSection(&Server::critical_section);
                EnterCriticalSection(&critical_section);
			    client->commandQueue.push_back(client->inputBuffer.substr(0, nl_pos));
			    //LeaveCriticalSection(&Server::critical_section);
                LeaveCriticalSection(&critical_section);
			    if(cr_pos != std::string::npos)
				    client->inputBuffer.erase(0, cr_pos+1);
			    else
				    client->inputBuffer.erase(0, nl_pos+1);
		    }
            cr_pos = client->inputBuffer.find('\r');
            nl_pos = client->inputBuffer.find('\n');
        }
        ZeroMemory(client->receiveBuffer, client->NETWORK_BUFFER_SIZE);
        client->Socket().async_receive(asio::buffer(client->receiveBuffer, client->NETWORK_BUFFER_SIZE), 
                              boost::bind(&Server::handle_read, shared_from_this(), client, asio::placeholders::error));
    }
    else
    {
        remove_client(client);
    }
}
*/

/*
void Server::remove_client(Client_ptr client)
{
	if(!client)
		return;

	if(client->Socket().is_open())
    {
        try{
            LogFile::Log("network", "Server::remove_client; Closing connection to " + client->Socket().remote_endpoint().address().to_string());
            client->Socket().shutdown(asio::ip::tcp::socket::shutdown_both);
	        client->Socket().close();
        }catch(std::exception & e)
        {
            LogFile::Log("error", e.what());

        }
    }

    std::list<Client_ptr>::iterator iter = clients.begin();
    while(clients.size() > 0 && iter != clients.end())
    {
        if((*iter) && (*iter) == client)
        {
            (*iter).reset();
            //iter = clients.erase(iter);
            break;
        }
        else
        {
            ++iter;
        }
    }

    //cout << clients.size() << endl;
}
*/

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



/*
void Server::handle_write(Client_ptr client, const asio::error_code & error)
{
    if(!error)
    {
        //asio::async_write(client->Socket(),
        //                  asio::buffer(msg.c_str(), msg.length()),
        //                  boost::bind(&Client::handle_write, shared_from_this(), asio::placeholders::error));

		//See if client is flagged to be disconnected (quit command)
		if(client->disconnect)
		{
			remove_client(client);
		}
    }
    else
    {
        remove_client(client);
    }
}
*/

void Server::Stop()
{
    //io_service.poll();
    DisconnectAllClients();
    //io_service.stop();
}
