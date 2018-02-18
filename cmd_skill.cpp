#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
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

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

//#include "luabind/luabind.hpp"

using namespace std;

void cmd_castCallback(Character::DelayData delayData)
{
    if(!delayData.caster)
    {
        LogFile::Log("error", "cmd_castCallback: NULL caster");
        return;
    }

    if(!delayData.sk)
    {
        LogFile::Log("error", "cmd_castCallback: NULL skill");
        return;
    }

	json casttime = { { "time", 0 } };
	delayData.caster->SendGMCP("char.casttime " + casttime.dump());

    if(delayData.charTarget && delayData.charTarget != delayData.caster)
    {
        delayData.charTarget->RemoveSubscriber(delayData.caster);
    }

    if(delayData.charTarget == NULL) //target will never be null from cmd_cast, only from Subscriber::Notify 
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
		lua_ret = lua_cost_func(delayData.caster, delayData.charTarget, delayData.sk);
        //lua_ret = luabind::call_function<int>(Server::luaState, cost_func.c_str(), delayData.caster, delayData.charTarget, delayData.sk);
    }
	/*catch(const std::runtime_error & e)
	{
        LogFile::Log("error", e.what());
		const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
		if(logstring != NULL)
			LogFile::Log("error", logstring);
	}*/
	catch (const sol::error& e) 
	{
		LogFile::Log("error", e.what());
	}
	/*catch(const std::exception & e)
	{
		LogFile::Log("error", e.what());
		const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
		if(logstring != NULL)
			LogFile::Log("error", logstring);
	}*/
	catch(...)
	{
		LogFile::Log("error", "call_function unhandled exception cmd_castcallback _cost");
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
		lua_cast_func(delayData.caster, delayData.charTarget, delayData.sk);
    }
	catch (const sol::error& e)
	{
		LogFile::Log("error", e.what());
	}
	catch (const std::exception& e)
	{
		LogFile::Log("error", e.what());
	}
	catch(...)
	{
		LogFile::Log("error", "call_function unhandled exception cmd_castcallback _cast");
	}
}

void cmd_cast(Character * ch, string argument)
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

    if(spell == NULL)
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
    
    Character * arg_target = NULL;
    if(!arg2.empty())
    {
        arg_target = ch->GetCharacterRoom(arg2);
        if(arg_target == NULL)
        {
            ch->Send("They aren't here.\n\r");
            return;
        }
    }

    if(arg_target == NULL)
        arg_target = ch->GetTarget();

    if((spell->targetType == Skill::TARGET_OTHER || spell->targetType == Skill::TARGET_HOSTILE)
        && (!arg_target || arg_target == ch)) //Requires a target
    {
        ch->Send("You must target someone with this skill.\n\r");
        return;
    }

    if(spell->targetType == Skill::TARGET_HOSTILE 
        && Utilities::FlagIsSet(arg_target->flags, Character::FLAG_FRIENDLY)
        && (!ch->player || !ch->player->IMMORTAL()))
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
		lua_ret = lua_cost_func(ch, arg_target, spell);
        //lua_ret = luabind::call_function<int>(Server::luaState, cost_func.c_str(), ch, arg_target, spell);
    }
	/*catch(const std::runtime_error & e)
	{
        LogFile::Log("error", e.what());
		const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
		if(logstring != NULL)
			LogFile::Log("error", logstring);
	}*/
	catch (const sol::error& e)
	{
		LogFile::Log("error", e.what());
	}
	/*
	catch(const std::exception & e)
	{
		LogFile::Log("error", e.what());
		const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
		if(logstring != NULL)
			LogFile::Log("error", logstring);
	}
	*/
	catch(...)
	{
		LogFile::Log("error", "call_function unhandled exception cmd_cast _cost");
	}
    
    if(lua_ret == 0)
    {
        //not enough resources to cast
        return;
    }

	if (spell->castTime != 0)
	{
		ch->Message("|W" + ch->name + " begins to cast " + spell->long_name + "...|X", Character::MSG_ROOM_NOTCHAR);
		ch->Send("|WYou begin to cast " + spell->long_name + "...|X\n\r");
		json casttime = { { "time", spell->castTime } };
		ch->SendGMCP("char.casttime " + casttime.dump());
	}
 
    //Start global cooldown
    ch->SetCooldown(NULL, "", true, 0);
    //TODO: call the skill function (or just cmd_castCallback) directly with no delay for instant skills ???
    ch->delay = (Game::GetGame()->currentTime + spell->castTime);
    Character::DelayData dd;
    dd.caster = ch;
    if(arg_target == NULL)
        arg_target = ch;
    dd.charTarget = arg_target;
	if (arg_target && ch != arg_target)
	{
		dd.charTarget->AddSubscriber(dd.caster); //if our target is gone when spell finishes, we need to know about it
	}
    dd.sk = spell;
    ch->delayData = dd;
    ch->delay_active = true;
    ch->delayFunction = cmd_castCallback;
}

