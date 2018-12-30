#include "stdafx.h"
#include "CUser.h"
#include "CClient.h"
#include "CPlayer.h"
#include "CServer.h"
#include "CLogFile.h"
#include "utils.h"

User::User(std::shared_ptr<Client> client_)
{
    client = client_;
    commandQueue.clear();
    outputQueue.clear();
	GMCPQueue.clear();
	character = nullptr;
    connectedState = CONN_GET_NAME;
    wasInput = false;
    stringEdit = nullptr;
    mxp = false;
	mccp = false;
	gmcp = false;
	remove = false;
	passwordAttempts = 0;

	_timeb time;
	_ftime64_s(&time);
	lastInput = ((int)((time_t)time.time) + ((double)time.millitm / 1000.0));
}

User::~User()
{
	client = nullptr; //if we're deleting the user, we should have called ->Disconnect to close the socket and delete client already
	if(character != nullptr)
	{
		delete character;
		character = nullptr;
	}
}

void User::SendBW(std::string str)
{
	if (str.empty() || !IsConnected())
		return;

	if (str.length() >= NETWORK_BUFFER_SIZE)
	{
		str = str.substr(0, NETWORK_BUFFER_SIZE - 1);
		LogFile::Log("error", "User::Send(), tried to send single std::string > NETWORK_BUFFER_SIZE. Truncated");
	}
	outputQueue.push_back(str);
}

void User::Send(std::string str)
{
    if(str.empty() || !IsConnected())
        return;

    for(int i = 0; i < (int)str.length()-1; i++)
    {
        if(str[i] == '|')
        {
			if ((int)str.length() > (i + 3) && Utilities::IsNumber(str.substr(i + 1, 3)))
			{
				// \033[38; 2; <r>; <g>; <b>m nope!
				std::string colorcode = "\033[38;5;" + str.substr(i + 1, 3) + "m";
				str.replace(i, 4, colorcode);
			}
			else if ((int)str.length() >= (i + 1))
			{
				std::string colorcode = Utilities::ColorString(str[i + 1]);
				str.replace(i, 2, colorcode);
			}
        }
    }
    if(str.length() >= NETWORK_BUFFER_SIZE)
    {
        str = str.substr(0, NETWORK_BUFFER_SIZE-1);
        LogFile::Log("error", "User::Send(), tried to send single std::string > NETWORK_BUFFER_SIZE. Truncated");
    }
	outputQueue.push_back(str);
}

void User::Send(char * str)
{
	Send(std::string(str));
}

Client * User::GetClient()
{
	if(client != nullptr)
		return client.get();
	return nullptr;
}

void User::SendGMCP(std::string str)
{
    if(str.empty() || !IsConnected())
        return;

	//std::string send = "\xFF\xFA" + Game::TELOPT_GMCP + str + "\xFF\xF0";
	//IAC SB TELOPT_GMCP str IAC SE
	std::string send = Server::IAC + Server::SB + Server::TELOPT_GMCP + str + Server::IAC + Server::SE;
	//std::string send = "\xFF\xFA\xC9" + str + "\xFF\xF0";
	GMCPQueue.push_back(send);
}

void User::SendGMCP(char * str)
{
	SendGMCP(std::string(str));
}

bool User::IsConnected()
{
	if (client == nullptr)
		return false;
	if (!client->IsConnected())
	{
		return false;
	}
	return true;
}

bool User::IsPlaying()
{
    return connectedState == CONN_PLAYING;
}

void User::ImmediateDisconnect()
{
	if (client)
	{
		CancelIoEx((HANDLE)client->Socket(), nullptr);
		closesocket(client->Socket());
		client->DisconnectGame();
	}
    client = nullptr;
	remove = true;
}

void User::SetDisconnect()
{
	if (client)
	{
		client->DisconnectGame();
	}
	remove = true;
}

void User::GetOneCommandFromNetwork()
{
	if(!client)
		return;

	EnterCriticalSection(&client->command_cs);
	if(!client->commandQueue.empty())
	{
		commandQueue.push_back(client->commandQueue.front());
		client->commandQueue.pop_front();
	}
    LeaveCriticalSection(&client->command_cs);
}

bool User::HasCommandReady()
{
	if (commandQueue.empty())
	{
		return false;
	}
	return true;
}

void User::ClearClientCommandQueue()
{
	if (!client)
		return;
	EnterCriticalSection(&client->command_cs);
	client->commandQueue.clear();
    LeaveCriticalSection(&client->command_cs);
}