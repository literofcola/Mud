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
typedef boost::shared_ptr<Server> Server_ptr;
#include "utils.h"

using namespace std;
using asio::ip::tcp;

Client::Client(asio::io_service& io_service) : socket_(io_service), MAX_INPUT_LENGTH(2046)
{
    receiveBuffer = new char[MAX_INPUT_LENGTH];
    commandQueue.clear();
    inputBuffer.clear();
	disconnect = false;
    ZeroMemory(receiveBuffer, MAX_INPUT_LENGTH);
}

Client::~Client()
{
    delete[] receiveBuffer;
}


asio::ip::tcp::socket & Client::Socket()
{
    return socket_;
}
