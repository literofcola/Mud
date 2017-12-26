#ifndef CUSER_H
#define CUSER_H

class User
{
public:
    User(Client_ptr client);
    ~User();

    enum State
    {
        CONN_PLAYING, CONN_GET_NAME, CONN_GET_OLD_PASSWORD, CONN_GET_NEW_PASSWORD, CONN_CONFIRM_NEW_PASSWORD, CONN_GET_SEX,
        CONN_GET_RACE, CONN_CONFIRM_RACE, CONN_GET_CLASS, CONN_CONFIRM_CLASS, CONN_CHANGEPW1, CONN_CHANGEPW2, CONN_CHANGEPW3, CONN_DELETE1, CONN_DELETE2, CONN_MENU
    };

    void Send(std::string str);
	void Send(char * str);
	void SendSubchannel(std::string str);
	void SendSubchannel(char * str);
    bool IsConnected();
    bool IsPlaying();

    const int MAX_INPUT_LENGTH;
    std::deque<std::string> commandQueue;
    std::deque<std::string> outputQueue;
	std::deque<std::string> subchannelQueue;
	Client_ptr client;
    State connectedState;
    Character * character;
    bool wasInput;
    
    std::string * stringEdit;
    bool mxp;		//Mud extension protocol
	bool mccp;      //Mud client compression protocol
	bool gmcp;

	//ZLIB
	static const int Z_BUFSIZE = 32768;
	int z_ret;
    z_stream z_strm;
    unsigned char z_in[Z_BUFSIZE];
    unsigned char z_out[Z_BUFSIZE];
	

    //int loginAttempts; //TODO

    //bool hasQuery;
	//bool (*queryFunction)(Server_ptr, User *, std::string);
	//void * queryData;
    //std::string queryPrompt;

private:
	
};

#endif