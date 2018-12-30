#include "stdafx.h"
#include "CNPCIndex.h"
#include "CCharacter.h"
#include "CTrigger.h"
#include "utils.h"
#include "CServer.h"
#include "CSkill.h"

using std::string;

NPCIndex::flag_type NPCIndex::flag_table[] =
{
	{ NPCIndex::FLAG_FRIENDLY, "friendly" },
	{ NPCIndex::FLAG_NEUTRAL, "neutral" },
	{ NPCIndex::FLAG_AGGRESSIVE, "aggressive" },
	{ NPCIndex::FLAG_GUILD, "guild" },
	{ NPCIndex::FLAG_VENDOR, "vendor" },
	{ NPCIndex::FLAG_REPAIR, "repair" },
	{ NPCIndex::FLAG_TRAINER, "trainer" },
	{ -1, "" }
};

NPCIndex::NPCIndex(int id_, std::string name_)
{
	id = id_;
	name = name_;

	//Set some defaults
	keywords = name_;
	level = 1;
	gender = 1;
	race = Character::Races::RACE_HUMAN;
	maxHealth = 50;
	maxMana = 50;
	maxEnergy = 100;
	maxRage = 100;
	npcAttackSpeed = 2.0;
	npcDamageLow = npcDamageHigh = 1;
	changed = false;
	remove = false;

	stringTable["name"] = &name;
	stringTable["keywords"] = &keywords;
	doubleTable["attack_speed"] = &npcAttackSpeed;
	intTable["id"] = &id;
	intTable["damage_low"] = &npcDamageLow;
	intTable["damage_high"] = &npcDamageHigh;
	intTable["maxhealth"] = &maxHealth;
	intTable["maxmana"] = &maxMana;
	intTable["maxenergy"] = &maxEnergy;
	intTable["maxrage"] = &maxRage;
	intTable["race"] = &race;
}

NPCIndex::~NPCIndex()
{

}

void NPCIndex::AddTrigger(Trigger & trig)
{
	int ctr = 1;
	std::map<int, Trigger>::iterator iter;
	for (iter = triggers.begin(); iter != triggers.end(); ++iter)
	{
		if (ctr != iter->second.id)
		{
			//found the first integer not in the map
			break;
		}
		ctr++;
	}
	trig.id = ctr;
	changed = true;
	triggers.insert(std::pair<int, Trigger>(trig.id, trig));
}

//If default argument type = -1, search for trigger id
//Otherwise search for id-th trigger of type
Trigger * NPCIndex::GetTrigger(int id, int type)
{
	if (type == -1)
	{
		std::map<int, Trigger>::iterator it = triggers.find(id);
		if (it != triggers.end())
			return &(it->second);
		return nullptr;
	}
	else
	{
		std::map<int, Trigger>::iterator it;
		int count = 0;
		for (it = triggers.begin(); it != triggers.end(); ++it)
		{
			if (it->second.GetType() == type && count++ >= id)
			{
				return &(it->second);
			}
		}
		return nullptr;
	}
	return nullptr;
}

bool NPCIndex::FlagIsSet(const int flag)
{
	return Utilities::FlagIsSet(this->flags, flag);
}


void NPCIndex::Save()
{
	string sql;
	string fixtitle = Utilities::SQLFixQuotes(title);

	sql = "INSERT INTO npcs (id, name, keywords, level, gender, race, ";
	sql += "health, mana, energy, rage, armor, title, attack_speed, damage_low, damage_high, flags, speechtext) values (";
	sql += Utilities::itos(id) + ", '";
	sql += Utilities::SQLFixQuotes(name) + "', '" + Utilities::SQLFixQuotes(keywords) + "', " + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
	sql += Utilities::itos(maxHealth) + "," + Utilities::itos(maxMana) + "," + Utilities::itos(maxEnergy) + "," + Utilities::itos(maxRage);
	sql += "," + Utilities::itos(armor) + ",'" + fixtitle + "', " + Utilities::dtos(npcAttackSpeed, 2) + ", " + Utilities::itos(npcDamageLow) + ", ";
	sql += Utilities::itos(npcDamageHigh) + ",'";

	std::vector<int>::iterator flagiter;
	for (flagiter = flags.begin(); flagiter != flags.end(); ++flagiter)
	{
		sql += Utilities::itos((*flagiter)) + ";";
	}
	sql += "','" + Utilities::SQLFixQuotes(speechText) + "')";

	sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), level=VALUES(level), gender=VALUES(gender), race=VALUES(race), ";
	sql += "health=VALUES(health), mana=VALUES(mana), energy=VALUES(energy), rage=VALUES(rage), armor=VALUES(armor),";
	sql += "title=VALUES(title), attack_speed=VALUES(attack_speed), damage_low=VALUES(damage_low), ";
	sql += "damage_high=VALUES(damage_high), flags=VALUES(flags), speechtext=VALUES(speechtext)";

	//save drops
	Server::sqlQueue->Write("DELETE FROM npc_drops where npc=" + Utilities::itos(id));
	std::list<DropData>::iterator dropsiter;
	for (dropsiter = drops.begin(); dropsiter != drops.end(); ++dropsiter)
	{
		string dropsql = "INSERT INTO npc_drops (npc, items, percent) values ";
		dropsql += "(" + Utilities::itos(id) + ", '";
		for (int i = 0; i < (int)(*dropsiter).id.size(); i++)
		{
			dropsql += Utilities::itos((*dropsiter).id[i]) + ";";
		}


		dropsql += "', " + Utilities::itos((*dropsiter).percent) + ")";
		dropsql += " ON DUPLICATE KEY UPDATE npc=VALUES(npc), items=VALUES(items), percent=VALUES(percent)";
		Server::sqlQueue->Write(dropsql);

	}

	//save Triggers
    string triggersql = "DELETE FROM triggers where triggers.parent_type=" + Utilities::itos(Trigger::PARENT_NPC)
        + " and triggers.parent_id=" + Utilities::itos(id);
    Server::sqlQueue->Write(triggersql);

	for(auto trigiter = begin(triggers); trigiter != end(triggers); ++trigiter)
	{
        Trigger * t = &(trigiter->second);

		string triggersql = "INSERT INTO triggers (parent_id, id, parent_type, type, argument, script, function) values ";
		triggersql += "(" + Utilities::itos(id) + ", " + Utilities::itos(t->id) + ", " + Utilities::itos(Trigger::PARENT_NPC) + ", ";
		triggersql += Utilities::itos(t->GetType()) + ", '" + Utilities::SQLFixQuotes(t->GetArgument()) + "', '";
		triggersql += Utilities::SQLFixQuotes(t->GetScript()) + "', '" + Utilities::SQLFixQuotes(t->GetFunction()) + "')";

		triggersql += " ON DUPLICATE KEY UPDATE parent_id=VALUES(parent_id), id=VALUES(id), parent_type=VALUES(parent_type), ";
		triggersql += "type=VALUES(type), argument=VALUES(argument), script=VALUES(script), function=VALUES(function)";

		Server::sqlQueue->Write(triggersql);
	}
	Server::sqlQueue->Write(sql);

    changed = false;
}
