#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CLogFile.h"
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
#include "mud.h"

using namespace std;

void cmd_inventory(Character * ch, string argument)
{
    if(ch == NULL || ch->player == NULL)
        return;

	ch->Send("You are carrying:\n\r");

	int total = 0;
	for (auto i = ch->player->inventory.begin(); i != ch->player->inventory.end(); i++)
	{
		if (i->second > 1)
			ch->Send("|M(" + Utilities::itos(i->second) + ") ");
		else
			ch->Send("    ");
		ch->Send(Item::quality_strings[i->first->quality] + i->first->name + "|X\n\r");
		total++;
	}

    if(total == 0)
        ch->Send("     Nothing.\n\r");

    ch->Send(Utilities::itos(total) + "/" + Utilities::itos(ch->player->maxInventorySize) + " items\n\r");
}

void cmd_equipment(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

    stringstream equipment;
    
    equipment << left << setw(15) << "<|BHead|X>";
    ch->player->equipped[Player::EQUIP_HEAD] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_HEAD]->quality] << ch->player->equipped[Player::EQUIP_HEAD]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BNeck|X>";
    ch->player->equipped[Player::EQUIP_NECK] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_NECK]->quality] << ch->player->equipped[Player::EQUIP_NECK]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BShoulder|X>";
    ch->player->equipped[Player::EQUIP_SHOULDER] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_SHOULDER]->quality] << ch->player->equipped[Player::EQUIP_SHOULDER]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BBack|X>";
    ch->player->equipped[Player::EQUIP_BACK] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_BACK]->quality] << ch->player->equipped[Player::EQUIP_BACK]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BChest|X>";
    ch->player->equipped[Player::EQUIP_CHEST] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_CHEST]->quality] << ch->player->equipped[Player::EQUIP_CHEST]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BWrist|X>";
    ch->player->equipped[Player::EQUIP_WRIST] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_WRIST]->quality] << ch->player->equipped[Player::EQUIP_WRIST]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BHands|X>";
    ch->player->equipped[Player::EQUIP_HANDS] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_HANDS]->quality] << ch->player->equipped[Player::EQUIP_HANDS]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BWaist|X>";
    ch->player->equipped[Player::EQUIP_WAIST] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_WAIST]->quality] << ch->player->equipped[Player::EQUIP_WAIST]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BLegs|X>";
    ch->player->equipped[Player::EQUIP_LEGS] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_LEGS]->quality] << ch->player->equipped[Player::EQUIP_LEGS]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BFeet|X>";
    ch->player->equipped[Player::EQUIP_FEET] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_FEET]->quality] << ch->player->equipped[Player::EQUIP_FEET]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BFinger|X>";
    ch->player->equipped[Player::EQUIP_FINGER1] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_FINGER1]->quality] << ch->player->equipped[Player::EQUIP_FINGER1]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BFinger|X>";
    ch->player->equipped[Player::EQUIP_FINGER2] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_FINGER2]->quality] << ch->player->equipped[Player::EQUIP_FINGER2]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BTrinket|X>";
    ch->player->equipped[Player::EQUIP_TRINKET1] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_TRINKET1]->quality] << ch->player->equipped[Player::EQUIP_TRINKET1]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BTrinket|X>";
    ch->player->equipped[Player::EQUIP_TRINKET2] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_TRINKET2]->quality] << ch->player->equipped[Player::EQUIP_TRINKET2]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BOffhand|X>";
    ch->player->equipped[Player::EQUIP_OFFHAND] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_OFFHAND]->quality] << ch->player->equipped[Player::EQUIP_OFFHAND]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << setw(15) << "<|BMainhand|X>";
    ch->player->equipped[Player::EQUIP_MAINHAND] ? 
        equipment << Item::quality_strings[ch->player->equipped[Player::EQUIP_MAINHAND]->quality] << ch->player->equipped[Player::EQUIP_MAINHAND]->name << "|X\n\r"
        : equipment << "None\n\r";
    equipment << "\n\r";

    ch->Send(equipment.str());
}

