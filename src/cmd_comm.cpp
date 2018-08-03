#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CClient.h"
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

using namespace std;

void cmd_shout(Character * ch, string argument)
{
    if(argument.empty())
    {
      	ch->Send("Shout what?\n\r");
      	return;
    }

    int shoutCount = 0;
	std::list<User *>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); iter++)
    {
        if((*iter)->connectedState == User::CONN_PLAYING && (*iter)->character && (*iter)->character != ch)
		{
		    (*iter)->Send("|M" + ch->name + " shouts '" + argument + "|X|M'|X\n\r");
			shoutCount++;
		}
    }
    ch->Send("|M[" + Utilities::itos(shoutCount) + "] people heard you shout: " + argument + "|X\n\r");
}

void cmd_say(Character * ch, string argument)
{
    if(!ch)
        return;

    if(argument.empty())
    {
		ch->Send("Say what?\n\r");
		return;
    }
	if (argument == "color_test")
	{
		string twofiftysix = "THIS STRING IS 256 CHARACTERS xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		string colored;
		for (int i = 0; i < 256; i++)
		{
			colored += "\033[38;5;" + Utilities::itos(i) + "m" + twofiftysix[i];
		}
		ch->Send(colored + "\n\r");
	}
    ch->Message("|W" + ch->name + " says, '" + argument + "|W'|X", Character::MSG_ROOM_NOTCHAR);
    ch->Send("|WYou say, '" + argument + "|X|W'|X\n\r");
}

void cmd_tell(Character * ch, string argument)
{
    ch->Send("cmd_tell\n\r");
}

void cmd_reply(Character * ch, string argument)
{
    ch->Send("cmd_reply\n\r");
}

void cmd_yell(Character * ch, string argument)
{
    ch->Send("cmd_yell\n\r");
}

void cmd_set(Character * ch, string argument)
{
	if (!ch || !ch->player)
		return;

	string arg;
	Utilities::one_argument(argument, arg);

	if (!Utilities::str_cmp(arg, "prompt"))
	{
		ch->player->prompt = !ch->player->prompt;
		return;
	}

	ch->Send("\"set prompt\"\n\r");
	return;
}