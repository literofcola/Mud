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
        delayData.caster->Send("Your target is no longer here.\n\r");
        return;
    }
	
    if(delayData.charTarget != delayData.caster)
    {
		if (!delayData.caster->GetCharacterRoom(delayData.charTarget) 
			&& FindDirection(delayData.caster, delayData.charTarget, 3) == Exit::DIR_LAST)
		{
			delayData.caster->Send("Your target is no longer here.\n\r");
			return;
		}
    }
    
    if(delayData.charTarget == delayData.caster && (delayData.sk->targetType == Skill::TARGET_OTHER ||
        delayData.sk->targetType == Skill::TARGET_HOSTILE))
    {
        delayData.caster->Send("You must target someone else with this skill.\n\r");
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
        delayData.caster->Send("You can't cast this spell on that target.\n\r");
        return;
    }

    int lua_ret = 0;
    string cost_func = delayData.sk->function_name + "_cost";
	try
	{
		sol::function lua_cost_func = Server::lua[cost_func.c_str()];
		sol::protected_function_result result = lua_cost_func(delayData.caster, delayData.charTarget, delayData.sk);
		if (!result.valid())
		{
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			LogFile::Log("error", "_cost call failed, sol::error::what() is: " + what);
		}
		else
		{
			lua_ret = result;
		}
	}
	catch (const std::exception & e)
	{
		LogFile::Log("error", e.what());
	}

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

    string func = delayData.sk->function_name + "_cast";
	try
	{
		sol::function lua_cast_func = Server::lua[func.c_str()];
		sol::protected_function_result result = lua_cast_func(delayData.caster, delayData.charTarget, delayData.sk);
		if (!result.valid())
		{
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			LogFile::Log("error", "_cast call failed, sol::error::what() is: " + what);
		}
	}
	catch (const std::exception & e)
	{
		LogFile::Log("error", e.what());
	}
	delayData.caster->SetCooldown(delayData.sk, -1);
}

