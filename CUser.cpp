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

User::User(Client_ptr client_) : MAX_INPUT_LENGTH(2046)
{
    client = client_;
    commandQueue.clear();
    outputQueue.clear();
	subchannelQueue.clear();
	character = NULL;
    connectedState = CONN_GET_NAME;
    wasInput = false;
    stringEdit = NULL;
    mxp = false;
	mccp = false;
	gmcp = false;
}

User::~User()
{
	if(character != NULL)
		delete character;
}

void User::Send(string str)
{
    if(str.empty() || !IsConnected())
        return;

    for(int i = 0; i < (int)str.length()-1; i++)
    {
        if(str[i] == '|')
        {
            string colorcode = Utilities::ColorString(str[i+1]);
            str.replace(i, 2, colorcode);
        }
    }
	outputQueue.push_back(str);
}

void User::Send(char * str)
{
	Send(string(str));
}

void User::SendSubchannel(string str)
{
    if(str.empty() || !IsConnected())
        return;

	//string send = "\xFF\xFA" + Game::TELOPT_GMCP + str + "\xFF\xF0";
	//IAC SB TELOPT_GMCP str IAC SE
	string send = "\xFF\xFA\x69" + str + "\xFF\xF0";
	subchannelQueue.push_back(send);
}

void User::SendSubchannel(char * str)
{
	SendSubchannel(string(str));
}

bool User::IsConnected()
{
    try{
        return (client && client->Socket().is_open());
    }catch(std::exception & e)
    {
        LogFile::Log("error", e.what());
    }
    return false;
}

bool User::IsPlaying()
{
    return connectedState == CONN_PLAYING;
}
