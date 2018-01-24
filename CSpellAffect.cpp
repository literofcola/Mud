#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
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
#include "CCommand.h"
#include "utils.h"
#include "mud.h"

const struct SpellAffect::AuraTable aura_table[] = 
{
    { "MOVE_SPEED", SpellAffect::AURA_MOVE_SPEED },
    { "RESOURCE_COST", SpellAffect::AURA_RESOURCE_COST },
    { "", 0 }
};

const struct SpellAffect::CategoryType category_table[] = 
{
    { "", SpellAffect::AFFECT_NONE },
    { "|WMagic|X", SpellAffect::AFFECT_MAGIC },
    { "|MCurse|X", SpellAffect::AFFECT_CURSE },
    { "|GPoison|X", SpellAffect::AFFECT_POISON },
    { "|YDisease|X", SpellAffect::AFFECT_DISEASE },
    { "|RBleed|X", SpellAffect::AFFECT_BLEED },
    { "|CFrenzy|X", SpellAffect::AFFECT_FRENZY },
    { "AFFECT_LAST", -1 }
};

SpellAffect::SpellAffect()
{
    debuff = hidden = stackable = false;
    id = ticks = ticksRemaining = 0;
    duration = appliedTime = 0;
    caster = NULL;
    skill = NULL;
    affectCategory = SpellAffect::AFFECT_NONE;
    remove_me = false;
}

SpellAffect::~SpellAffect()
{
    if(caster)
    {
        //LogFile::Log("status", "Removing listener " + this->name + " from " + caster->name);
        caster->RemoveListener(this);
    }
    auraAffects.clear();
}

void SpellAffect::ApplyAura(string affectName, int modifier)
{
    bool found = false;
    int i;
    for(i = 0; aura_table[i].affectID != 0; i++)
    {
        if(affectName == aura_table[i].affectName)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        LogFile::Log("error", "SpellAffect::ApplyAura, bad affectName");
        return;
    }
    AuraAffect af = { aura_table[i].affectID, modifier };
    auraAffects.push_front(af);
}

void SpellAffect::ApplyAura(int affectID, int modifier)
{
    bool found = false;
    for(int i = 0; aura_table[i].affectID != 0; i++)
    {
        if(affectID == aura_table[i].affectID)
        {
            found = true;
            break;
        }
    }
    if(!found)
    {
        LogFile::Log("error", "SpellAffect::ApplyAura, bad affectID");
        return;
    }
    AuraAffect af = { affectID, modifier };
    auraAffects.push_front(af);
}

std::string SpellAffect::GetCasterName()
{
    if(caster != NULL)
        return caster->name;
    return casterName;
}

std::string SpellAffect::GetAffectCategoryName()
{
    if(affectCategory < 0 || affectCategory > SpellAffect::AFFECT_LAST)
        return "";
    return category_table[affectCategory].categoryName;
}

void SpellAffect::SaveDataInt(std::string tag, int val)
{
    affectDataInt[tag] = val;
}

void SpellAffect::SaveDataDouble(std::string tag, double val)
{
    affectDataDouble[tag] = val;
}

void SpellAffect::SaveDataString(std::string tag, string val)
{
    affectDataString[tag] = val;
}

int SpellAffect::GetDataInt(std::string tag)
{
    return affectDataInt[tag];
}

double SpellAffect::GetDataDouble(std::string tag)
{
    return affectDataDouble[tag];
}

std::string SpellAffect::GetDataString(std::string tag)
{
    return affectDataString[tag];
}

void SpellAffect::Notify(ListenerManager * lm)
{
    //caster about to be deleted... player quit, npc killed etc. At least save the name
    //LogFile::Log("status", "SA " + name + " setting casterName = " + caster->name);
    casterName = caster->name;
    caster = NULL;
}

