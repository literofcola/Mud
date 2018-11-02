#include "stdafx.h"
#include "mud.h"
#include "CLogFile.h"
#include "CSkill.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CSpellAffect.h"
#include "CGame.h"
#include "CNPCIndex.h"
#include "CNPC.h"
#include "CServer.h"
#include "CSkill.h"
#include "utils.h"
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
#include <iostream>

using std::string;

void cmd_castCallback(Character::DelayData delayData)
{
    if(!delayData.caster)
    {
        LogFile::Log("error", "cmd_castCallback: nullptr caster");
        return;
    }
	delayData.caster->delay_active = false;

    if(!delayData.sk)
    {
        LogFile::Log("error", "cmd_castCallback: nullptr skill");
        return;
    }

	json casttime = { { "time", 0 } };
	delayData.caster->SendGMCP("char.casttime " + casttime.dump());

    if(delayData.charTarget && delayData.charTarget != delayData.caster)
    {
        delayData.charTarget->RemoveSubscriber(delayData.caster);
		//cout << "cmd_castCallback REMOVE" << endl;
    }

    if(delayData.charTarget == nullptr) //target will never be null from cmd_cast, only from Subscriber::Notify 
    {
        delayData.caster->Send("Your target is no longer here.\r\n");
        return;
    }
	
    if(delayData.charTarget != delayData.caster)
    {
		if (!delayData.caster->GetCharacterRoom(delayData.charTarget) 
			&& FindDirection(delayData.caster, delayData.charTarget, 3) == Exit::DIR_LAST)
		{
			delayData.caster->Send("Your target is no longer here.\r\n");
			return;
		}
    }
    
    if(delayData.charTarget == delayData.caster && (delayData.sk->targetType == Skill::TARGET_OTHER ||
        delayData.sk->targetType == Skill::TARGET_HOSTILE))
    {
        delayData.caster->Send("You must target someone else with this skill.\r\n");
        return;
    }
    /*if(!delayData.charTarget && (delayData.sk->targetType == Skill::TARGET_SELF || delayData.sk->targetType == Skill::TARGET_ANY
        || delayData.sk->targetType == Skill::TARGET_FRIENDLY))
    {
        delayData.charTarget = delayData.caster;
    }*/
    if(delayData.sk->targetType == Skill::TARGET_FRIENDLY && delayData.charTarget != delayData.caster 
        && delayData.caster->IsFighting(delayData.charTarget))
    {
        delayData.caster->Send("You can't cast this spell on that target.\r\n");
        return;
    }

    int lua_ret = delayData.sk->CallLuaCost(delayData.caster, delayData.charTarget);

    if(lua_ret == 0)
    {
        //not enough resources to cast
        return;
    }

    if(delayData.sk->function_name.empty())
    {
        LogFile::Log("error", "cmd_castCallback: no function_name defined");
        return;
    }

    delayData.sk->CallLuaCast(delayData.caster, delayData.charTarget);
	delayData.caster->SetCooldown(delayData.sk, -1);
}

