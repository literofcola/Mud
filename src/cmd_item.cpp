#include "stdafx.h"
#include "mud.h"
#include "CCharacter.h"
#include "CServer.h"
#include "CPlayer.h"
#include "CRoom.h"
#include "utils.h"
#include "CGame.h"
#include "CNPC.h"
#include "CSkill.h"
#include "CSpellAffect.h"
#include "CItem.h"
#include "CLogFile.h"
#include "json.hpp"
// for convenience
using json = nlohmann::json;
extern "C"
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}
#define SOL_CHECK_ARGUMENTS
#define SOL_PRINT_ERRORS
#include <sol.hpp>
#include <sstream>
#include <iomanip>

using std::string;

void cmd_inventory(Player * ch, string argument)
{
	ch->Send("You are carrying:\r\n");

	int total = 0;
	for (auto i = ch->inventory.begin(); i != ch->inventory.end(); i++)
	{
		if (i->second > 1)
			ch->Send("|M(" + Utilities::itos(i->second) + ") ");
		else
			ch->Send("    ");
		ch->Send(i->first->GetColoredName() + "|X\r\n");
		total++;
	}

    if(total == 0)
        ch->Send("     Nothing.\r\n");

    ch->Send(Utilities::itos(total) + "/" + Utilities::itos(ch->maxInventorySize) + " items\r\n");
}

void cmd_equipment(Player * ch, string argument)
{
    std::stringstream equipment = ch->FormatEquipment();
    ch->Send(equipment.str());
}