void SpellAffect::Save(std::string charname)
{
    double timeleft = (appliedTime + duration) - Game::currentTime;

    string affectsql = "INSERT INTO affects (player, hidden, stackable, ticks, duration, skill, debuff, timeleft, ";
    affectsql += "caster, auras, data, category) ";
    affectsql += "values ('" + charname + "', " + Utilities::itos(hidden) + ", " + Utilities::itos(stackable) + ", " + Utilities::itos(ticks);
    affectsql += ", " + Utilities::dtos(duration, 1) + ", ";
    if(skill)
            affectsql += Utilities::itos(skill->id);
    else
            affectsql += "0";

    affectsql += Utilities::itos(debuff) + ", ";
    affectsql += Utilities::dtos(timeleft, 1) + ", '" + casterName + "', '";

    std::list<struct AuraAffect>::iterator iter;

    if(!auraAffects.empty())
    {
        for(iter = auraAffects.begin(); iter != auraAffects.end(); ++iter)
        {
            affectsql += Utilities::itos((*iter).affectID) + "," + Utilities::itos((*iter).modifier) + ";";
        }
    }
    affectsql += "', '";
    
    for(std::map<std::string, int>::iterator iter = affectDataInt.begin(); iter != affectDataInt.end(); iter++)
    {
        affectsql += "i," + (*iter).first + "," + Utilities::itos((*iter).second) + ";";
    }
    for(std::map<std::string, double>::iterator iter = affectDataDouble.begin(); iter != affectDataDouble.end(); iter++)
    {
        affectsql += "d," + (*iter).first + "," + Utilities::dtos((*iter).second, 2) + ";";
    }
    for(std::map<std::string, std::string>::iterator iter = affectDataString.begin(); iter != affectDataString.end(); iter++)
    {
        affectsql += "s," + (*iter).first + "," + (*iter).second + ";";
    }

    affectsql += "', " + Utilities::itos(affectCategory) + ");";
    Server::sqlQueue->Write(affectsql);
}

void SpellAffect::Load(Character * ch)
{
    StoreQueryResult affectres = Server::sqlQueue->Read("select * from player_spell_affects where player='" + ch->name + "'");
    if(affectres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = affectres.begin(); i != affectres.end(); i++)
    {
        row = *i;

        Skill * sk = Game::GetGame()->GetSkill(row["skill"]);
        if(sk == NULL)
        {
            LogFile::Log("error", "SpellAffect::Load, bad skill id");
            continue;
        }

        //ch->AddSpellAffect(row["is_debuff"], ch, sk->long_name, row["hidden"], row["stackable"], row["ticks"], row["duration"], sk); 
        //don't use AddSpellAffect since we don't want to call the _apply function
        SpellAffect * sa = new SpellAffect();
        sa->name = sk->long_name;
        sa->hidden = row["hidden"];
        sa->stackable = row["stackable"];
        sa->ticks = row["ticks"];
        sa->duration = row["duration"];
        sa->appliedTime = Utilities::GetTime() - ((double)row["duration"] - row["timeleft"]); //Game::currentTime isn't initialized until after one update...
        sa->skill = sk;
        sa->debuff = row["debuff"];
        sa->caster = NULL;
        sa->casterName = row["caster"];
        sa->affectCategory = row["category"];
        double tick_interval = (double)row["duration"] / row["ticks"];
        sa->ticksRemaining = (int)(row["timeleft"] / tick_interval);

        if(sa->debuff)
        {
            sa->id = (int)ch->debuffs.size() + 1;
            ch->debuffs.push_front(sa);
        }
        else
        {
            sa->id = (int)ch->buffs.size() + 1;
            ch->buffs.push_front(sa);
        }
        string affect_data = (row["data"]).c_str();
        int first = 0, second = 0;
        while(first < (int)affect_data.length())
        {
            char data_type = affect_data[first];
            second = affect_data.find(',', first+2);
            string data_name = affect_data.substr(first+2, second - (first+2));
            first = second+1;
            second = affect_data.find(';', first);
            switch(data_type)
            {
                case 'i':
                {         
                    sa->affectDataInt[data_name] = Utilities::atoi(affect_data.substr(first, second - first));
                    break;
                }
                case 'd':
                {
                    sa->affectDataDouble[data_name] = Utilities::atof(affect_data.substr(first, second - first));
                    break;
                }
                case 's':
                {
                    sa->affectDataString[data_name] = affect_data.substr(first, second - first);
                    break;
                }
            }
            first = second + 1;
        }

        string aurastring = row["auras"].c_str();
        first = second = 0;
        while(first < (int)aurastring.length())
        {
            int affectid;
            int modifier;
            second = aurastring.find(',', first);
            affectid = Utilities::atoi(aurastring.substr(first, second - first));
            first = second + 1;
            second = aurastring.find(';', first);
            modifier = Utilities::atoi(aurastring.substr(first, second - first));
            first = second + 1;
            sa->ApplyAura(affectid, modifier);
        }
    }
    string affectsql = "DELETE FROM player_spell_affects WHERE player = '" + ch->name + "'";
    Server::sqlQueue->Write(affectsql);
}