void cmd_cast(Player * ch, string argument)
{
    if(!ch)
        return;

    if(ch->delay_active)
    {
        ch->Send("Another action is in progress!\r\n");
        return;
    }

    if(argument.empty())
    {
        ch->Send("Cast what spell?\r\n");
        return;
    }

    string arg1; //spell name
    string arg2; //target name
    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

    Skill * spell;
    //if(!ch->IsNPC())
    //{
        spell = ch->GetSkillShortName(arg1);
    //}
    //else
    //{
    //    spell = Game::GetGame()->GetSkill(Utilities::atoi(arg1)); //Get skill by ID for NPCs
    //}

    if(spell == nullptr)
    {
        ch->Send("You don't know that skill.\r\n");
        return;
    }

    double cd;
    if((cd = ch->GetCooldownRemaining(spell)) > 0)
    {
        ch->Send(spell->long_name + " will be ready in " + Utilities::dtos(cd, 1) + " seconds.\r\n");
        return;
    }
    
    Character * arg_target = nullptr;
    if(!arg2.empty())
    {
        arg_target = ch->GetCharacterRoom(arg2);
        if(arg_target == nullptr)
        {
            ch->Send("They aren't here.\r\n");
            return;
        }
    }

    if(arg_target == nullptr)
        arg_target = ch->GetTarget();
	if(arg_target == nullptr) //If no target or argument, target self
		arg_target = ch;

    if((spell->targetType == Skill::TARGET_OTHER || spell->targetType == Skill::TARGET_HOSTILE)
        && (!arg_target || arg_target == ch)) //Requires a target
    {
        ch->Send("You must target someone with this skill.\r\n");
        return;
    }


	if (spell->targetType == Skill::TARGET_HOSTILE
		&& arg_target->FlagIsSet(NPCIndex::FLAG_FRIENDLY)
		&& (!ch->IsImmortal()))
	{
		ch->Send("That target is friendly.\r\n");
		return;
	}
    

    if(spell->targetType == Skill::TARGET_FRIENDLY && arg_target && ch->IsFighting(arg_target))
    {
        ch->Send("You can't cast this spell on that target.\r\n");
        return;
    }

    int lua_ret = spell->CallLuaCost(ch, arg_target);
    
    if(lua_ret == 0)
    {
        //not enough resources to cast, or some other restriction
        return;
    }

	if (spell->castTime != 0)
	{
		ch->Message("|W" + ch->GetName() + " begins to cast " + spell->long_name + "...|X", Character::MSG_ROOM_NOTCHAR);
		ch->Send("|WYou begin to cast " + spell->long_name + "...|X\r\n");
		json casttime = { { "time", spell->castTime } };
		ch->SendGMCP("char.casttime " + casttime.dump());
	}
 
	if(!Utilities::FlagIsSet(spell->flags, Skill::FLAG_NOGCD))
		ch->StartGlobalCooldown();

    //TODO: call cmd_castCallback directly with no delay for instant skills ???
    ch->delay = (Game::GetGame()->currentTime + spell->castTime);
    Character::DelayData dd;
    dd.caster = ch;
    if(arg_target == nullptr)
        arg_target = ch;
    dd.charTarget = arg_target;
	if (arg_target && ch != arg_target)
	{
		dd.charTarget->AddSubscriber(dd.caster); //if our target is gone when spell finishes, we need to know about it
		//cout << "cmd_cast ADD:" << endl;
	}
    dd.sk = spell;
    ch->delayData = dd;
    ch->delay_active = true;
    ch->delayFunction = cmd_castCallback;
}

void cmd_skills(Player * ch, string argument)
{
    if(!ch)
        return;

    ch->Send("|MKnown spells/skills:|X\r\n");
    std::stringstream skill_string;
	std::map<string, Skill *>::iterator iter;
    for(iter = ch->knownSkills.begin(); iter != ch->knownSkills.end(); ++iter)
    {
		skill_string << "|G" << std::left << std::setw(20) << (*iter).second->long_name << " |MCast name:|G " << std::setw(20) << (*iter).second->name;
		skill_string << " |MCast time:|G " << std::setw(5) << Utilities::dtos((*iter).second->castTime, 2);
		skill_string << " |MCooldown:|G " << std::setw(7) << Utilities::dtos((*iter).second->cooldown, 2);
		skill_string << " |MCost: |G" << iter->second->costDescription;
		skill_string << "\r\n  |Y-- " << (*iter).second->description << "|X\r\n";
		ch->Send(skill_string.str());
		skill_string.str("");
    }
}

void cmd_cooldowns(Player * ch, string argument)
{
    ch->Send("|MCooldowns greater than 1.5 seconds:|X\r\n");

    for (auto iter = begin(ch->cooldowns); iter != end(ch->cooldowns); ++iter)
    {
        double cd;
        Skill * cdskill = Game::GetGame()->GetSkill(iter->first);
        if ((cd = ch->GetCooldownRemaining(cdskill)) > 1.5)
        {
            ch->Send("|MSpell:|G " + cdskill->long_name + " |MTime Left:|G " + Utilities::dtos(cd, 1) + "|X\r\n");
        }
    }
}

