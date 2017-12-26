#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CLogFile.h"
#include "CmySQLQueue.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
#include "CClient.h"
typedef boost::shared_ptr<Client> Client_ptr;
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
#include "mud.h"

using namespace std;

void cmd_inventory(Character * ch, string argument)
{
    if(ch == NULL || ch->player == NULL)
        return;

    int total = 0;
    std::list<Item*>::iterator iter;
    for(iter = ch->player->inventory.begin(); iter != ch->player->inventory.end(); ++iter)
    {
        total++;
        ch->Send(Item::quality_strings[(*iter)->quality] + (*iter)->name + "|X\n\r");
    }
    if(total == 0)
        ch->Send("Your inventory is empty.\n\r");

    if(total != ch->player->inventorySize)
    {
        LogFile::Log("error", "cmd_inventory, total != user->character->inventorySize");
    }
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

    string arg1;
    Utilities::one_argument(argument, arg1);

    if(arg1.empty())
    {
        ch->Send("Wear what?\n\r");
        return;
    }

    if(!Utilities::str_cmp(arg1, "all"))
    {
        std::list<Item *>::iterator iter = ch->player->inventory.begin();
        
        while(iter != ch->player->inventory.end())
        {
            std::list<Item *>::iterator thisiter = iter;
            iter++;
            int equiploc = ch->player->GetEquipLocation(*thisiter);
            if(equiploc != Player::EQUIP_LAST)
            {
                Item * wear = (*thisiter);

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

    if(ch->hasQuery)
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

	ch->queryData = new string(arg1);
	ch->hasQuery = true;
	ch->queryPrompt = "Destroy " + item->name + "? (y/n) ";
	ch->queryFunction = cmd_drop_Query;

}

bool cmd_drop_Query(Character * ch, string argument)
{
    if(!ch || !ch->player)
    {
        if(ch->queryData)
            delete ch->queryData;
        ch->QueryClear();
        return true;
    }
    if(ch->delay_active)
    {
        ch->Send("You can't do that while casting.\n\r");
        if(ch->queryData)
            delete ch->queryData;
        ch->QueryClear();
        return true;
    }

    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
    {
        string itemname = *((string*)ch->queryData);
        if(ch->queryData)
            delete ch->queryData;
        ch->QueryClear();
        Item * item;
        if((item = ch->player->GetItemInventory(itemname)) == NULL)
        {
            ch->Send("You're not carrying that item.\n\r");
            return true;
        }

        ch->player->RemoveItemInventory(itemname);
        return true;
    }
    if(!Utilities::str_cmp(argument, "no") || !Utilities::str_cmp(argument, "n"))
    {
        if(ch->queryData)
            delete ch->queryData;
        ch->QueryClear();
        return true;
    }
    return false;
}