void cmd_remove(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

	if (ch->InCombat())
	{
		ch->Send("You can't do that while in combat!\n\r");
		return;

	}
    string arg1;
    Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("Remove what?\n\r");
        return;
    }

    if(!Utilities::str_cmp(arg1, "all"))
    {
        for(int i = 0; i < Player::EQUIP_LAST; i++)
        {
            if(ch->player->equipped[i] != NULL)
            {
                Item * remove = ch->player->RemoveItemEquipped(i);
                if(!remove)
                {
                    LogFile::Log("error", "cmd_remove (all), removed a NULL item");
                }
                ch->player->AddItemInventory(remove);
                ch->Send("You remove " + remove->name + ".\n\r");
            }
        }
        return;
    }

    int slot = ch->player->GetEquippedItemIndex(arg1);
    if(slot == Player::EQUIP_LAST)
    {
        ch->Send("You're not wearing that item.\n\r");
        return;
    }

    if(ch->player->inventorySize >= ch->player->maxInventorySize
        && !ch->player->IMMORTAL())
    {
        ch->Send("Your inventory is full.\n\r");
        return;
    }
    Item * remove = ch->player->RemoveItemEquipped(slot);
    if(!remove)
    {
        LogFile::Log("error", "cmd_remove, removed a NULL item");
    }
    ch->player->AddItemInventory(remove);
    ch->Send("You remove " + remove->name + ".\n\r");
}   

void cmd_wear(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

	if (ch->InCombat())
	{
		ch->Send("You can't do that while in combat!\n\r");
		return;
	}

    string arg1;
    Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("Wear what?\n\r");
        return;
    }

    if(!Utilities::str_cmp(arg1, "all"))
    {
        auto iter = ch->player->inventory.begin();
        
        while(iter != ch->player->inventory.end())
        {
            auto thisiter = iter;
            iter++;
            int equiploc = ch->player->GetEquipLocation(thisiter->first);
            if(equiploc != Player::EQUIP_LAST)
            {
                Item * wear = thisiter->first;

                //don't equip anything that requires something be removed
                if(ch->player->equipped[equiploc] != NULL)
                {
                    continue;
                }
                if(equiploc == Player::EQUIP_MAINHAND && wear->equipLocation == Item::EQUIP_TWOHAND && ch->player->equipped[Player::EQUIP_OFFHAND] != NULL)
                {
                    continue;
                }
                if(equiploc == Player::EQUIP_OFFHAND && ch->player->equipped[Player::EQUIP_MAINHAND] != NULL && ch->player->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_TWOHAND)
                {
                    continue;
                }
                ch->player->EquipItemFromInventory(wear, equiploc);
                ch->Send("You wear " + wear->name + ".\n\r");
            }
        }
        return;
    }

    Item * wear = ch->player->GetItemInventory(arg1);

    if(!wear)
    {
        ch->Send("You're not carrying that item.\n\r");
        return;
    }
    int equiploc = ch->player->GetEquipLocation(wear);
    if(equiploc == Player::EQUIP_LAST)
    {
        ch->Send("You can't equip that item.\n\r");
        return;
    }

    Item * removed = ch->player->RemoveItemEquipped(equiploc);
    if(removed != NULL) //remove anything already in this slot
    {
        ch->player->AddItemInventory(removed);
        ch->Send("You remove " + removed->name + ".\n\r");
    }
    if(equiploc == Player::EQUIP_MAINHAND && wear->equipLocation == Item::EQUIP_TWOHAND) //remove the offhand when equipping a two hand
    {
        Item * offhand = ch->player->RemoveItemEquipped(Player::EQUIP_OFFHAND);
        if(offhand != NULL)
        {
            ch->player->AddItemInventory(offhand);
            ch->Send("You remove " + offhand->name + ".\n\r");
        }
    }
    else if(equiploc == Player::EQUIP_OFFHAND) //remove a twohand when equipping an offhand
    {
        if(ch->player->equipped[Player::EQUIP_MAINHAND] != NULL && ch->player->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_TWOHAND)
        {
            Item * mh = ch->player->RemoveItemEquipped(Player::EQUIP_MAINHAND);
            ch->player->AddItemInventory(mh);
            ch->Send("You remove " + mh->name + ".\n\r");
        }
    }
    ch->player->EquipItemFromInventory(wear, equiploc);
    ch->Send("You wear " + wear->name + ".\n\r");
}

