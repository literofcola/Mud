#include "stdafx.h"
#include "mud.h"
#include "CCharacter.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CRoom.h"
#include "utils.h"

using std::string;

void cmd_north(Player * ch, string argument)
{
    ch->Move(Exit::DIR_NORTH);
}

void cmd_east(Player * ch, string argument)
{
    ch->Move(Exit::DIR_EAST);
}

void cmd_south(Player * ch, string argument)
{
    ch->Move(Exit::DIR_SOUTH);
}

void cmd_west(Player * ch, string argument)
{
    ch->Move(Exit::DIR_WEST);
}

void cmd_northeast(Player * ch, string argument)
{
    ch->Move(Exit::DIR_NORTHEAST);
}

void cmd_southeast(Player * ch, string argument)
{
    ch->Move(Exit::DIR_SOUTHEAST);
}

void cmd_southwest(Player * ch, string argument)
{
    ch->Move(Exit::DIR_SOUTHWEST);
}

void cmd_northwest(Player * ch, string argument)
{
    ch->Move(Exit::DIR_NORTHWEST);
}

void cmd_up(Player * ch, string argument)
{
    ch->Move(Exit::DIR_UP);
}

void cmd_down(Player * ch, string argument)
{
    ch->Move(Exit::DIR_DOWN);
}

void cmd_sit(Player * ch, string argument)
{
	if (ch->InCombat())
	{
		ch->Send("You can't do that while in combat!\r\n");
		return;
	}
	if (ch->position == Player::Position::POSITION_SITTING)
	{
		ch->Send("You are already sitting.\r\n");
		return;
	}
	ch->Sit();
}

void cmd_stand(Player * ch, string argument)
{
	if (ch->position == Player::Position::POSITION_STANDING)
	{
		ch->Send("You are already standing.\r\n");
		return;
	}
	ch->Stand();
}

void cmd_sleep(Player * ch, string argument)
{
    ch->Send("cmd_sleep\r\n");
}

void cmd_wake(Player * ch, string argument)
{
    ch->Send("cmd_wake\r\n");
}

void cmd_recall(Player * ch, string argument)
{
	Room * current = Game::GetGame()->GetRoom(ch->recall);
	if (current != nullptr)
	{
		ch->Send("Your current recall room is: " + current->name + "\r\n");
	}
	if(ch->room == nullptr || !Utilities::FlagIsSet(ch->room->flags, Room::FLAG_RECALL))
	{
		ch->Send("You cannot set your recall location here.\r\n");
		return;
	}
	ch->SetQuery("Set recall location to: |W" + ch->room->name + "|X? (y/n) ", ch->room, cmd_recall_Query);
}

bool cmd_recall_Query(Player * ch, string argument)
{
	if (!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
	{
		Room * recall = (Room*)ch->GetQueryData();
		ch->recall = recall->id;
		ch->Send("Recall location set.\r\n");
	}
	ch->QueryClear();
	return true;
}