void cmd_skills(Character * ch, string argument)
{
    if(!ch)
        return;

    ch->Send("|MKnown spells/skills:|X\n\r");
    stringstream skill_string;
	std::map<string, Skill *>::iterator iter;
    for(iter = ch->knownSkills.begin(); iter != ch->knownSkills.end(); ++iter)
    {
		skill_string << "|G" << left << setw(20) << (*iter).second->long_name << " |MCast name:|G " << setw(20) << (*iter).second->name;
		skill_string << " |MCast time:|G " << setw(5) << Utilities::dtos((*iter).second->castTime, 2);
		skill_string << " |MCooldown:|G " << setw(7) << Utilities::dtos((*iter).second->cooldown, 2);
		skill_string << " |MCost: |G" << iter->second->costDescription;
		skill_string << "\n\r  |Y-- " << (*iter).second->description << "\n\r";
		ch->Send(skill_string.str());
		skill_string.str("");
    }
}

void cmd_cooldowns(Character * ch, string argument)
{
    ch->Send("|MCooldowns greater than 2 seconds: |X\n\r");

    std::map<string, Skill *>::iterator iter;
    for(iter = ch->knownSkills.begin(); iter != ch->knownSkills.end(); ++iter)
    {
        double cd;
        if((cd = ch->GetCooldownRemaining((*iter).second)) >= 2)
        {
            ch->Send("|MSpell:|G " + (*iter).second->long_name + " |MTime Left:|G " + Utilities::dtos(cd, 1) + "|X\n\r");
        }
    }
}

//todo: learn is outdated, new plan is to figure out known skills via class skills only. Saving this function for immortal use
void cmd_learn(Character * ch, string argument)
{
    if(!ch || !ch->player)
        return;

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

    if(ch->player->IMMORTAL())
    {
        std::map<int, Skill*>::iterator skilliter; //ALL the skills :(
        for(skilliter = Game::GetGame()->skills.begin(); skilliter != Game::GetGame()->skills.end(); ++skilliter)
        {
            if(!Utilities::str_cmp((*skilliter).second->long_name, arg1))
            {
                Skill * dupe;
                if((dupe = ch->GetSkillShortName((*skilliter).second->name)) != NULL)
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
                if((dupe = ch->GetSkillShortName((*classSkill).skill->name)) != NULL)
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

bool cmd_learn_Query(Character * ch, string argument)
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
}

void cmd_affects(Character * ch, string argument)
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
            if((*iter)->skill != NULL && (!(*iter)->hidden || (ch->player && ch->player->IMMORTAL())))
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
            if((*iter)->skill != NULL && (!(*iter)->hidden || (ch->player && ch->player->IMMORTAL())))
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

void cmd_train(Character * ch, string argument)
{
	if (!ch || !ch->player)
		return;

	if (argument.empty())
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\n\r");
		return;
	}
	if (ch->player->statPoints <= 0)
	{
		ch->Send("You don't have any attribute points.\n\r");
		return;
	}
	if (!Utilities::str_cmp(argument, "agility"))
	{
		ch->player->statPoints--;
		ch->agility++;
		ch->Send("|WAgility increased: " + Utilities::itos(ch->agility) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "intellect"))
	{
		ch->player->statPoints--;
		ch->intellect++;
		ch->Send("|WIntellect increased: " + Utilities::itos(ch->intellect) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "strength"))
	{
		ch->player->statPoints--;
		ch->strength++;
		ch->Send("|WStrength increased: " + Utilities::itos(ch->strength) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "stamina"))
	{
		ch->player->statPoints--;
		ch->stamina++;
		ch->Send("|WStamina increased: " + Utilities::itos(ch->stamina) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "wisdom"))
	{
		ch->player->statPoints--;
		ch->wisdom++;
		ch->Send("|WWisdom increased: " + Utilities::itos(ch->wisdom) + "|X\n\r");
	}
	else if (!Utilities::str_cmp(argument, "spirit"))
	{
		ch->player->statPoints--;
		ch->spirit++;
		ch->Send("|WSpirit increased: " + Utilities::itos(ch->spirit) + "|X\n\r");
	}
	else
	{
		ch->Send("Specify an attribute to increase: agility intellect strength stamina wisdom spirit\n\r");
		return;
	}
	ch->ResetMaxStats();
}