//learn is outdated, new plan is to figure out known skills via class skills only. Saving this function for immortal use
/*
void cmd_learn(Player * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("Skills available to learn: \r\n");
		std::set<int> foundskills;
        std::list<Player::ClassData>::iterator classdata;
        //For each player class
        for(classdata = ch->player->classList.begin(); classdata != ch->player->classList.end(); ++classdata)
        {
            std::list<Class::SkillData>::iterator classSkill;
            Class * currClass = Game::GetGame()->GetClass(classdata->id);
            for(classSkill = currClass->classSkills.begin(); classSkill != currClass->classSkills.end(); ++classSkill)
            {
                if((*classSkill).level <= ch->level && !ch->HasSkill((*classSkill).skill))
                {
                    Skill * sk = (*classSkill).skill;
					if(foundskills.find(sk->id) == foundskills.end())
					{
						stringstream learnstring;
						learnstring << "|G" << left << setw(40) << sk->long_name << " |MCast name:|G " << setw(20);
						learnstring << sk->name << " |MDesc:|G " << sk->description << "|X\r\n";
						ch->Send(learnstring.str());
						learnstring.str("");

						//ch->Send("|MSpell:|G " + sk->long_name + " |MName:|G " + sk->name + " |MDesc:|G " + sk->affectDescription + "|X\r\n");
					}
					foundskills.insert(sk->id);
                }
            }
        }
        return;
    }
    string arg1;
    argument = Utilities::one_argument(argument, arg1);

    if(ch->player->IsImmortal())
    {
        std::map<int, Skill*>::iterator skilliter; //ALL the skills :(
        for(skilliter = Game::GetGame()->skills.begin(); skilliter != Game::GetGame()->skills.end(); ++skilliter)
        {
            if(!Utilities::str_cmp((*skilliter).second->long_name, arg1))
            {
                Skill * dupe;
                if((dupe = ch->GetSkillShortName((*skilliter).second->name)) != nullptr)
                {
                    if(dupe->id == (*skilliter).second->id)
                    {
                        ch->Send("You already know this skill.\r\n");
                        return;
                    }
                    ch->Send("Learning this skill will replace " + dupe->long_name + ".\r\n");
					ch->SetQuery("Replace (" + dupe->long_name + ") with (" + (*skilliter).second->long_name + ")? (y/n) ", (*skilliter).second, cmd_learn_Query);
                }
                else
                {
                    ch->AddSkill((*skilliter).second);
                    ch->Send("You have learned " + (*skilliter).second->long_name + ".\r\n");
                }
                return;
            }
        }
        ch->Send("That skill is not available to learn.\r\n");
        return;
    }

    //for each player->class
    //  for each class skill
    //    if learnable and arg1 == name
    //      find trainer flag
    //      see if trainer in room
    //      check if we already know skill
    //      learn it
    std::list<Player::ClassData>::iterator classdata;
    for(classdata = ch->player->classList.begin(); classdata != ch->player->classList.end(); ++classdata)
    {
        std::list<Class::SkillData>::iterator classSkill;
        Class * currClass = Game::GetGame()->GetClass(classdata->id);
        for(classSkill = currClass->classSkills.begin(); classSkill != currClass->classSkills.end(); ++classSkill)
        {
            if((*classSkill).level <= ch->level && !ch->HasSkill((*classSkill).skill)
                && !Utilities::str_cmp((*classSkill).skill->long_name, arg1))
            {
                //Found a learnable skill
                //Figure out which type of trainer we should be looking for
                int classFlag = -1;
                for(int i = 0; Character::flag_table[i].flag != -1; i++)
                {
                    if(!Utilities::str_cmp(Character::flag_table[i].flag_name, currClass->name + "train"))
                    {
                        classFlag = Character::flag_table[i].flag;
                    }
                }
                if(classFlag == -1)
                {
                    LogFile::Log("error", "cmd_learn, class flag not found in flag_table");
                    return;
                }

                //See if a trainer is in the room
                bool found = false;
                std::list<Character *>::iterator inroom;
                for(inroom = ch->room->characters.begin(); inroom != ch->room->characters.end(); ++inroom)
                {
                    if(Utilities::FlagIsSet((*inroom)->flags, classFlag))
                    {
                        found = true;
                        break;
                    }
                }
                if(!found)
                {
                    ch->Send("You need to visit a " + currClass->name + " trainer to learn this skill.\r\n");
                    return;
                }

                Skill * dupe;
                if((dupe = ch->GetSkillShortName((*classSkill).skill->name)) != nullptr)
                {
                    if(dupe->id == (*classSkill).skill->id)
                    {
                        ch->Send("You already know this skill.\r\n");
                        return;
                    }
                    ch->Send("Learning this skill will replace " + dupe->long_name + ".\r\n");
					ch->SetQuery("Replace (" + dupe->long_name + ") with (" + (*classSkill).skill->long_name + ")? (y/n) ", (*classSkill).skill, cmd_learn_Query);
                }
                else
                {
                    ch->AddSkill((*classSkill).skill);
                    ch->Send("You have learned " + (*classSkill).skill->long_name + ".\r\n");
                }
                return;
            }
        }
    }
    ch->Send("That skill is not available to learn.\r\n");
}
*/