void cmd_drop(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

    if(ch->HasQuery())
    {
        ch->Send("Answer your current question first.\n\r");
        return;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\n\r");
        return;
    }
    if(argument.empty())
    {
        ch->Send("Drop what?\n\r");
        return;
    }

    string arg1;
    //string arg2;
    argument = Utilities::one_argument(argument, arg1);
    /*argument = Utilities::one_argument(argument, arg2);
    string itemname;

    bool all = false;
    int count = 1;
    if(!Utilities::str_cmp(arg1, "all"))
    {
        count = user->character->player->maxInventorySize;
        itemname = arg2;
    }
    else if(!Utilities::IsNumber(arg1))
    {
        count = Utilities::atoi(arg1);
        if(count <= 0)
        {
            ch->Send("You must drop one or more items.\n\r");
            return;
        }
        itemname = arg2;
    }
    else
    {
        itemname = arg1;
    }

    if(itemname.empty())
    {
        ch->Send("Drop what?\n\r");
        return;
    }*/

    Item * item;
    if((item = ch->player->GetItemInventory(arg1)) == NULL)
    {
        ch->Send("You're not carrying that item.\n\r");
        return;
    }
	ch->SetQuery("Destroy " + item->name + "? (y/n) ", item, cmd_drop_Query);
}

bool cmd_drop_Query(Character * ch, string argument)
{
    if(!ch || !ch->player)
    {
        ch->QueryClear();
        return true;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\n\r");
        ch->QueryClear();
        return true;
    }

    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
    {
        Item * item = (Item*)ch->GetQueryData(); //the query data is the Item Index just in case it got deleted in the meantime
        ch->QueryClear();
        if((item = ch->player->GetItemInventory(item->id)) == nullptr)
        {
            ch->Send("You're not carrying that item.\n\r");
            return true;
        }
        ch->player->RemoveItemInventory(item->id);
        return true;
    }
    ch->QueryClear();
    return true;
}

void cmd_take(Character * ch, string argument)
{
	if (!ch || !ch->player || !ch->room)
		return;

	if (ch->delay_active)
	{
		ch->Send("Another action is in progress!\n\r");
		return;
	}
	if (argument.empty())
	{
		ch->Send("Take what?\n\r");
		return;
	}

	string arg1;
	argument = Utilities::one_argument(argument, arg1);

	Item * i = ch->GetItemRoom(arg1);

	if (i == nullptr)
	{
		ch->Send("You don't see that here.\n\r");
		return;
	}

	json casttime = { { "time", 2.5 } };
	ch->SendGMCP("char.casttime " + casttime.dump());

	ch->Send("You begin taking " + i->name + "...\n\r");

	ch->delay = (Game::GetGame()->currentTime + 2.5);
	Character::DelayData dd;
	dd.caster = ch;
	dd.itemTarget = i;
	dd.charTarget = nullptr;
	dd.itemTarget->AddSubscriber(dd.caster); //if the item is gone when delay finishes, we need to know about it
	dd.sk = nullptr;
	ch->delayData = dd;
	ch->delay_active = true;
	ch->delayFunction = cmd_takeCallback;
}

void cmd_takeCallback(Character::DelayData delayData)
{
	if (!delayData.caster)
	{
		LogFile::Log("error", "cmd_takeCallback: NULL caster");
		return;
	}
	delayData.caster->delay_active = false;

	json casttime = { { "time", 0 } };
	delayData.caster->SendGMCP("char.casttime " + casttime.dump());

	if (delayData.itemTarget == NULL) //target will never be null from cmd_take, only from Subscriber::Notify 
	{
		delayData.caster->Send("That item is no longer here.\n\r");
		return;
	}

	delayData.itemTarget->RemoveSubscriber(delayData.caster);
	delayData.itemTarget->NotifySubscribers();

	if (!delayData.caster->IsItemInRoom(delayData.itemTarget))
	{
		delayData.caster->Send("That item is no longer here.\n\r");
		return;
	}

	delayData.caster->room->RemoveItem(delayData.itemTarget);
	if(delayData.caster->player)
		delayData.caster->player->AddItemInventory(delayData.itemTarget);
	delayData.caster->Send("You take " + delayData.itemTarget->name + "\n\r");
	delayData.caster->Message(delayData.caster->name + " takes " + delayData.itemTarget->name, Character::MSG_ROOM_NOTCHAR);
}

