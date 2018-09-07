#include "CNPC.h"
#include "CCharacter.h"
#include "CLogFile.h"
#include "CPlayer.h"
#include "CItem.h"
#include "utils.h"
#include "CServer.h"
#include <string>

using std::string;

NPC::NPC(NPCIndex * index_) : Character()
{
	npcindex = index_;

	reset = nullptr;
	leashOrigin = nullptr;
}

NPC::~NPC()
{

}

int NPC::GetIndexID()
{
	if (!npcindex)
	{
		LogFile::Log("error", "NPC has invalid pointer to NPCIndex!! (this is impossible and very bad)");
		return 0;
	}
	return npcindex->id;
}

std::string NPC::GetName()
{
	return npcindex->name;
}

int NPC::GetLevel()
{
	return npcindex->level;
}

bool NPC::FlagIsSet(const int flag)
{
	return npcindex->FlagIsSet(flag);
}

bool NPC::IsAlive()
{
	if (!IsCorpse())
		return true;
	return false;
}

void NPC::DoLootRoll(OneLoot * oneloot)
{
	int which_roll = NPC::Looter::ROLL_PASS;
	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		if (looter_iter->roll_type == NPC::Looter::ROLL_UNDECIDED)
		{
			looter_iter->roll_type = NPC::Looter::ROLL_PASS;
			looter_iter->ch->RemoveLootRoll(this, oneloot->id);

			//tell everyone else we passed
			for (auto looter_iter2 = oneloot->looters.begin(); looter_iter2 != oneloot->looters.end(); ++looter_iter2)
			{
				if (looter_iter2->ch != looter_iter->ch)
				{
					looter_iter2->ch->Send(looter_iter->ch->GetName() + " has passed on " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
				}
				else if (looter_iter2->ch == looter_iter->ch)
				{
					looter_iter2->ch->Send("You passed on " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
				}
			}
		}
		if (looter_iter->roll_type == NPC::Looter::ROLL_NEED)
		{
			which_roll = NPC::Looter::ROLL_NEED;
		}
		else if (which_roll != NPC::Looter::ROLL_NEED && looter_iter->roll_type == NPC::Looter::ROLL_GREED)
		{
			which_roll = NPC::Looter::ROLL_GREED;
		}
	}
	string roll_msg;
	switch (which_roll)
	{
	case NPC::Looter::ROLL_NEED:
		roll_msg = "Need Roll";
		break;
	case NPC::Looter::ROLL_GREED:
		roll_msg = "Greed Roll";
		break;
	case NPC::Looter::ROLL_PASS:
		oneloot->roll_timer = 0;
		for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
		{
			looter_iter->ch->Send("Everyone passed on " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
		}
		return;
	}
	int highest_roll = 0;
	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		if (looter_iter->roll_type == which_roll)
		{
			looter_iter->final_roll = Server::rand() % 100;
			if (looter_iter->final_roll > highest_roll)
			{
				highest_roll = looter_iter->final_roll;
			}
			//tell everyone about the roll
			for (auto looter_iter2 = oneloot->looters.begin(); looter_iter2 != oneloot->looters.end(); ++looter_iter2)
			{
				looter_iter2->ch->Send(roll_msg + " - " + Utilities::itos(looter_iter->final_roll) + " for " +
					(string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X by " + looter_iter->ch->GetName() + "\n\r");
			}
		}
	}
	//Check ties
	std::list<std::pair<Player *, int>> highest_rollers;
	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		if (looter_iter->final_roll == highest_roll)
		{
			highest_rollers.push_back(std::make_pair(looter_iter->ch, 0));
		}
	}
	while (highest_rollers.size() > 1)
	{
		highest_roll = 0;
		for (auto tieiter = begin(highest_rollers); tieiter != end(highest_rollers); ++tieiter)
		{
			tieiter->second = Server::rand() % 100;
			if (tieiter->second > highest_roll)
			{
				highest_roll = tieiter->second;
			}
		}
		for (auto tieiter = begin(highest_rollers); tieiter != end(highest_rollers);)
		{
			if (tieiter->second != highest_roll)
				tieiter = highest_rollers.erase(tieiter);
			else
				++tieiter;
		}
	}

	Player * winner = highest_rollers.front().first;
	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		if (looter_iter->ch != winner)
		{
			looter_iter->ch->Send(winner->GetName() + " receives loot: " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
		}
	}
	winner->Send("You receive loot: " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
	if (!winner->AddItemInventory(oneloot->item)) //if additeminventory fails, leave it in the corpse for cmd_loot
	{
		winner->Send("Your inventory is full.\n\r");
		oneloot->roll_timer = 0;
	}
	else
	{
		this->RemoveLoot(oneloot);
	}
}

NPC::OneLoot * NPC::GetCorpseLoot(int corpse_id)
{
	for (auto iter = begin(loot); iter != end(loot); ++iter)
	{
		if (iter->id == corpse_id)
			return &(*iter);
	}
	return nullptr;
}

void NPC::RemoveLoot(OneLoot * removeme)
{
	for (auto iter = begin(loot); iter != end(loot); ++iter)
	{
		if (&(*iter) == removeme)
		{
			for (auto iter2 = iter->looters.begin(); iter2 != iter->looters.end();)
			{
				iter2->ch->RemoveSubscriber(this);
				iter2 = iter->looters.erase(iter2);
			}
			iter->looters.clear();
			loot.erase(iter);
			return;
		}
	}
}

void NPC::SetRollType(Player * who, int corpse_id, NPC::Looter::RollType type)
{
	OneLoot * oneloot = nullptr;
	for (auto iter = loot.begin(); iter != loot.end(); ++iter)
	{
		if (iter->id == corpse_id)
		{
			oneloot = &(*iter);
			break;
		}
	}
	if (oneloot == nullptr)
	{
		LogFile::Log("error", "NPC::SetRollType: could not find loot with corpse_id on this NPC");
		return;
	}
	std::string roll_msg;
	switch (type)
	{
	case Looter::ROLL_NEED:
		roll_msg = " selected Need for: ";
		break;
	case Looter::ROLL_GREED:
		roll_msg = " selected Greed for: ";
		break;
	case Looter::ROLL_PASS:
		roll_msg = " passed on: ";
		break;
	}
	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		if (looter_iter->ch == who)
		{
			looter_iter->roll_type = type;
			looter_iter->ch->RemoveLootRoll(this, oneloot->id);

			//tell everyone else our choice
			for (auto looter_iter2 = oneloot->looters.begin(); looter_iter2 != oneloot->looters.end(); ++looter_iter2)
			{
				if (looter_iter2->ch != looter_iter->ch)
				{
					looter_iter2->ch->Send(looter_iter->ch->GetName() + " has" + roll_msg + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
				}
				else if (looter_iter2->ch == looter_iter->ch)
				{
					looter_iter2->ch->Send("You have" + roll_msg + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
				}
			}
		}
	}

	for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
	{
		//check the status of everyone's choice to see if it's time to roll
		if (looter_iter->roll_type == Looter::ROLL_UNDECIDED)
			return;
	}
	this->DoLootRoll(oneloot);
}

void NPC::RemoveLooter(Character * ch)
{
	for (auto iter = loot.begin(); iter != loot.end(); ++iter)
	{
		for (auto iter2 = iter->looters.begin(); iter2 != iter->looters.end();)
		{
			if (iter2->ch == ch)
			{
				iter2 = iter->looters.erase(iter2);
				ch->RemoveSubscriber(this);
			}
			else
			{
				++iter2;
			}
		}
	}
}

void NPC::RemoveAllLooters() //for subscriber/subscribermanager considerations
{
	for (auto iter = loot.begin(); iter != loot.end(); ++iter)
	{
		for (auto iter2 = iter->looters.begin(); iter2 != iter->looters.end();)
		{
			iter2->ch->RemoveSubscriber(this);
			iter2 = iter->looters.erase(iter2);
		}
		iter->looters.clear();
	}
}

void NPC::AddSkill(Skill * newskill)
{
	npcindex->AddSkill(newskill);
}

void NPC::RemoveSkill(Skill * sk)
{
	npcindex->RemoveSkill(sk);
}

void NPC::RemoveSkill(string name)
{
	npcindex->RemoveSkill(name);
}

Skill * NPC::GetSkillShortName(string name)
{
	return npcindex->GetSkillShortName(name);
}

bool NPC::HasSkill(Skill * sk)
{
	return npcindex->HasSkill(sk);
}

bool NPC::HasSkillByName(string name) //Not guaranteed to be the same skill id, just the same name
{
	return npcindex->HasSkillByName(name);
}