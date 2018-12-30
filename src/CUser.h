#ifndef CUSER_H
#define CUSER_H

class Client;
class Player;

class User
{
public:
    User(std::shared_ptr<Client> client);
    ~User();

    enum State
    {
        CONN_PLAYING, CONN_GET_NAME, CONN_GET_OLD_PASSWORD, CONN_GET_NEW_PASSWORD, CONN_CONFIRM_NEW_PASSWORD, CONN_GET_GENDER,
        CONN_GET_RACE, CONN_CONFIRM_RACE, CONN_GET_CLASS, CONN_CONFIRM_CLASS, CONN_CHANGEPW1, CONN_CHANGEPW2, CONN_CHANGEPW3, CONN_DELETE1, CONN_DELETE2, CONN_MENU
    };

    void Send(std::string str);
	void Send(char * str);
	void SendBW(std::string str);
	void SendGMCP(std::string str);
	void SendGMCP(char * str);
    bool IsConnected();
    bool IsPlaying();
	void ImmediateDisconnect();
	void SetDisconnect();
	void GetOneCommandFromNetwork();
	bool HasCommandReady();
	void ClearClientCommandQueue();
	Client * GetClient();

    std::deque<std::string> commandQueue;
    std::deque<std::string> outputQueue;
	std::deque<std::string> GMCPQueue;
	
    State connectedState;
    Player * character;
    bool wasInput;
	bool remove;
	double lastInput; //timestamp for idle disconnect
    
    std::string * stringEdit;
    bool mxp;		//Mud extension protocol
	bool mccp;      //Mud client compression protocol
	bool gmcp;		//Generic MUD Communication Protocol

	//ZLIB
	static const int Z_BUFSIZE = 32768;
	int z_ret;
    z_stream z_strm;
    unsigned char z_in[Z_BUFSIZE];
    unsigned char z_out[Z_BUFSIZE];
	
    int passwordAttempts; //disconnect after password fails

	static const int MAX_PASSWORD_TRIES = 3;

private:

	std::shared_ptr<Client> client;
};

#endif