void cmd_loot(Character * ch, string argument)
{
	if (ch->IsNPC())
		return;
	
	string arg1;
	string arg2;
	string arg3;
	argument = Utilities::one_argument(argument, arg1);
	argument = Utilities::one_argument(argument, arg2);
	argument = Utilities::one_argument(argument, arg3);

	if (arg1.empty())
	{
		if (ch->pending_loot_rolls.empty() && !ch->GetTarget())
		{
			ch->Send("Target a corpse with this command to see available items to loot.\n\r");
			ch->Send("loot take||get <loot id>||all\n\r");
			ch->Send("loot need||greed||pass <loot id>\n\r");
			ch->Send("loot info roll||target <loot id>\n\r");
			return;
		}

		if(!ch->pending_loot_rolls.empty())
			ch->Send("Pending loot rolls:\n\r");

		auto iter = std::begin(ch->pending_loot_rolls);
		while (iter != std::end(ch->pending_loot_rolls))
		{
			Character::OneLoot * loot = iter->corpse->GetCorpseLoot(iter->corpse_id);
			if (loot == nullptr)
			{
				LogFile::Log("error", "Pending loot roll couldn't find corresponding loot on corpse");
				iter = ch->pending_loot_rolls.erase(iter);
				continue;
			}
			ch->Send(Utilities::itos(iter->my_id) + ". " + (string)Item::quality_strings[loot->item->quality] + loot->item->name + "|X");
			if (loot->roll_timer > 0 && loot->roll_timer > Game::currentTime)
				ch->Send(" |Y[" + Utilities::dtos(loot->roll_timer - Game::currentTime, 1) + "s remaining]|X");
			ch->Send("\n\r");
			++iter;
		}

		if (ch->GetTarget() != nullptr && ch->GetTarget()->IsNPC() && !ch->GetTarget()->IsAlive())
		{
			Character * loot_target = ch->GetTarget();
			bool lootable_items = false;
			ch->Send("You can loot the following items from " + loot_target->GetName() + ":\n\r");
			for (auto iter = std::begin(loot_target->loot); iter != std::end(loot_target->loot); ++iter)
			{
				/*std::find_if(iter->looters.begin(), iter->looters.end(),
							[&cch = ch] (const struct Character::Looter & lc) -> bool { return cch == lc.ch; }); //lamba function solution instead of operator== in the struct*/
				auto can_loot = std::find(iter->looters.begin(), iter->looters.end(), ch);
				if (can_loot != std::end(iter->looters))
				{
					lootable_items = true;
					ch->Send(Utilities::itos(iter->id) + ". " + (string)Item::quality_strings[iter->item->quality] + iter->item->name + "|X");
					if (iter->roll_timer > 0 && iter->roll_timer > Game::currentTime)
						ch->Send(" |Y[" + Utilities::dtos(iter->roll_timer - Game::currentTime, 1) + "s remaining]|X");
					ch->Send("\n\r");
				}
			}
			if (lootable_items == 0)
				ch->Send("None\n\r");
		}
		return;
	}
	else if (!Utilities::str_cmp(arg1, "take") || !Utilities::str_cmp(arg1, "get"))
	{
		if (ch->GetTarget() == nullptr || !ch->GetTarget()->IsNPC() || ch->GetTarget()->IsAlive())
		{
			ch->Send("You must target a corpse with loot to take it.\n\r");
			return;
		}
		if (!Utilities::IsNumber(arg2) && Utilities::str_cmp(arg2, "all"))
		{
			ch->Send("loot take||get <loot id>||all\n\r");
			return;
		}
		if (!Utilities::str_cmp(arg2, "all"))
		{
			Character * loot_target = ch->GetTarget();
			for (auto iter = std::begin(loot_target->loot); iter != std::end(loot_target->loot);)
			{
				Character::OneLoot * oneloot = &(*iter);
				++iter;
				auto can_loot = std::find(oneloot->looters.begin(), oneloot->looters.end(), ch);
				if (oneloot->roll_timer == 0 && can_loot != std::end(oneloot->looters))
				{
					Item * theitem = oneloot->item;
					if (ch->player->AddItemInventory(theitem))
					{
						//send to other looters
						for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
						{
							if (looter_iter->ch != ch)
							{
								looter_iter->ch->Send(ch->GetName() + " receives loot: " + (string)Item::quality_strings[oneloot->item->quality] + oneloot->item->name + "|X\n\r");
							}
						}
						ch->Send("You receive loot: " + (string)Item::quality_strings[theitem->quality] + theitem->name + "|X\n\r");
						loot_target->RemoveLoot(oneloot);
					}
					else
					{
						ch->Send("Your inventory is full.\n\r");
						return;
					}
				}
			}
			return;
		}
		if (Utilities::IsNumber(arg2))
		{
			return;
		}
		ch->Send("loot take||get <loot id>||all\n\r");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "info"))
	{
		if (!Utilities::IsNumber(arg3) || Utilities::str_cmp(arg2, "roll") || Utilities::str_cmp(arg2, "target"))
		{
			ch->Send("loot info roll||target <loot id>\n\r");
			return;
		}
		int lootnum = Utilities::atoi(arg3);

		if (!Utilities::str_cmp(arg2, "roll"))
		{
			for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
			{
				if (iter->my_id == lootnum)
				{
					ch->Send(iter->corpse->GetCorpseLoot(iter->corpse_id)->item->FormatItemInfo()); //this is fun pointer times
					return;
				}
			}
			ch->Send("You do not have a pending loot roll with that number.\n\r");
			return;
		}
		if (!Utilities::str_cmp(arg2, "target"))
		{
			if (!ch->GetTarget())
			{
				ch->Send("You do not have a target.\n\r");
				return;
			}
			Character::OneLoot * oneloot = ch->GetTarget()->GetCorpseLoot(lootnum);
			if (oneloot == nullptr)
			{
				ch->Send("Could not find loot item " + Utilities::itos(lootnum) + " on your target.\n\r");
				return;
			}
			ch->Send(oneloot->item->FormatItemInfo());
			return;
		}
		ch->Send("loot info roll||target <loot id>\n\r");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "need"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot need <loot id>\n\r");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				ch->SetRollType(iter->corpse, iter->corpse_id, Character::Looter::ROLL_NEED);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\n\r");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "greed"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot greed <loot id>\n\r");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				ch->SetRollType(iter->corpse, iter->corpse_id, Character::Looter::ROLL_GREED);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\n\r");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "pass"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot pass <loot id>\n\r");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				ch->SetRollType(iter->corpse, iter->corpse_id, Character::Looter::ROLL_PASS);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\n\r");
		return;
	}
	ch->Send("Target a corpse with this command to see available items to loot.\n\r");
	ch->Send("loot take||get <loot id>||all\n\r");
	ch->Send("loot need||greed||pass <loot id>\n\r");
	ch->Send("loot info roll||target <loot id>\n\r");
}

