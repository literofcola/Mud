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
		LogFile::Log("error", "Error occurred while accepting socket: " + WSAGetLastError());
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
	Client * pClientContext = new Client(Socket);

	//Store this object
	mygame->NewUser(pClientContext);

	if (true == AssociateWithIOCP(pClientContext))
	{
		//pClientContext->SetOpCode(OP_WRITE);
		boost::shared_ptr<OVERLAPPEDEX> operationData = pClientContext->NewOperationData(OP_READ);
		OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(operationData.get());

		//Get data.
		DWORD dwFlags = 0;
		DWORD dwBytes = 0;

		//Post initial Recv
		//This is a right place to post a initial Recv
		//Posting a initial Recv in WorkerThread will create scalability issues.
		int nBytesRecv = WSARecv(pClientContext->Socket(), &operationData->wsabuf, 1, &dwBytes, &dwFlags, base_overlapped, NULL);

		if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
		{
			LogFile::Log("error", "Error in Initial Post.");
		}
	}
}

bool Server::AssociateWithIOCP(Client * pClientContext)
{
	//Associate the socket with IOCP
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pClientContext->Socket(), hIOCompletionPort, (ULONG_PTR)pClientContext, 0);

	if (NULL == hTemp)
	{
		LogFile::Log("error", "Error occurred while executing CreateIoCompletionPort(). " + Utilities::itos(GetLastError()));

		//Let's not work with this client
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
			LogFile::Log("error", "GetQueuedCompletionStatus() failed: " + Utilities::itos(GetLastError()));

		if ((bReturn == FALSE) )// || ((bReturn != FALSE) && (0 == dwBytesTransfered)))
		{
			//Client connection gone, remove it.
			thisserver->mygame->RemoveUser(pClientContext);
			continue;
		}

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
					nBytesSent = WSASend(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, &dwBytes, dwFlags, pOverlapped, NULL);

					if ((SOCKET_ERROR == nBytesSent) && (WSA_IO_PENDING != WSAGetLastError()))
					{
						//Let's not work with this client
						thisserver->mygame->RemoveUser(pClientContext);
					}
				}
				else
				{
					//remove the per operation data from the send
					//OVERLAPPEDEXPtr freeme(pOverlappedEx);
					pClientContext->FreeOperationData(pOverlappedEx);
					/*
					//Once the data is successfully received, we will print it.
					pOverlappedEx->opCode = (OP_READ);
					//pClientContext->SetOpCode(OP_READ);
					//pClientContext->ResetWSABUF();
					ZeroMemory(pOverlappedEx->buffer, MAX_INPUT_LENGTH);
					ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
					pOverlappedEx->totalBytes = pOverlappedEx->sentBytes = 0;

					dwFlags = 0;

					//Get the data.
					nBytesRecv = WSARecv(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, &dwBytes, &dwFlags, pOverlapped, NULL);

					if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != WSAGetLastError()))
					{
						LogFile::Log("error", "Thread ?: Error occurred while executing WSARecv().");
						LogFile::Log("error", Utilities::itos(GetLastError()));
						//Let's not work with this client
						thisserver->mygame->RemoveUser(pClientContext);
					}*/
				}

				break;

			case OP_READ:

				char localBuffer[MAX_INPUT_LENGTH];

				//pClientContext->GetBuffer(localBuffer);
				strcpy_s(localBuffer, MAX_INPUT_LENGTH, pOverlappedEx->buffer);

				LogFile::Log("network", "Thread ?: The following message was received: " + std::string(localBuffer));

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
						EnterCriticalSection(&thisserver->critical_section);
						pClientContext->commandQueue.push_back(pClientContext->inputBuffer.substr(0, cr_pos));
						LeaveCriticalSection(&thisserver->critical_section);
						if(nl_pos != std::string::npos)
							pClientContext->inputBuffer.erase(0, nl_pos+1);
						else
							pClientContext->inputBuffer.erase(0, cr_pos+1);
					}
					else if(nl_pos < cr_pos) //...is it ever done any other way?
					{
						EnterCriticalSection(&thisserver->critical_section);
						pClientContext->commandQueue.push_back(pClientContext->inputBuffer.substr(0, nl_pos));
						LeaveCriticalSection(&thisserver->critical_section);
						if(cr_pos != std::string::npos)
							pClientContext->inputBuffer.erase(0, cr_pos+1);
						else
							pClientContext->inputBuffer.erase(0, nl_pos+1);
					}
					cr_pos = pClientContext->inputBuffer.find('\r');
					nl_pos = pClientContext->inputBuffer.find('\n');
				}
				ZeroMemory(pClientContext->receiveBuffer, MAX_INPUT_LENGTH);
				/*pClientContext->Socket().async_receive(asio::buffer(client->receiveBuffer, client->MAX_INPUT_LENGTH), 
								  boost::bind(&Server::handle_read, shared_from_this(), client, asio::placeholders::error));*/

				/*pClientContext->SetOpCode(OP_READ);
				WSABUF *p_wbuf = pClientContext->GetWSABUFPtr();
				OVERLAPPED *p_ol = pClientContext->GetOVERLAPPEDPtr();
				pClientContext->ResetWSABUF();*/

				pOverlappedEx->opCode = (OP_READ);
				ZeroMemory(pOverlappedEx->buffer, MAX_INPUT_LENGTH);
				ZeroMemory(pOverlapped, sizeof(OVERLAPPED));
				pOverlappedEx->totalBytes = pOverlappedEx->sentBytes = 0;

				nBytesRecv = WSARecv(pClientContext->Socket(), &pOverlappedEx->wsabuf, 1, &dwBytes, &dwFlags, pOverlapped, NULL);

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
	/*
	asio::async_write(c->Socket(), 
				  asio::buffer(msg.c_str(), msg.length()),
				  boost::bind(&Server::handle_write, shared_from_this(), c, asio::placeholders::error));
				  */
	OVERLAPPEDEXPtr olptr = c->NewOperationData(OP_WRITE);
	strcpy(olptr->wsabuf.buf, msg.c_str());
	olptr->wsabuf.len = (DWORD)msg.length();
	olptr->totalBytes = (DWORD)msg.length();
	OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(olptr.get());

	/*WSABUF *p_wbuf = c->GetWSABUFPtr();
	OVERLAPPED *p_ol = c->GetOVERLAPPEDPtr();
	p_wbuf->buf = (CHAR *)msg.c_str();
	p_wbuf->len = (DWORD)msg.length();*/

	/*c->SetTotalBytes((DWORD)msg.length());
	c->SetSentBytes(0);*/

	//Overlapped send
	WSASend(c->Socket(), &olptr->wsabuf, 1, NULL, 0, base_overlapped, NULL);
}