void cmd_remove(Player * ch, string argument)
{
	if (ch->InCombat())
	{
		ch->Send("You can't do that while in combat!\r\n");
		return;

	}
    string arg1;
    Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("Remove what?\r\n");
        return;
    }

    if(!Utilities::str_cmp(arg1, "all"))
    {
        for(int i = 0; i < Player::EQUIP_LAST; i++)
        {
            if(ch->equipped[i] != nullptr)
            {
                Item * remove = ch->RemoveItemEquipped(i);
                if(!remove)
                {
                    LogFile::Log("error", "cmd_remove (all), removed a nullptr item");
                }
				ch->RemoveEquipmentStats(remove);
                ch->AddItemInventory(remove);
                ch->Send("You remove " + remove->GetColoredName() + "|X\r\n");
				ch->Message(ch->GetName() + " removes " + remove->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
            }
        }
        return;
    }

    int slot = ch->GetEquippedItemIndex(arg1);
    if(slot == Player::EQUIP_LAST)
    {
        ch->Send("You're not wearing that item.\r\n");
        return;
    }

    if(ch->inventorySize >= ch->maxInventorySize
        && !ch->IsImmortal())
    {
        ch->Send("Your inventory is full.\r\n");
        return;
    }
    Item * remove = ch->RemoveItemEquipped(slot);
    if(!remove)
    {
        LogFile::Log("error", "cmd_remove, removed a nullptr item");
    }
	ch->RemoveEquipmentStats(remove);
    ch->AddItemInventory(remove);
    ch->Send("You remove " + remove->GetColoredName() + "|X\r\n");
	ch->Message(ch->GetName() + " removes " + remove->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
}   

void cmd_wear(Player * ch, string argument)
{
	if (ch->InCombat())
	{
		ch->Send("You can't do that while in combat!\r\n");
		return;
	}

    string arg1;
    Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("Wear what?\r\n");
        return;
    }

    if(!Utilities::str_cmp(arg1, "all"))
    {
        auto iter = ch->inventory.begin();
        
        while(iter != ch->inventory.end())
        {
            auto thisiter = iter;
            iter++;
            int equiploc = ch->GetEquipLocation(thisiter->first);
            if(equiploc != Player::EQUIP_LAST)
            {
                Item * wear = thisiter->first;

                //don't equip anything that requires something be removed
                if(ch->equipped[equiploc] != nullptr)
                {
                    continue;
                }
                if(equiploc == Player::EQUIP_MAINHAND && wear->equipLocation == Item::EQUIP_TWOHAND && ch->equipped[Player::EQUIP_OFFHAND] != nullptr)
                {
                    continue;
                }
                if(equiploc == Player::EQUIP_OFFHAND && ch->equipped[Player::EQUIP_MAINHAND] != nullptr && ch->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_TWOHAND)
                {
                    continue;
                }
				if (!ch->CanWearArmor(wear->type))
				{
					continue;
				}
                ch->EquipItemFromInventory(wear);
				ch->AddEquipmentStats(wear);
                ch->Send("You wear " + wear->GetColoredName() + "|X\r\n");
				ch->Message(ch->GetName() + " wears " + wear->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
            }
        }
        return;
    }

    Item * wear = ch->GetItemInventory(arg1);

    if(!wear)
    {
        ch->Send("You're not carrying that item.\r\n");
        return;
    }
    int equiploc = ch->GetEquipLocation(wear);
    if(equiploc == Player::EQUIP_LAST)
    {
        ch->Send("You can't equip that item.\r\n");
        return;
    }

	if ((wear->type == Item::TYPE_ARMOR_CLOTH || wear->type == Item::TYPE_ARMOR_LEATHER
		|| wear->type == Item::TYPE_ARMOR_MAIL || wear->type == Item::TYPE_ARMOR_PLATE) && !ch->CanWearArmor(wear->type))
	{
		ch->Send("You can't equip that item.\r\n");
		return;
	}

    Item * removed = ch->RemoveItemEquipped(equiploc);
    if(removed != nullptr) //remove anything already in this slot
    {
		ch->RemoveEquipmentStats(removed);
        ch->AddItemInventory(removed);
        ch->Send("You remove " + removed->GetColoredName() + "|X\r\n");
		ch->Message(ch->GetName() + " removes " + removed->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
    }
    if(equiploc == Player::EQUIP_MAINHAND && wear->equipLocation == Item::EQUIP_TWOHAND) //remove the offhand when equipping a two hand
    {
        Item * offhand = ch->RemoveItemEquipped(Player::EQUIP_OFFHAND);
        if(offhand != nullptr)
        {
			ch->RemoveEquipmentStats(offhand);
            ch->AddItemInventory(offhand);
            ch->Send("You remove " + offhand->GetColoredName() + "|X\r\n");
			ch->Message(ch->GetName() + " removes " + offhand->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
        }
    }
    else if(equiploc == Player::EQUIP_OFFHAND) //remove a twohand when equipping an offhand
    {
        if(ch->equipped[Player::EQUIP_MAINHAND] != nullptr && ch->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_TWOHAND)
        {
            Item * mh = ch->RemoveItemEquipped(Player::EQUIP_MAINHAND);
			ch->RemoveEquipmentStats(mh);
            ch->AddItemInventory(mh);
            ch->Send("You remove " + mh->GetColoredName() + "|X\r\n");
			ch->Message(ch->GetName() + " removes " + mh->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
        }
    }
    ch->EquipItemFromInventory(wear);
	ch->AddEquipmentStats(wear);
    ch->Send("You wear " + wear->GetColoredName() + "|X\r\n");
	ch->Message(ch->GetName() + " wears " + wear->GetColoredName() + "|X", Character::MSG_ROOM_NOTCHAR);
}

void cmd_drop(Player * ch, string argument)
{
    if(ch->HasQuery())
    {
        ch->Send("Answer your current question first.\r\n");
        return;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\r\n");
        return;
    }
    if(argument.empty())
    {
        ch->Send("Drop what?\r\n");
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
            ch->Send("You must drop one or more items.\r\n");
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
        ch->Send("Drop what?\r\n");
        return;
    }*/

    Item * item;
    if((item = ch->GetItemInventory(arg1)) == nullptr)
    {
        ch->Send("You're not carrying that item.\r\n");
        return;
    }
	ch->SetQuery("Destroy " + item->GetColoredName() + "|X? (y/n) ", item, cmd_drop_Query);
}

bool cmd_drop_Query(Player * ch, string argument)
{
    if(!ch)
    {
        ch->QueryClear();
        return true;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\r\n");
        ch->QueryClear();
        return true;
    }

    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
    {
        Item * item = (Item*)ch->GetQueryData(); //the query data is the Item Index just in case it got deleted in the meantime
        ch->QueryClear();
        if((item = ch->GetItemInventory(item->GetID())) == nullptr)
        {
            ch->Send("You're not carrying that item.\r\n");
            return true;
        }
        ch->RemoveItemInventory(item->GetID());
        return true;
    }
    ch->QueryClear();
    return true;
}

void cmd_take(Player * ch, string argument)
{
	if (!ch || !ch->room)
		return;

	if (ch->delay_active)
	{
		ch->Send("Another action is in progress!\r\n");
		return;
	}
	if (argument.empty())
	{
		ch->Send("Take what?\r\n");
		return;
	}

	string arg1;
	argument = Utilities::one_argument(argument, arg1);

	Item * i = ch->GetItemRoom(arg1);

	if (i == nullptr)
	{
		ch->Send("You don't see that here.\r\n");
		return;
	}
	if (Utilities::FlagIsSet(i->flags, Item::FLAG_ROOMONLY))
	{
		ch->Send("You can't take that.\r\n");
		return;
	}
	if (i->quest && !ch->ShouldDropQuestItem(i))
	{
		ch->Send("You can't take that.\r\n");
		return;
	}

	json casttime = { { "time", 2.5 } };
	ch->SendGMCP("char.casttime " + casttime.dump());

	ch->Send("You begin taking " + i->GetName() + "...\r\n");

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
		LogFile::Log("error", "cmd_takeCallback: nullptr caster");
		return;
	}
	delayData.caster->delay_active = false;

	json casttime = { { "time", 0 } };
	delayData.caster->SendGMCP("char.casttime " + casttime.dump());

	if (delayData.itemTarget == nullptr) //target will never be null from cmd_take, only from Subscriber::Notify 
	{
		delayData.caster->Send("That item is no longer here.\r\n");
		return;
	}

	delayData.itemTarget->RemoveSubscriber(delayData.caster);
	delayData.itemTarget->NotifySubscribers();

	if (!delayData.caster->IsItemInRoom(delayData.itemTarget))
	{
		delayData.caster->Send("That item is no longer here.\r\n");
		return;
	}

	delayData.caster->room->RemoveItem(delayData.itemTarget);
	if (delayData.caster->IsPlayer())
	{
		((Player*)(delayData.caster))->AddItemInventory(delayData.itemTarget);
	}
	delayData.caster->Send("You take " + delayData.itemTarget->GetName() + "\r\n");
	delayData.caster->Message(delayData.caster->GetName() + " takes " + delayData.itemTarget->GetName(), Character::MSG_ROOM_NOTCHAR);
}

void cmd_loot(Player * ch, string argument)
{
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
			ch->Send("Target a corpse with this command to see available items to loot.\r\n");
			ch->Send("loot take||get <loot id>||all\r\n");
			ch->Send("loot need||greed||pass <loot id>\r\n");
			ch->Send("loot info roll||target <loot id>\r\n");
			return;
		}

		if(!ch->pending_loot_rolls.empty())
			ch->Send("Pending loot rolls:\r\n");

		auto iter = std::begin(ch->pending_loot_rolls);
		while (iter != std::end(ch->pending_loot_rolls))
		{
			NPC::OneLoot * loot = iter->corpse->GetCorpseLoot(iter->corpse_id);
			if (loot == nullptr)
			{
				LogFile::Log("error", "Pending loot roll couldn't find corresponding loot on corpse");
				iter = ch->pending_loot_rolls.erase(iter);
				continue;
			}
			ch->Send(Utilities::itos(iter->my_id) + ". " + loot->item->GetColoredName() + "|X");
			if (loot->roll_timer > 0 && loot->roll_timer > Game::currentTime)
				ch->Send(" |Y[" + Utilities::dtos(loot->roll_timer - Game::currentTime, 1) + "s remaining]|X");
			ch->Send("\r\n");
			++iter;
		}

		if (ch->GetTarget() != nullptr && ch->GetTarget()->IsNPC() && !ch->GetTarget()->IsAlive())
		{
			NPC * loot_target = (NPC*)ch->GetTarget();
			bool lootable_items = false;
			ch->Send("You can loot the following items from " + loot_target->GetName() + ":\r\n");
			for (auto iter = std::begin(loot_target->loot); iter != std::end(loot_target->loot); ++iter)
			{
				/*std::find_if(iter->looters.begin(), iter->looters.end(),
							[&cch = ch] (const struct Character::Looter & lc) -> bool { return cch == lc.ch; }); //lamba function solution instead of operator== in the struct*/
				auto can_loot = std::find(iter->looters.begin(), iter->looters.end(), ch);
				if (can_loot != std::end(iter->looters))
				{
					lootable_items = true;
					ch->Send(Utilities::itos(iter->id) + ". " + iter->item->GetColoredName() + "|X");
					if (iter->roll_timer > 0 && iter->roll_timer > Game::currentTime)
						ch->Send(" |Y[" + Utilities::dtos(iter->roll_timer - Game::currentTime, 1) + "s remaining]|X");
					ch->Send("\r\n");
				}
			}
			if (lootable_items == 0)
				ch->Send("None\r\n");
		}
		return;
	}
	else if (!Utilities::str_cmp(arg1, "take") || !Utilities::str_cmp(arg1, "get"))
	{
		if (ch->GetTarget() == nullptr || !ch->GetTarget()->IsNPC() || ch->GetTarget()->IsAlive())
		{
			ch->Send("You must target a corpse with loot to take it.\r\n");
			return;
		}
		if (!Utilities::IsNumber(arg2) && Utilities::str_cmp(arg2, "all"))
		{
			ch->Send("loot take||get <loot id>||all\r\n");
			return;
		}
		if (!Utilities::str_cmp(arg2, "all"))
		{
			NPC * loot_target = (NPC*)ch->GetTarget();
			for (auto iter = std::begin(loot_target->loot); iter != std::end(loot_target->loot);)
			{
				NPC::OneLoot * oneloot = &(*iter);
				++iter;
				auto can_loot = std::find(oneloot->looters.begin(), oneloot->looters.end(), ch);
				if (oneloot->roll_timer == 0 && can_loot != std::end(oneloot->looters))
				{
					Item * theitem = oneloot->item;
					if (ch->AddItemInventory(theitem))
					{
						//send to other looters
						for (auto looter_iter = oneloot->looters.begin(); looter_iter != oneloot->looters.end(); ++looter_iter)
						{
							if (looter_iter->ch != ch)
							{
								looter_iter->ch->Send(ch->GetName() + " receives loot: " + oneloot->item->GetColoredName() + "|X\r\n");
							}
						}
						ch->Send("You receive loot: " + theitem->GetColoredName() + "|X\r\n");
						loot_target->RemoveLoot(oneloot);
					}
					else
					{
						ch->Send("Your inventory is full.\r\n");
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
		ch->Send("loot take||get <loot id>||all\r\n");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "info"))
	{
		if (!Utilities::IsNumber(arg3) || Utilities::str_cmp(arg2, "roll") || Utilities::str_cmp(arg2, "target"))
		{
			ch->Send("loot info roll||target <loot id>\r\n");
			return;
		}
		int lootnum = Utilities::atoi(arg3);

		if (!Utilities::str_cmp(arg2, "roll"))
		{
			for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
			{
				if (iter->my_id == lootnum)
				{
					ch->Send(iter->corpse->GetCorpseLoot(iter->corpse_id)->item->FormatItemInfo(ch));
					return;
				}
			}
			ch->Send("You do not have a pending loot roll with that number.\r\n");
			return;
		}
		if (!Utilities::str_cmp(arg2, "target"))
		{
			if (!ch->GetTarget() || !ch->GetTarget()->IsNPC())
			{
				ch->Send("You do not have a lootable target.\r\n");
				return;
			}
			NPC::OneLoot * oneloot = ((NPC*)(ch->GetTarget()))->GetCorpseLoot(lootnum);
			if (oneloot == nullptr)
			{
				ch->Send("Could not find loot item " + Utilities::itos(lootnum) + " on your target.\r\n");
				return;
			}
			ch->Send(oneloot->item->FormatItemInfo(ch));
			return;
		}
		ch->Send("loot info roll||target <loot id>\r\n");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "need"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot need <loot id>\r\n");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				iter->corpse->SetRollType(ch, iter->corpse_id, NPC::Looter::ROLL_NEED);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\r\n");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "greed"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot greed <loot id>\r\n");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				iter->corpse->SetRollType(ch, iter->corpse_id, NPC::Looter::ROLL_GREED);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\r\n");
		return;
	}
	else if (!Utilities::str_cmp(arg1, "pass"))
	{
		if (!Utilities::IsNumber(arg2))
		{
			ch->Send("loot pass <loot id>\r\n");
			return;
		}
		int rollnum = Utilities::atoi(arg2);

		for (auto iter = ch->pending_loot_rolls.begin(); iter != ch->pending_loot_rolls.end(); ++iter)
		{
			if (iter->my_id == rollnum)
			{
				iter->corpse->SetRollType(ch, iter->corpse_id, NPC::Looter::ROLL_PASS);
				return;
			}
		}
		ch->Send("You do not have a pending loot roll with that number.\r\n");
		return;
	}
	ch->Send("Target a corpse with this command to see available items to loot.\r\n");
	ch->Send("loot take||get <loot id>||all\r\n");
	ch->Send("loot need||greed||pass <loot id>\r\n");
	ch->Send("loot info roll||target <loot id>\r\n");
}

void cmd_drink(Player * ch, string argument)
{

}

void cmd_eat(Player * ch, string argument)
{
	if (ch->delay_active)
	{
		ch->Send("Another action is in progress!\r\n");
		return;
	}

	string arg1;
	Utilities::one_argument(argument, arg1);

	if (arg1.empty())
	{
		ch->Send("Eat what?\r\n");
		return;
	}

	Item * eat = ch->GetItemInventory(arg1);

	if (!eat)
	{
		ch->Send("You're not carrying that item.\r\n");
		return;
	}

	if (eat->type != Item::TYPE_FOOD && eat->type != Item::TYPE_CONSUMABLE)
	{
		ch->Send("That's not edible.\r\n");
		return;
	}
	
	if (eat->type == Item::TYPE_FOOD)
	{
		if (ch->InCombat())
		{
			ch->Send("You can't do that while in combat.\r\n");
			return;
		}
		Skill * sk = Game::GetGame()->GetSkill(eat->useSkillID);
		if (sk == nullptr)
		{
			LogFile::Log("error", "Item \"" + eat->GetName() + "\": cmd_eat bad skillid");
			return;
		}


		ch->Sit();
		ch->RemoveItemInventory(eat);
		ch->Send("You start eating " + eat->GetName() + ".\r\n");

        sk->CallLuaCast(ch, ch);
	}
}