void cmd_drink(Character * ch, string argument)
{

}

void cmd_eat(Character * ch, string argument)
{
	if (!ch || !ch->player)
		return;

	if (ch->delay_active)
	{
		ch->Send("Another action is in progress!\n\r");
		return;
	}

	string arg1;
	Utilities::one_argument(argument, arg1);

	if (arg1.empty())
	{
		ch->Send("Eat what?\n\r");
		return;
	}

	Item * eat = ch->player->GetItemInventory(arg1);

	if (!eat)
	{
		ch->Send("You're not carrying that item.\n\r");
		return;
	}

	if (eat->type != Item::TYPE_FOOD && eat->type != Item::TYPE_CONSUMABLE)
	{
		ch->Send("That's not edible.\n\r");
		return;
	}
	
	if (eat->type == Item::TYPE_FOOD)
	{
		if (ch->InCombat())
		{
			ch->Send("You can't do that while in combat.\n\r");
			return;
		}
		Skill * sk = Game::GetGame()->GetSkill(eat->useSkillID);
		if (sk == nullptr)
		{
			LogFile::Log("error", "Item \"" + eat->name + "\": cmd_eat bad skillid");
			return;
		}


		ch->Sit();
		ch->player->RemoveItemInventory(eat);
		ch->Send("You start eating " + eat->name + ".\n\r");

		string func = sk->function_name + "_cast";
		try
		{
			sol::function lua_cast_func = Server::lua[func.c_str()];
			lua_cast_func(ch, ch, sk);
		}
		catch (const sol::error& e)
		{
			LogFile::Log("error", e.what());
		}
		catch (const std::exception& e)
		{
			LogFile::Log("error", e.what());
		}
		catch (...)
		{
			LogFile::Log("error", "lua unhandled exception cmd_eat _cast");
		}
	}
}