/*bool cmd_learn_Query(Player * ch, string argument)
{
    if(!ch)
    {
        ch->QueryClear();
        return true;
    }

    Skill * newskill = (Skill*)ch->GetQueryData();
    
    if(!Utilities::str_cmp(argument, "yes") || !Utilities::str_cmp(argument, "y"))
    {
        //learn newskill
        ch->AddSkill(newskill);
        ch->Send("You have learned " + newskill->long_name + ".\r\n");
        ch->QueryClear();
        return true;
    }
    else if(!Utilities::str_cmp(argument, "no") || !Utilities::str_cmp(argument, "n"))
    {
        ch->QueryClear();
        return true;
    }
    return false;
}*/

void cmd_affects(Player * ch, string argument)
{
    if(!ch)
        return;

    string arg1;
    argument = Utilities::one_argument(argument, arg1);
    std::list<SpellAffect*>::iterator iter;
    int i = 1;
    bool found = false;

    if(arg1.empty())
    {
        ch->Send("|MYou are affected by the following spells:|X\r\n");
        for(iter = ch->buffs.begin(); iter != ch->buffs.end(); ++iter)
        {
            if((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string stacks;
                if ((*iter)->currentStacks > 1)
                    stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                string oneaffect = Utilities::itos(i++) + ". |G" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\r\n";
                ch->Send(oneaffect);
                found = true;
            }
        }
        for(iter = ch->debuffs.begin(); iter != ch->debuffs.end(); ++iter)
        {
            if((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string stacks;
                if ((*iter)->currentStacks > 1)
                    stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\r\n";
                ch->Send(oneaffect);
                found = true;
            }
        }
        if (!found)
            ch->Send("None\r\n");
        found = false;
        if (ch->GetTarget() && ch->GetTarget() != ch)
        {
            ch->Send("|MYour target is affected by the following debuffs:|X\r\n");
            for (auto iter = ch->GetTarget()->debuffs.begin(); iter != ch->GetTarget()->debuffs.end(); ++iter)
            {
                if ((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
                {
                    double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                    string stacks;
                    if ((*iter)->currentStacks > 1)
                        stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                    string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                    if ((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                    {
                        oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                    }
                    oneaffect += (*iter)->affectDescription + "\r\n";
                    ch->Send(oneaffect);
                    found = true;
                }
            }
            if (!found)
                ch->Send("None\r\n");
        }
        return;
    }
    else if(!Utilities::str_prefix(arg1, "buff"))
    {
        ch->Send("|MYou are affected by the following beneficial spells:|X\r\n");
        for(iter = ch->buffs.begin(); iter != ch->buffs.end(); ++iter)
        {
            if(!(*iter)->hidden)
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string stacks;
                if ((*iter)->currentStacks > 1)
                    stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                string oneaffect = Utilities::itos(i++) + ". |G" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\r\n";
                ch->Send(oneaffect);            
            }
        }
        return;
    }
    else if(!Utilities::str_prefix(arg1, "debuff"))
    {
        ch->Send("|MYou are affected by the following harmful spells:|X\r\n");
        for(iter = ch->debuffs.begin(); iter != ch->debuffs.end(); ++iter)
        {
            if(!(*iter)->hidden)
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string stacks;
                if ((*iter)->currentStacks > 1)
                    stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\r\n";
                ch->Send(oneaffect);
            }
        }
        return;
    }
    else if (!Utilities::str_prefix(arg1, "target"))
    {
        if (!ch->GetTarget())
        {
            ch->Send("You don't have a target.\r\n");
            return;
        }
        ch->Send("|MYour target is affected by the following debuffs:|X\r\n");
        for (auto iter = ch->GetTarget()->debuffs.begin(); iter != ch->GetTarget()->debuffs.end(); ++iter)
        {
            if ((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string stacks;
                if ((*iter)->currentStacks > 1)
                    stacks = " |M(" + Utilities::itos((*iter)->currentStacks) + ")";
                string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + stacks + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if ((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\r\n";
                ch->Send(oneaffect);
            }
        }
        return;
    }
    ch->Send("affect 'buff'||'debuff'||'target'\r\n");
}

void cmd_train(Player * ch, string argument)
{
    std::string arg1;
    std::string arg2;

    argument = Utilities::one_argument(argument, arg1);
    argument = Utilities::one_argument(argument, arg2);

	if (arg1.empty())
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\r\n");
		return;
	}

    int howmany = 1;
    if (!arg2.empty() && Utilities::IsNumber(arg2))
    {
        howmany = Utilities::atoi(arg2);
    }

	if (ch->statPoints < howmany)
	{
		ch->Send("You don't have enough attribute points.\r\n");
		return;
	}
    
	if (!Utilities::str_cmp(arg1, "agility"))
	{
		ch->statPoints -= howmany;
		ch->SetAgility(ch->GetBaseAgility() + howmany);
		ch->Send("|WAgility increased: " + Utilities::itos(ch->GetBaseAgility()) + "|X\r\n");
	}
	else if (!Utilities::str_cmp(arg1, "intellect"))
	{
        ch->statPoints -= howmany;
        ch->SetIntellect(ch->GetBaseIntellect() + howmany);
		ch->Send("|WIntellect increased: " + Utilities::itos(ch->GetBaseIntellect()) + "|X\r\n");
	}
	else if (!Utilities::str_cmp(arg1, "strength"))
	{
        ch->statPoints -= howmany;
        ch->SetStrength(ch->GetBaseStrength() + howmany);
		ch->Send("|WStrength increased: " + Utilities::itos(ch->GetBaseStrength()) + "|X\r\n");
	}
	else if (!Utilities::str_cmp(arg1, "stamina"))
	{
        ch->statPoints -= howmany;
        ch->SetStamina(ch->GetBaseStamina() + howmany);
		ch->Send("|WStamina increased: " + Utilities::itos(ch->GetBaseStamina()) + "|X\r\n");
	}
	else if (!Utilities::str_cmp(arg1, "wisdom"))
	{
        ch->statPoints -= howmany;
        ch->SetWisdom(ch->GetBaseWisdom() + howmany);
		ch->Send("|WWisdom increased: " + Utilities::itos(ch->GetBaseWisdom()) + "|X\r\n");
	}
	else if (!Utilities::str_cmp(arg1, "spirit"))
	{
        ch->statPoints -= howmany;
        ch->SetSpirit(ch->GetBaseSpirit() + howmany);
		ch->Send("|WSpirit increased: " + Utilities::itos(ch->GetBaseSpirit()) + "|X\r\n");
	}
	else
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\r\n");
		return;
	}
	ch->ResetMaxStats();
}