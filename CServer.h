#ifndef CSERVER_H
#define CSERVER_H

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

class Server : public boost::enable_shared_from_this<Server>
{
public:
    Server(asio::io_service& io_service_, const asio::ip::tcp::endpoint& endpoint_);
    ~Server();

    int Start();
    void Stop();
    static unsigned int __stdcall Run(void * lpParam);

    void handle_accept(Client_ptr client, const asio::error_code& error);
    void deliver(Client_ptr client, const std::string msg);
	void deliver(Client_ptr c, const unsigned char * msg, int length);

    void start_client(Client_ptr c);
	void remove_client(Client_ptr client);
	void remove_all_clients();
    void handle_read(Client_ptr client, const asio::error_code & error);
    void handle_write(Client_ptr client, const asio::error_code& error);

    void GameLoop(Server_ptr server);
    void ShutdownGame();

	std::list<Client_ptr> clients;

	//static CRITICAL_SECTION critical_section;
    CRITICAL_SECTION critical_section;

    static mySQLQueue * sqlQueue;
    static lua_State * luaState;

    //Game * game;

private:
    asio::io_service &io_service;
    asio::ip::tcp::acceptor acceptor;

    
};

#endif //CSERVER_H