#ifndef CCLIENT_H
#define CCLIENT_H

class Client : public boost::enable_shared_from_this<Client>
{
public:
    Client(asio::io_service& io_service);
    ~Client();

    asio::ip::tcp::socket & Socket();

    const int MAX_INPUT_LENGTH;

    char * receiveBuffer;       //Buffer for async_receive, which SHOULD work with std::string, but...
    std::string inputBuffer;    //Dump the receive buffer here to parse for commands
    std::deque<std::string> commandQueue;
	bool disconnect;

private:
    asio::ip::tcp::socket socket_;
	
};

#endif