void Server::deliver(Client * c, const unsigned char * msg, int length)
{
	/*
	asio::async_write(c->Socket(), 
				  asio::buffer(msg, length),
				  boost::bind(&Server::handle_write, shared_from_this(), c, asio::placeholders::error));
				  */
	OVERLAPPEDEXPtr olptr = c->NewOperationData(OP_WRITE);
	strcpy(olptr->wsabuf.buf, (const char *)msg);
	olptr->wsabuf.len = length;
	olptr->totalBytes = length;
	OVERLAPPED * base_overlapped = static_cast<OVERLAPPED*>(olptr.get());

	/*WSABUF *p_wbuf = c->GetWSABUFPtr();
	OVERLAPPED *p_ol = c->GetOVERLAPPEDPtr();
	p_wbuf->buf = (CHAR *)msg.c_str();
	p_wbuf->len = (DWORD)msg.length();*/

	/*c->SetTotalBytes((DWORD)msg.length());
	c->SetSentBytes(0);*/

	//Overlapped send
	WSASend(c->Socket(), &olptr->wsabuf, 1, NULL, 0, base_overlapped, NULL);
}

/*
void Server::handle_accept(Client_ptr client, const asio::error_code& error)
{
    if(!error)
    {
        LogFile::Log("network", "Server::handle_accept; Accepting connection from " + client->Socket().remote_endpoint().address().to_string());

        //Disable Nagle
        asio::ip::tcp::no_delay option(true);
        client->Socket().set_option(option);

        Game::GetGame()->NewUser(client);
        clients.push_back(client);
        start_client(client);
        Client_ptr newclient(new Client(io_service));
        acceptor.async_accept(newclient->Socket(), boost::bind(&Server::handle_accept, this, newclient, asio::placeholders::error));
    }
}
*/
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
        ZeroMemory(client->receiveBuffer, client->MAX_INPUT_LENGTH);
        client->Socket().async_receive(asio::buffer(client->receiveBuffer, client->MAX_INPUT_LENGTH), 
                              boost::bind(&Server::handle_read, shared_from_this(), client, asio::placeholders::error));
    }
    else
    {
        remove_client(client);
    }
}
*/
/*
void Server::start_client(Client_ptr c)
{
    c->Socket().async_receive(asio::buffer(c->receiveBuffer, c->MAX_INPUT_LENGTH), 
                          boost::bind(&Server::handle_read, shared_from_this(), c, asio::placeholders::error));
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
/*
void Server::remove_all_clients()
{
	LogFile::Log("network", "Server::remove_all_clients");
	
	std::list<Client_ptr>::iterator iter;
	for(iter = clients.begin(); iter != clients.end(); iter++)
	{
		if(!(*iter))
			continue;
        if((*iter)->Socket().is_open())
        {
            try{
		    (*iter)->Socket().shutdown(asio::ip::tcp::socket::shutdown_both);
		    (*iter)->Socket().close();
            }catch(std::exception & e)
            {
                LogFile::Log("error", e.what());
            }
        }
	}
	clients.clear();
}
*/


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
    /*io_service.poll();
    remove_all_clients();
    io_service.stop();*/
}

/*
unsigned int __stdcall Server::Run(void * lpParam)
{
    Server * s = (Server*)lpParam;
    try{
        s->io_service.run();
    }catch(std::exception & e)
    {
        LogFile::Log("error", e.what());
    }

    cout << "io_service.run() finished" << endl;

    return 0;
}
*/
/*
int Server::Start()
{
    _beginthreadex(NULL, 0, Server::Run, this, 0, NULL);

    return 0;
}
*/