void cmd_cast(Player * ch, string argument)
{
    if(!ch)
        return;

    if(ch->delay_active)
    {
        ch->Send("Another action is in progress!\n\r");
        return;
    }

    if(argument.empty())
    {
        ch->Send("Cast what spell?\n\r");
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
        ch->Send("You don't know that skill.\n\r");
        return;
    }

    double cd;
    if((cd = ch->GetCooldownRemaining(spell)) > 0)
    {
        ch->Send(spell->long_name + " will be ready in " + Utilities::dtos(cd, 1) + " seconds.\n\r");
        return;
    }
    
    Character * arg_target = nullptr;
    if(!arg2.empty())
    {
        arg_target = ch->GetCharacterRoom(arg2);
        if(arg_target == nullptr)
        {
            ch->Send("They aren't here.\n\r");
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
        ch->Send("You must target someone with this skill.\n\r");
        return;
    }


	if (spell->targetType == Skill::TARGET_HOSTILE
		&& arg_target->FlagIsSet(NPCIndex::FLAG_FRIENDLY)
		&& (!ch->IsImmortal()))
	{
		ch->Send("That target is friendly.\n\r");
		return;
	}
    

    if(spell->targetType == Skill::TARGET_FRIENDLY && arg_target && ch->IsFighting(arg_target))
    {
        ch->Send("You can't cast this spell on that target.\n\r");
        return;
    }

    int lua_ret = 0;
    string cost_func = spell->function_name + "_cost";
    try
    {
		sol::function lua_cost_func = Server::lua[cost_func.c_str()];
		sol::protected_function_result result = lua_cost_func(ch, arg_target, spell);
		if (!result.valid())
		{
			// Call failed
			sol::error err = result;
			std::string what = err.what();
			LogFile::Log("error", "_cost call failed, sol::error::what() is: " + what);
		}
		else
		{
			lua_ret = result;
		}
    }
	catch (const std::exception & e)
	{
		LogFile::Log("error", e.what());
	}
    
    if(lua_ret == 0)
    {
        //not enough resources to cast, or some other restriction
        return;
    }

	if (spell->castTime != 0)
	{
		ch->Message("|W" + ch->GetName() + " begins to cast " + spell->long_name + "...|X", Character::MSG_ROOM_NOTCHAR);
		ch->Send("|WYou begin to cast " + spell->long_name + "...|X\n\r");
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

    ch->Send("|MKnown spells/skills:|X\n\r");
    std::stringstream skill_string;
	std::map<string, Skill *>::iterator iter;
    for(iter = ch->knownSkills.begin(); iter != ch->knownSkills.end(); ++iter)
    {
		skill_string << "|G" << std::left << std::setw(20) << (*iter).second->long_name << " |MCast name:|G " << std::setw(20) << (*iter).second->name;
		skill_string << " |MCast time:|G " << std::setw(5) << Utilities::dtos((*iter).second->castTime, 2);
		skill_string << " |MCooldown:|G " << std::setw(7) << Utilities::dtos((*iter).second->cooldown, 2);
		skill_string << " |MCost: |G" << iter->second->costDescription;
		skill_string << "\n\r  |Y-- " << (*iter).second->description << "|X\n\r";
		ch->Send(skill_string.str());
		skill_string.str("");
    }
}

void cmd_cooldowns(Player * ch, string argument)
{
    ch->Send("|MCooldowns greater than 1.5 seconds:|X\n\r");

    std::map<string, Skill *>::iterator iter;
    for(iter = ch->knownSkills.begin(); iter != ch->knownSkills.end(); ++iter)
    {
        double cd;
        if((cd = ch->GetCooldownRemaining((*iter).second)) > 1.5)
        {
            ch->Send("|MSpell:|G " + (*iter).second->long_name + " |MTime Left:|G " + Utilities::dtos(cd, 1) + "|X\n\r");
        }
    }
}

//todo: learn is outdated, new plan is to figure out known skills via class skills only. Saving this function for immortal use
/*
void cmd_learn(Player * ch, string argument)
{
    if(argument.empty())
    {
        ch->Send("Skills available to learn: \n\r");
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
						learnstring << sk->name << " |MDesc:|G " << sk->description << "|X\n\r";
						ch->Send(learnstring.str());
						learnstring.str("");

						//ch->Send("|MSpell:|G " + sk->long_name + " |MName:|G " + sk->name + " |MDesc:|G " + sk->affectDescription + "|X\n\r");
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
                        ch->Send("You already know this skill.\n\r");
                        return;
                    }
                    ch->Send("Learning this skill will replace " + dupe->long_name + ".\n\r");
					ch->SetQuery("Replace (" + dupe->long_name + ") with (" + (*skilliter).second->long_name + ")? (y/n) ", (*skilliter).second, cmd_learn_Query);
                }
                else
                {
                    ch->AddSkill((*skilliter).second);
                    ch->Send("You have learned " + (*skilliter).second->long_name + ".\n\r");
                }
                return;
            }
        }
        ch->Send("That skill is not available to learn.\n\r");
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
                    ch->Send("You need to visit a " + currClass->name + " trainer to learn this skill.\n\r");
                    return;
                }

                Skill * dupe;
                if((dupe = ch->GetSkillShortName((*classSkill).skill->name)) != nullptr)
                {
                    if(dupe->id == (*classSkill).skill->id)
                    {
                        ch->Send("You already know this skill.\n\r");
                        return;
                    }
                    ch->Send("Learning this skill will replace " + dupe->long_name + ".\n\r");
					ch->SetQuery("Replace (" + dupe->long_name + ") with (" + (*classSkill).skill->long_name + ")? (y/n) ", (*classSkill).skill, cmd_learn_Query);
                }
                else
                {
                    ch->AddSkill((*classSkill).skill);
                    ch->Send("You have learned " + (*classSkill).skill->long_name + ".\n\r");
                }
                return;
            }
        }
    }
    ch->Send("That skill is not available to learn.\n\r");
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
        ch->Send("You have learned " + newskill->long_name + ".\n\r");
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

    if(arg1.empty())
    {
        ch->Send("|MYou are affected by the following spells:|X\n\r");
        for(iter = ch->buffs.begin(); iter != ch->buffs.end(); ++iter)
        {
            if((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string oneaffect;
                oneaffect = Utilities::itos(i++) + ". |G" + (*iter)->name + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\n\r";
                ch->Send(oneaffect);
            }
        }
        for(iter = ch->debuffs.begin(); iter != ch->debuffs.end(); ++iter)
        {
            if((*iter)->skill != nullptr && (!(*iter)->hidden || ch->IsImmortal()))
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\n\r";
                ch->Send(oneaffect);
            }
        }
    }
    else if(!Utilities::str_prefix(arg1, "buff"))
    {
        ch->Send("|MYou are affected by the following beneficial spells:|X\n\r");
        for(iter = ch->buffs.begin(); iter != ch->buffs.end(); ++iter)
        {
            if(!(*iter)->hidden)
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string oneaffect;
                oneaffect = Utilities::itos(i++) + ". |G" + (*iter)->name + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\n\r";
                ch->Send(oneaffect);            
            }
        }
    }
    else if(!Utilities::str_prefix(arg1, "debuff"))
    {
        ch->Send("|MYou are affected by the following harmful spells:|X\n\r");
        for(iter = ch->debuffs.begin(); iter != ch->debuffs.end(); ++iter)
        {
            if(!(*iter)->hidden)
            {
                double timeleft = ((*iter)->appliedTime + (*iter)->duration) - Game::currentTime;
                string oneaffect = Utilities::itos(i++) + ". |R" + (*iter)->name + "|X " + Utilities::dtos(timeleft, 1) + " seconds || ";
                if((*iter)->affectCategory != SpellAffect::AFFECT_NONE)
                {
                    oneaffect += (*iter)->GetAffectCategoryName() + " || ";
                }
                oneaffect += (*iter)->affectDescription + "\n\r";
                ch->Send(oneaffect);
            }
        }
    }
}

void cmd_train(Player * ch, string argument)
{
	if (argument.empty())
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\n\r");
		return;
	}
	if (ch->statPoints <= 0)
	{
		ch->Send("You don't have any attribute points.\n\r");
		return;
	}
	if (!Utilities::str_cmp(argument, "agility"))
	{
		ch->statPoints--;
		ch->SetAgility(ch->GetBaseAgility()+1);
		ch->Send("|WAgility increased: " + Utilities::itos(ch->GetBaseAgility()) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "intellect"))
	{
		ch->statPoints--;
		ch->SetIntellect(ch->GetBaseIntellect() + 1);
		ch->Send("|WIntellect increased: " + Utilities::itos(ch->GetBaseIntellect()) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "strength"))
	{
		ch->statPoints--;
		ch->SetStrength(ch->GetBaseStrength() + 1);
		ch->Send("|WStrength increased: " + Utilities::itos(ch->GetBaseStrength()) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "stamina"))
	{
		ch->statPoints--;
		ch->SetStamina(ch->GetBaseStamina() + 1);
		ch->Send("|WStamina increased: " + Utilities::itos(ch->GetBaseStamina()) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "wisdom"))
	{
		ch->statPoints--;
		ch->SetWisdom(ch->GetBaseWisdom() + 1);
		ch->Send("|WWisdom increased: " + Utilities::itos(ch->GetBaseWisdom()) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "spirit"))
	{
		ch->statPoints--;
		ch->SetSpirit(ch->GetBaseSpirit() + 1);
		ch->Send("|WSpirit increased: " + Utilities::itos(ch->GetBaseSpirit()) + "|X\n\r");
	}
	else
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\n\r");
		return;
	}
	ch->ResetMaxStats();
}