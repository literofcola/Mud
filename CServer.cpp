#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
typedef boost::shared_ptr<Client> Client_ptr;
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
//typedef boost::shared_ptr<Server> Server_ptr;
#include "utils.h"

using namespace std;
using asio::ip::tcp;

//CRITICAL_SECTION Server::critical_section;
lua_State * Server::luaState;
mySQLQueue * Server::sqlQueue;

Server::Server(asio::io_service& io_service_, const tcp::endpoint& endpoint_) : io_service(io_service_), acceptor(io_service_, endpoint_)
{
    Client_ptr client(new Client(io_service));
    acceptor.async_accept(client->Socket(), boost::bind(&Server::handle_accept, this, client, asio::placeholders::error));

    sqlQueue = new mySQLQueue();
	InitializeCriticalSection(&critical_section);
}

Server::~Server()
{
	DeleteCriticalSection(&critical_section);
    Game::DeleteGame();
    if(sqlQueue != NULL)
    {
        sqlQueue->Close();
        delete sqlQueue;
    }
}

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

void Server::start_client(Client_ptr c)
{
    c->Socket().async_receive(asio::buffer(c->receiveBuffer, c->MAX_INPUT_LENGTH), 
                          boost::bind(&Server::handle_read, shared_from_this(), c, asio::placeholders::error));
}

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

void Server::deliver(Client_ptr c, const string msg)
{
	asio::async_write(c->Socket(), 
				  asio::buffer(msg.c_str(), msg.length()),
				  boost::bind(&Server::handle_write, shared_from_this(), c, asio::placeholders::error));
}

void Server::deliver(Client_ptr c, const unsigned char * msg, int length)
{
	asio::async_write(c->Socket(), 
				  asio::buffer(msg, length),
				  boost::bind(&Server::handle_write, shared_from_this(), c, asio::placeholders::error));
}

void Server::handle_write(Client_ptr client, const asio::error_code & error)
{
    if(!error)
    {
        /*asio::async_write(client->Socket(),
                          asio::buffer(msg.c_str(), msg.length()),
                          boost::bind(&Client::handle_write, shared_from_this(), asio::placeholders::error));*/

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

void Server::Stop()
{
    io_service.poll();
    remove_all_clients();
    io_service.stop();
}

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

int Server::Start()
{
    _beginthreadex(NULL, 0, Server::Run, this, 0, NULL);

    return 0;
}

void Server::GameLoop(Server_ptr server)
{
    Game::GetGame()->GameLoop(server);
}

void Server::ShutdownGame()
{
    Game::GetGame()->shutdown = true;
}