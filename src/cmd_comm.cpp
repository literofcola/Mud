#include "stdafx.h"
#include "mud.h"
#include "CPlayer.h"
#include "CUser.h"
#include "CCharacter.h"
#include "CGame.h"
#include "utils.h"

void cmd_shout(Player * ch, std::string argument)
{
    if(argument.empty())
    {
      	ch->Send("Shout what?\r\n");
      	return;
    }

    int shoutCount = 0;
	std::list<User *>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); iter++)
    {
        if((*iter)->connectedState == User::CONN_PLAYING && (*iter)->character && (*iter)->character != ch)
		{
		    (*iter)->Send("|M" + ch->GetName() + " shouts '" + argument + "|X|M'|X\r\n");
			shoutCount++;
		}
    }
    ch->Send("|M[" + Utilities::itos(shoutCount) + "] people heard you shout: " + argument + "|X\r\n");
}

void cmd_say(Player * ch, std::string argument)
{
    if(!ch)
        return;

    if(argument.empty())
    {
		ch->Send("Say what?\r\n");
		return;
    }
	if (argument == "color_test")
	{
		std::string twofiftysix = "THIS STRING IS 256 CHARACTERS xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
		std::string colored;
		for (int i = 0; i < 256; i++)
		{
			colored += "\033[38;5;" + Utilities::itos(i) + "m" + twofiftysix[i];
		}
		ch->Send(colored + "\r\n");
	}
    ch->Message("|W" + ch->GetName() + " says, '" + argument + "|W'|X", Character::MSG_ROOM_NOTCHAR);
    ch->Send("|WYou say, '" + argument + "|X|W'|X\r\n");
}

void cmd_tell(Player * ch, std::string argument)
{
    ch->Send("cmd_tell\r\n");
}

void cmd_reply(Player * ch, std::string argument)
{
    ch->Send("cmd_reply\r\n");
}

void cmd_yell(Player * ch, std::string argument)
{
    ch->Send("cmd_yell\r\n");
}

void cmd_set(Player * ch, std::string argument)
{
	std::string arg;
	Utilities::one_argument(argument, arg);

	if (!Utilities::str_cmp(arg, "prompt"))
	{
		ch->prompt = !ch->prompt;
		return;
	}

	ch->Send("\"set prompt\"\r\n");
	return;
}