#include "stdafx.h"
#include "CPlayer.h"
#include "CGame.h"
#include "CServer.h"
#include "CmySQLQueue.h"
#include "CUser.h"
#include "CGroup.h"
#include "CNPCIndex.h"
#include "CNPC.h"
#include "CSpellAffect.h"
#include "CCharacter.h"
#include "CItem.h"
#include "CSkill.h"
#include "CRoom.h"
#include "CQuest.h"
#include "CClass.h"
#include "CLogFile.h"
#include "utils.h"
// for convenience
using json = nlohmann::json;

using std::string;

Player::Player(std::string name_, User * user_) : Character()
{
	name = name_;
	user = user_;

    saved = -1;
	level = 1;
	gender = 1;
	race = Character::RACE_HUMAN;
    immlevel = 0;
    experience = 0;
	statPoints = 0;
	armor = 0;
	recall = 0;
    isGhost = false;
    lastCombatAction = 0;
    inventorySize = 0;
    maxInventorySize = 16;
    equipped.resize(Player::EQUIP_LAST, nullptr);
	prompt = true;
	editState = ED_NONE;
	editData = nullptr;
	group = nullptr;
	position = Player::POSITION_STANDING;
	globalCooldown = 0;
	death_timer = Player::DEFAULT_DEATH_TIME;
	death_timer_runback = Player::DEFAULT_DEATH_TIME_RUNBACK;
	graveyard_room = corpse_room = 0;
	statPoints = 0;
	recall = 0;
	prompt = true;
	agility = intellect = strength = stamina = wisdom = spirit = 5;
	bonus_agility = bonus_intellect = bonus_strength = bonus_stamina = bonus_wisdom = bonus_spirit = 0;
	maxHealth = stamina * Player::HEALTH_FROM_STAMINA;
	maxMana = wisdom * Player::MANA_FROM_WISDOM;
	maxEnergy = 100;
	maxRage = 100;
	comboPointTarget = nullptr;
	lastAutoAttack_off = lastAutoAttack_main = lastCombatAction = 0;
	inventorySize = 0;
	maxInventorySize = 16; 
	currentClass = nullptr;
	comboPoints = 0;
	maxComboPoints = 5;
	isGhost = false;

	health = GetMaxHealth();
	mana = GetMaxMana();
	energy = GetMaxEnergy();
}

Player::~Player()
{

}

void Player::SendBW(std::string str)
{
	user->SendBW(str);
}

void Player::Send(std::string str)
{
	user->Send(str);
}

void Player::Send(char * str)
{
	user->Send(str);
}

void Player::SendGMCP(std::string str)
{
	user->SendGMCP(str);
}

void Player::SendGMCP(char * str)
{
	user->SendGMCP(str);
}

void Player::QueryClearAll()
{
    queryList.clear();
}

void Player::QueryClear(bool(*whichFunc)(Player *, std::string))
{
    for (auto iter = begin(queryList); iter != end(queryList); ++iter)
    {
        if (iter->queryFunction == whichFunc)
        {
            queryList.erase(iter);
            return;
        }
    }
}

void Player::AddQuery(std::string prompt, void * data, bool(*func)(Player *, std::string))
{
    Query q;
    q.queryDataPtr = data;
    q.queryFunction = func;
    q.queryPrompt = prompt;
    queryList.push_back(q);
}

void Player::AddQuery(std::string prompt, int data, bool(*func)(Player *, std::string))
{
    Query q;
    q.queryDataInt = data;
    q.queryFunction = func;
    q.queryPrompt = prompt;
    queryList.push_back(q);
}

//Couldn't figure out how to pass a function pointer through Lua so needed a Lua specific version
void Player::LuaAddQuery(std::string prompt, sol::userdata * data, std::string whichQuery)
{
    if (whichQuery == "PlayerResQuery")
    {
        Query q;
        q.queryDataPtr = data;
        q.queryFunction = acceptPlayerRes;
        q.queryPrompt = prompt;
        queryList.push_back(q);
    }
}

void * Player::GetQueryDataPtr(bool(*whichFunc)(Player *, std::string))
{
    for (auto iter = begin(queryList); iter != end(queryList); ++iter)
    {
        if (iter->queryFunction == whichFunc)
            return iter->queryDataPtr;
    }
    return nullptr;
}

int Player::GetQueryDataInt(bool(*whichFunc)(Player *, std::string))
{
    for (auto iter = begin(queryList); iter != end(queryList); ++iter)
    {
        if (iter->queryFunction == whichFunc)
            return iter->queryDataInt;
    }
    return -1;
}

bool Player::HasQuery()
{
    if (!queryList.empty())
        return true;
    return false;
}

bool Player::HasQuery(bool(*whichFunc)(Player *, std::string))
{
    for (auto iter = begin(queryList); iter != end(queryList); ++iter)
    {
        if(iter->queryFunction == whichFunc)
            return true;
    }
	return false;
}

bool(*Player::GetQueryFunc(int queryindex))(Player *, std::string)
{
    int ctr = 0;
    for (auto iter = begin(queryList); iter != end(queryList); ++iter)
    {
        if (ctr == queryindex)
            return iter->queryFunction;
        ctr++;
    }
    return nullptr;
}

void Player::AddSkill(Skill * newskill)
{
	if (newskill == nullptr)
		return;
	knownSkills[newskill->name] = newskill;
}

void Player::RemoveSkill(Skill * sk)
{
	std::map<string, Skill*>::iterator iter;

	iter = knownSkills.find(sk->name);
	if (iter != knownSkills.end() && (*iter).second->id == sk->id)
	{
		knownSkills.erase(iter);
	}
}

void Player::RemoveSkill(string name)
{
	std::map<string, Skill*>::iterator iter;

	iter = knownSkills.find(name);
	if (iter != knownSkills.end())
	{
		knownSkills.erase(iter);
	}
}

Skill * Player::GetSkillShortName(string name)
{
	std::map<string, Skill*>::iterator iter;

	name = Utilities::ToLower(name);
	iter = knownSkills.find(name);
	if (iter != knownSkills.end())
	{
		return (*iter).second;
	}
	return nullptr;
}

bool Player::HasSkill(Skill * sk)
{
	std::map<string, Skill*>::iterator iter;

	iter = knownSkills.find(sk->name);
	if (iter != knownSkills.end() && (*iter).second->id == sk->id)
	{
		return true;
	}
	return false;
}

bool Player::HasSkillByName(string name) //Not guaranteed to be the same skill id, just the same name
{
	std::map<string, Skill*>::iterator iter;

	iter = knownSkills.find(name);
	if (iter != knownSkills.end())
	{
		return true;
	}
	return false;
}

void Player::ResetMaxStats()
{
	SetMaxHealth(GetStamina() * Player::HEALTH_FROM_STAMINA);
	SetMaxMana(GetWisdom() * Player::MANA_FROM_WISDOM);
	if (GetHealth() > GetMaxHealth())
		SetHealth(GetMaxHealth());
	if (GetMana() > GetMaxMana())
		SetMana(GetMaxMana());
	//todo: these might be higher based on skills or talents?
	SetMaxEnergy(100);
	SetMaxRage(100);
	maxComboPoints = 5;
}

void Player::AddEquipmentStats(Item * add)
{
	bonus_agility += add->agility;
	bonus_intellect += add->intellect;
	bonus_strength += add->strength;
	bonus_stamina += add->stamina;
	bonus_wisdom += add->wisdom;
	bonus_spirit += add->spirit;
	armor += add->armor;
	ResetMaxStats();
}

void Player::RemoveEquipmentStats(Item * remove)
{
	bonus_agility -= remove->agility;
	bonus_intellect -= remove->intellect;
	bonus_strength -= remove->strength;
	bonus_stamina -= remove->stamina;
	bonus_wisdom -= remove->wisdom;
	bonus_spirit -= remove->spirit;
	armor -= remove->armor;

	if (bonus_agility < 0) bonus_agility = 0;
	if (bonus_intellect < 0) bonus_intellect = 0;
	if (bonus_strength < 0) bonus_strength = 0;
	if (bonus_stamina < 0) bonus_stamina = 0;
	if (bonus_wisdom < 0) bonus_wisdom = 0;
	if (bonus_spirit < 0) bonus_spirit = 0;
	if (armor < 0) armor = 0;

	ResetMaxStats();
}

int Player::GetAgility() 
{ 
    if (agility + bonus_agility + GetAuraModifier(SpellAffect::AURA_MODIFY_AGILITY, 1) < 0)
    {
        return 0;
    }
    return agility + bonus_agility + GetAuraModifier(SpellAffect::AURA_MODIFY_AGILITY, 1);
}

int Player::GetIntellect()
{ 
    if (intellect + bonus_intellect + GetAuraModifier(SpellAffect::AURA_MODIFY_INTELLECT, 1) < 0)
    {
        return 0;
    }
    return intellect + bonus_intellect + GetAuraModifier(SpellAffect::AURA_MODIFY_INTELLECT, 1);
}

int Player::GetStrength() 
{ 
    if (strength + bonus_strength + GetAuraModifier(SpellAffect::AURA_MODIFY_STRENGTH, 1) < 0)
    {
        return 0;
    }
    return strength + bonus_strength + GetAuraModifier(SpellAffect::AURA_MODIFY_STRENGTH, 1);
}

int Player::GetStamina() 
{ 
    if (stamina + bonus_stamina + GetAuraModifier(SpellAffect::AURA_MODIFY_STAMINA, 1) < 0)
    {
        return 0;
    }
    return stamina + bonus_stamina + GetAuraModifier(SpellAffect::AURA_MODIFY_STAMINA, 1);
}

int Player::GetWisdom() 
{ 
    if (wisdom + bonus_wisdom + GetAuraModifier(SpellAffect::AURA_MODIFY_WISDOM, 1) < 0)
    {
        return 0;
    }
    return wisdom + bonus_wisdom + GetAuraModifier(SpellAffect::AURA_MODIFY_WISDOM, 1);
}

int Player::GetSpirit() 
{ 
    if (spirit + bonus_spirit + GetAuraModifier(SpellAffect::AURA_MODIFY_SPIRIT, 1) < 0)
    {
        return 0;
    }
    return spirit + bonus_spirit + GetAuraModifier(SpellAffect::AURA_MODIFY_SPIRIT, 1);
}

void Player::GeneratePrompt(double currentTime)
{
	string prompt = "\r\n";

	if (editState != ED_NONE)
	{
		switch (editState)
		{
		case ED_ROOM:
			prompt += "|G(Room)|X";
			break;
		case ED_SKILL:
			prompt += "|G(Skill)|X";
			break;
		case ED_PLAYER:
			prompt += "|G(Player)|X";
			break;
		case ED_NPC:
			prompt += "|G(NPC)|X";
			break;
		case ED_ITEM:
			prompt += "|G(Item)|X";
			break;
		case ED_QUEST:
			prompt += "|G(Quest)|X";
			break;
		case ED_HELP:
			prompt += "|G(Help)|X";
			break;
		case ED_AREA:
			prompt += "|G(Area)|X";
			break;
		case ED_CLASS:
			prompt += "|G(Class)|X";
			break;
		default:
			prompt += "|G(ED_?)|X";
			break;
		}
	}
	prompt += "|B<";

	int percent;
	string statColor;
	if (!IsGhost())
	{
		//Health
		if (GetHealth() > 0 && GetMaxHealth() > 0)
			percent = (GetHealth() * 100) / GetMaxHealth();
		else
			percent = 0;

		if (percent >= 75)
			statColor = "|x";
		else if (percent >= 50)
			statColor = "|G";
		else if (percent >= 25)
			statColor = "|Y";
		else
			statColor = "|R";

		prompt += statColor + Utilities::itos(health) + "/|X" + Utilities::itos(maxHealth) + "|Bh ";

		//Mana
		if (mana > 0 && maxMana > 0)
			percent = (mana * 100) / maxMana;
		else
			percent = 0;

		if (percent >= 75)
			statColor = "|x";
		else if (percent >= 50)
			statColor = "|G";
		else if (percent >= 25)
			statColor = "|Y";
		else
			statColor = "|R";

		prompt += statColor + Utilities::itos(mana) + "/|X" + Utilities::itos(maxMana) + "|Bm ";

		//Energy
		prompt += "|X" + Utilities::itos(energy) + "/" + Utilities::itos(maxEnergy) + "|Be ";

		//Rage
		prompt += "|X" + Utilities::itos(rage) + "/|X" + Utilities::itos(maxRage) + "|Br";
	}
	else
	{
		prompt += "|xGhost";
	}
	prompt += "|B>|x";

	//Combat flag
	if (combat)
	{
		prompt += "|B<|RX|B>|X";
	}
	//Cast timer
	if (HasActiveDelay())
	{
		double timeleft = delay - currentTime;
		prompt += "|Y[" + Utilities::dtos(timeleft, 1) + "s]|X";
	}

	//Target
	if (GetTarget() != nullptr)
	{
		int jsonvitals[5]; //level, health, mana, energy, rage as %'s
		string targetPrompt = "";

		//Combo points
		if (comboPoints > 0 && GetTarget() == comboPointTarget)
		{
			targetPrompt += "|R(" + Utilities::itos(comboPoints) + ")|X";
			json combos = { { "points", comboPoints } };
			SendGMCP("target.combo " + combos.dump());
		}
		else if (GetTarget() != comboPointTarget)
		{
			json combos = { { "points", 0 } };
			SendGMCP("target.combo " + combos.dump());
		}

		string targetLevel = Game::LevelDifficultyColor(Game::LevelDifficulty(level, GetTarget()->GetLevel()));
		if (!GetTarget()->FlagIsSet(NPCIndex::FLAG_FRIENDLY)
			&& Game::LevelDifficulty(level, GetTarget()->GetLevel()) == 5)
		{
			targetLevel += "??";
			jsonvitals[0] = 0;
		}
		else
		{
			targetLevel += Utilities::itos(GetTarget()->GetLevel());
			jsonvitals[0] = GetTarget()->GetLevel();
		}

		targetPrompt += "|B<" + targetLevel + " ";
		if (GetTarget() == this || GetTarget()->FlagIsSet(NPCIndex::FLAG_FRIENDLY))
			targetPrompt += "|G";
		else if (GetTarget()->IsPlayer())
			targetPrompt += "|C";
		else if (GetTarget()->FlagIsSet(NPCIndex::FLAG_NEUTRAL))
			targetPrompt += "|Y";
		else
			targetPrompt += "|R";
		targetPrompt += GetTarget()->GetName() + "|X ";

		if (!GetTarget()->IsCorpse())
		{
			//Health
			if (GetTarget()->GetHealth() > 0 && GetTarget()->GetMaxHealth() > 0)
				percent = (GetTarget()->GetHealth() * 100) / GetTarget()->GetMaxHealth();
			else
				percent = 0;

			if (percent >= 75 || GetTarget()->GetMaxHealth() == 0)
				statColor = "|x";
			else if (percent >= 50)
				statColor = "|G";
			else if (percent >= 25)
				statColor = "|Y";
			else
				statColor = "|R";

			targetPrompt += statColor + Utilities::itos(percent) + "|B%h ";
			jsonvitals[1] = percent;

			//Mana
			if (GetTarget()->GetMana() > 0 && GetTarget()->GetMaxMana() > 0)
				percent = (GetTarget()->GetMana() * 100) / GetTarget()->GetMaxMana();
			else
				percent = 0;

			if (percent >= 75 || GetTarget()->GetMaxMana() == 0)
				statColor = "|x";
			else if (percent >= 50)
				statColor = "|G";
			else if (percent >= 25)
				statColor = "|Y";
			else
				statColor = "|R";

			targetPrompt += statColor + Utilities::itos(percent) + "|B%m ";
			jsonvitals[2] = percent;

			//Energy
			if (GetTarget()->GetEnergy() > 0 && GetTarget()->GetMaxEnergy() > 0)
				percent = (GetTarget()->GetEnergy() * 100) / GetTarget()->GetMaxEnergy();
			else
				percent = 0;
			targetPrompt += "|X" + Utilities::itos(percent) + "|B%e ";
			jsonvitals[3] = percent;

			//Rage
			if (GetTarget()->GetRage() > 0 && GetTarget()->GetMaxRage() > 0)
				percent = (GetTarget()->GetRage() * 100) / GetTarget()->GetMaxRage();
			else
				percent = 0;
			targetPrompt += "|X" + Utilities::itos(percent) + "|B%r>|X";
			jsonvitals[4] = percent;
		}
		else
		{
			targetPrompt += "Corpse|B>|X";
			jsonvitals[1] = 0;
			jsonvitals[2] = 0;
			jsonvitals[3] = 0;
			jsonvitals[4] = 0;
		}
		prompt += targetPrompt;
		json vitals = { { "name", GetTarget()->GetName() },{ "level", jsonvitals[0] },{ "hppercent", jsonvitals[1] },{ "mppercent", jsonvitals[2] },
		{ "enpercent", jsonvitals[3] },{ "ragepercent", jsonvitals[4] } };
		SendGMCP("target.vitals " + vitals.dump());
	}

	//Target of target (changed to display name, level, health only)
	if (GetTarget() != nullptr && GetTarget()->GetTarget() != nullptr)
	{
		Character * targettarget = GetTarget()->GetTarget();
		string gmcplevel = "";
		string targetLevel = Game::LevelDifficultyColor(Game::LevelDifficulty(level, targettarget->GetLevel()));
		if (Game::LevelDifficulty(level, targettarget->GetLevel()) == 5)
		{
			targetLevel += "??";
			gmcplevel = "??";
		}
		else
		{
			targetLevel += Utilities::itos(targettarget->GetLevel());
			gmcplevel = Utilities::itos(targettarget->GetLevel());
		}

		//TODO: Target name coloring based on pvp/attack status
		string targetPrompt = "|B<" + targetLevel + " ";
		if (targettarget == this || targettarget->FlagIsSet(NPCIndex::FLAG_FRIENDLY))
			targetPrompt += "|G";
		else if (targettarget->IsPlayer())
			targetPrompt += "|C";
		else if (targettarget->FlagIsSet(NPCIndex::FLAG_NEUTRAL))
			targetPrompt += "|Y";
		else
			targetPrompt += "|R";
		targetPrompt += targettarget->GetName() + "|X ";

		//Health
		if (targettarget->GetHealth() > 0 && targettarget->GetMaxHealth() > 0)
			percent = (targettarget->GetHealth() * 100) / targettarget->GetMaxHealth();
		else
			percent = 0;

		if (percent >= 75 || targettarget->GetMaxHealth() == 0)
			statColor = "|x";
		else if (percent >= 50)
			statColor = "|G";
		else if (percent >= 25)
			statColor = "|Y";
		else
			statColor = "|R";

		targetPrompt += statColor + Utilities::itos(percent) + "|B%h>|X";

		prompt += targetPrompt;

		json targettargetvitals = { { "name", targettarget->GetName() },{ "level", gmcplevel },{ "hppercent", percent } };
		SendGMCP("targettarget.vitals " + targettargetvitals.dump());
	}

	if (HasGroup())
	{
		int firstingroup = group->FindFirstSlotInSubgroup(this);
		if (group->GetSubgroupCount(firstingroup) > 1)
		{
			prompt += "\r\n";
			int gmcpslot = 1;
			for (int i = 0; i < Group::MAX_GROUP_SIZE; i++)
			{
				Character * current_member = group->GetMember(firstingroup + i);
				if (current_member && current_member != this)
				{
					prompt += "|B<";

					if (current_member->InCombat())
						prompt += "|R(X)|X";
					if (current_member == group->leader)
						prompt += "|Y*|X";

					prompt += "|C" + current_member->GetName() + "|X ";

					//Health
					int percent;
					string statColor;
					if (current_member->GetHealth() > 0 && current_member->GetMaxHealth() > 0)
						percent = (current_member->GetHealth() * 100) / current_member->GetMaxHealth();
					else
						percent = 0;

					if (percent >= 75)
						statColor = "|x";
					else if (percent >= 50)
						statColor = "|G";
					else if (percent >= 25)
						statColor = "|Y";
					else
						statColor = "|R";

					prompt += statColor + Utilities::itos(current_member->GetHealth()) + "|X/" + Utilities::itos(current_member->GetMaxHealth());
					prompt += "|B>|X";

					json vitals = { { "name", current_member->GetName() },{ "combat", current_member->InCombat() ? 1 : 0 },
					{ "hp", current_member->GetHealth() },{ "hpmax", current_member->GetMaxHealth() },
					{ "mp", current_member->GetMana() },{ "mpmax", current_member->GetMaxMana() } };
					SendGMCP("group.vitals " + vitals.dump());
				}
			}
		}
	}

	//todo: We could block the entire function at the top, but then we lose a lot of GMCP data. We need to structure this differently
	if (this->prompt)
	{
		prompt += "\r\n";
		Send(prompt);
	}

	if (HasQuery())
	{
        for (auto iter = begin(queryList); iter != end(queryList); ++iter)
        {
            Send(iter->queryPrompt + "\r\n");
        }
	}
}

void Player::SetMaxHealth(int amount)
{
	maxHealth = amount;

	json vitals = { { "hpmax", maxHealth } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxHealth > 0)
		percent = (GetHealth() * 100) / GetMaxHealth();
	vitals = { { "hppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}

void Player::SetMaxMana(int amount)
{
	maxMana = amount;

	json vitals = { { "mpmax", maxMana } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxMana > 0)
		percent = (GetMana() * 100) / GetMaxMana();
	vitals = { { "mppercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}

void Player::SetMaxEnergy(int amount)
{
	maxEnergy = amount;

	json vitals = { { "enmax", maxEnergy } };
	SendGMCP("char.vitals " + vitals.dump());

	int percent = 0;
	if (maxEnergy > 0)
		percent = (GetEnergy() * 100) / GetMaxEnergy();
	vitals = { { "enpercent", percent } };
	SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
}

void Player::SetMaxRage(int amount)
{
	maxRage = amount;

	if (IsPlayer())
	{
		json vitals = { { "ragemax", maxRage } };
		SendGMCP("char.vitals " + vitals.dump());

		int percent = 0;
		if (maxRage > 0)
			percent = (GetRage() * 100) / GetMaxRage();
		vitals = { { "ragepercent", percent } };
		SendTargetSubscriberGMCP("target.vitals " + vitals.dump());
	}
}

int Player::GetLevel()
{
	return level;
}

bool Player::HasGroup()
{
	if (group != nullptr)
		return true;
	return false;
}

void Player::Stand()
{
	if (position == Position::POSITION_STANDING)
		return;

	bool removed = false;
	while (RemoveSpellAffectByAura(false, SpellAffect::Auras::AURA_EATING)) //true if we removed a spell affect
	{
	}
    while (RemoveSpellAffectByAura(false, SpellAffect::Auras::AURA_DRINKING)) //true if we removed a spell affect
    {
    }
	Send("You stand up.\r\n");
    Message(GetName() + " stands up.", Character::MSG_ROOM_NOTCHAR);
	position = Position::POSITION_STANDING;
}

void Player::Sit()
{
	if (position == Position::POSITION_SITTING)
		return;

	Send("You sit down.\r\n");
    Message(GetName() + " sits down.", Character::MSG_ROOM_NOTCHAR);
	position = Position::POSITION_SITTING;
}

void Player::SetExperience(int newexp)
{
    if(newexp < 0)
        return;

    if(newexp > Game::ExperienceForLevel(Game::MAX_LEVEL))
    {
        experience = Game::ExperienceForLevel(Game::MAX_LEVEL);
        return;
    }
    experience = newexp;
}

bool Player::QuestEligible(Quest * quest)
{
    //check level
    if(user && user->character && user->character->level < quest->levelRequirement)
    {
        return false;
    }

    //check if we've already accepted this quest
    if(QuestActive(quest))
        return false;

    //check if we've done it already
    if(QuestCompleted(quest))
        return false;

    //check if we have any prereq
    if(quest->questRequirement != 0)
    {
        Quest * prereq = Game::GetGame()->GetQuest(quest->questRequirement);
        if(prereq != nullptr)
        {
            if(!QuestCompleted(prereq))
                return false;
        }
    }

    //check if we're on a quest that WOULD BE restricted if we accepted this quest
    std::vector<Quest *>::iterator questiter;
    for(questiter = questLog.begin(); questiter != questLog.end(); ++questiter)
    {
        if((*questiter)->questRestriction == quest->id)
            return false;
    }

    //check if we've completed a quest that restricts this quest
    if(quest->questRestriction != 0)
    {
        Quest * restrict = Game::GetGame()->GetQuest(quest->questRestriction);
        if(restrict != nullptr)
        {
            if(QuestActive(restrict) || QuestCompleted(restrict))
                return false;
        }
    }

    return true;
}

bool Player::QuestActive(Quest * quest)
{
    std::vector<Quest *>::iterator questiter;
    for(questiter = questLog.begin(); questiter != questLog.end(); ++questiter)
    {
        if((*questiter)->id == quest->id)
            return true;
    }
    return false;
}

bool Player::QuestCompleted(Quest * quest)
{
    std::set<int>::iterator qmapiter;
    qmapiter = completedQuests.find(quest->id);
    if(qmapiter != completedQuests.end())
        return true;
    return false;
}

void Player::QuestCompleteObjective(int type, void * obj)
{
    if(questLog.empty())
        return;

    std::vector<Quest *>::iterator logiter;
    int i = 0;
    for(logiter = questLog.begin(); logiter != questLog.end(); ++logiter)
    {
        Quest * q = (*logiter);

        std::vector<Quest::QuestObjective>::iterator objiter;
        int j = 0;
        for(objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
        {
            if(questObjectives[i][j] < (*objiter).count && (*objiter).type == type)
            {
                switch(type)
                {
                    case Quest::OBJECTIVE_ITEM: 
                    {
                        Item * founditem = (Item*)obj;
                        Item * questitem = (Item*)((*objiter).objective);
                        if(founditem && questitem && founditem->GetID() == questitem->GetID())
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\r\n");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_KILLNPC: 
                    {
                        NPC * npc = (NPC*)obj;
                        NPCIndex * questnpc = (NPCIndex*)((*objiter).objective);
                        if(npc && questnpc && npc->GetIndexID() == questnpc->id)
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\r\n\r\n");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_ROOM: 
                    {
                        Room * inroom = (Room*)obj;
                        Room * questroom = (Room*)((*objiter).objective);
                        if(inroom && questroom && inroom->id == questroom->id)
                        {
                            questObjectives[i][j]++;
                            user->Send("|W" + q->name + ": ");
                            user->Send("|Y" + (*objiter).description + " (" + Utilities::itos(questObjectives[i][j]) 
                                       + "/" + Utilities::itos((*objiter).count) + ")|X\r\n\r\n");
                        }
                        break;
                    }

                    case Quest::OBJECTIVE_VISITNPC: 
                        break;
                }                
            }
            j++;
        }
        i++;
    }
}

bool Player::ShouldDropQuestItem(Item * founditem)
{
	if (questLog.empty())
		return false;

	int i = 0;
	for (auto logiter = questLog.begin(); logiter != questLog.end(); ++logiter)
	{
		Quest * q = (*logiter);

		int j = 0;
		for (auto objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
		{
			if (questObjectives[i][j] < (*objiter).count && (*objiter).type == Quest::OBJECTIVE_ITEM)
			{
				Item * questitem = (Item*)((*objiter).objective);
				if (founditem && questitem && founditem->GetID() == questitem->GetID())
				{
					return true;
				}
				break;
			}
			j++;
		}
		i++;
	}
	return false;
}

bool Player::QuestObjectivesComplete(Quest * quest)
{
    if(questLog.empty())
        return false;

    std::vector<Quest *>::iterator logiter;
    int i = 0;
    for(logiter = questLog.begin(); logiter != questLog.end(); ++logiter)
    {
        Quest * q = (*logiter);

        if(q->id == quest->id)
        {
            std::vector<Quest::QuestObjective>::iterator objiter;
            int j = 0;
            for(objiter = q->objectives.begin(); objiter != q->objectives.end(); ++objiter)
            {
                if(questObjectives[i][j] < (*objiter).count)
                {
                    return false;
                }
                j++;
            }
            return true;
        }
        i++;
    }
    return false;
}

void Player::AddItemInventory(Item * item)
{
    if (IsInventoryFull() && !IsImmortal())
    {
        //Allow the item and print error... There are cases we might want to allow this. Do this check higher up when needed
        LogFile::Log("error", "Player::AddItemInventory, added item to full inventory: " + GetName() + "::" + item->GetName());
    }

	QuestCompleteObjective(Quest::OBJECTIVE_ITEM, (void*)item);
	for (auto iter = inventory.begin(); iter != inventory.end(); iter++)
	{
		if (iter->first->GetID() == item->GetID())
		{
			iter->second++;
            return;
		}
	}
    inventory.push_front(std::make_pair(item, 1));
    inventorySize++;
}

Item * Player::GetItemInventory(int id)
{
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(iter->first->GetID() == id)
        {
			return iter->first;
        }
    }
    return nullptr;
}

Item * Player::GetItemInventory(string name)
{
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(Utilities::IsName(name, iter->first->GetName()))
        {
            if(ctr == numberarg)
                return iter->first;
            ctr++;
        }
    }
    return nullptr;
}

Item * Player::RemoveItemInventory(int id)
{
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(iter->first->GetID() == id)
        {
			Item * ret = iter->first;
			iter->second--;
			if (iter->second == 0)
			{
				inventory.erase(iter);
				inventorySize--;
			}
            return ret;
        }
    }
    return nullptr;
}

Item * Player::RemoveItemInventory(string name)
{
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(Utilities::IsName(name, iter->first->GetName()))
        {
            if(ctr == numberarg)
            {
				Item * ret = iter->first;
				iter->second--;
				if (iter->second == 0)
				{
					inventory.erase(iter);
					inventorySize--;
				}
				return ret;
            }
            ctr++;
        }
    }
    return nullptr;
}

bool Player::RemoveItemInventory(Item * item)
{
    for(auto iter = inventory.begin(); iter != inventory.end(); ++iter)
    {
        if(iter->first == item)
        {
			iter->second--;
			if (iter->second == 0)
			{
				inventory.erase(iter);
				inventorySize--;
			}
            return true;
        }
    }
    return false;
}

int Player::GetEquippedItemIndex(string name)
{
    for(int i = 0; i < Player::EQUIP_LAST; i++)
    {
        if(equipped[i] && Utilities::IsName(name, equipped[i]->GetName()))
        {
            //return equipped[i];
            return i;
        }
    }
    return Player::EQUIP_LAST;
}

Item * Player::GetItemEquipped(string name)
{
    for(int i = 0; i < Player::EQUIP_LAST; i++)
    {
        if(equipped[i] && Utilities::IsName(name, equipped[i]->GetName()))
        {
            return equipped[i];
        }
    }
    return nullptr;
}

Item * Player::RemoveItemEquipped(int index)
{
    Item * remove = equipped[index];
    equipped[index] = nullptr;
    return remove;
}

bool Player::EquipItemFromInventory(Item * wear)
{
	int equipSlot = GetEquipLocation(wear);
	if (equipSlot < 0 || equipSlot >= Player::EQUIP_LAST)
	{
		LogFile::Log("error", "EquipItemFromInventory, invalid equipSlot");
		return false;
	}

	if (equipped[equipSlot] != nullptr)
	{
		LogFile::Log("error", "EquipItemFromInventory, equipped[equipSlot] != nullptr");
		return false;
	}
	RemoveItemInventory(wear);
	equipped[equipSlot] = wear;
	return true;
}

bool Player::EquipItem(Item * wear)
{
	int equipSlot = GetEquipLocation(wear);

	if (equipSlot < 0 || equipSlot >= Player::EQUIP_LAST)
	{
		LogFile::Log("error", "EquipItem, invalid equipSlot");
		return false;
	}

	if (equipped[equipSlot] != nullptr)
	{
		LogFile::Log("error", "EquipItem, equipped[equipSlot] != nullptr");
		return false;
	}
	equipped[equipSlot] = wear;
	return true;
}

//return the equip location for this item given what the player is currently wearing
// unfortunately different from equip->equipLocation, especially for weapons, double slot items (ring trinket)
int Player::GetEquipLocation(Item * equip)
{
    int equipSlot = Player::EQUIP_LAST;
    switch(equip->equipLocation)
    {
        case Item::EQUIP_BACK:
        case Item::EQUIP_CHEST:
        case Item::EQUIP_FEET:
        case Item::EQUIP_HANDS:
        case Item::EQUIP_HEAD:
        case Item::EQUIP_LEGS:
        case Item::EQUIP_NECK:
        case Item::EQUIP_WAIST:
        case Item::EQUIP_WRIST:
        case Item::EQUIP_SHOULDER:
            equipSlot = equip->equipLocation - 1; //If we change Item::EquipLocation or Player::EquipmentSlot, we'll be back here when stuff goes weird 
            break;
        case Item::EQUIP_FINGER:
            if(equipped[Player::EQUIP_FINGER1] == nullptr)
                equipSlot = Player::EQUIP_FINGER1;
            else if(equipped[Player::EQUIP_FINGER2] == nullptr)
                equipSlot = Player::EQUIP_FINGER2;
            else
                equipSlot = Player::EQUIP_FINGER1;
            break;
        case Item::EQUIP_TRINKET:
            if(equipped[Player::EQUIP_TRINKET1] == nullptr)
                equipSlot = Player::EQUIP_TRINKET1;
            else if(equipped[Player::EQUIP_TRINKET2] == nullptr)
                equipSlot = Player::EQUIP_TRINKET2;
            else
                equipSlot = Player::EQUIP_TRINKET1;
            break;
        case Item::EQUIP_MAINHAND:
            equipSlot = Player::EQUIP_MAINHAND;
            break;
        case Item::EQUIP_ONEHAND:
            if(equipped[Player::EQUIP_MAINHAND] != nullptr && equipped[Player::EQUIP_MAINHAND]->equipLocation != Item::EQUIP_TWOHAND)
                equipSlot = Player::EQUIP_OFFHAND;
            else
                equipSlot = Player::EQUIP_MAINHAND;
            break;
        case Item::EQUIP_OFFHAND: 
            equipSlot = Player::EQUIP_OFFHAND;
            break;
        case Item::EQUIP_TWOHAND:
            equipSlot = Player::EQUIP_MAINHAND;
            break;
    }

    /*if(equipSlot == Player::EQUIP_LAST)
    {
        LogFile::Log("error", "Player::EquipItemFromInventory, bad wear_location");
    }*/

    return equipSlot;
}

void Player::AddClass(int id, int level)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == id && (*iter).level < level)
        {
            (*iter).level = level;
            return;
        }
    }
    ClassData cd;
    cd.id = id;
    cd.level = level;
    classList.push_back(cd);
}

int Player::GetClassLevel(int classid)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == classid)
        {
            return (*iter).level;
        }
    }
    return 0;
}

void Player::SetClassLevel(int classid, int newlevel)
{
    std::list<ClassData>::iterator iter;
    for(iter = classList.begin(); iter != classList.end(); ++iter)
    {
        if((*iter).id == classid)
        {
            (*iter).level = newlevel;
            return;
        }
    }
    AddClass(classid, newlevel);
}

void Player::SetGhost()
{
	isCorpse = false;
	isGhost = true;
}

void Player::SetCorpse()
{
	Character::SetCorpse();
	UnsetGhost();
	death_timer = Player::DEFAULT_DEATH_TIME;
	death_timer_runback = Player::DEFAULT_DEATH_TIME_RUNBACK;
}

void Player::SetAlive()
{
	Character::SetAlive();
	UnsetGhost();
}

void Player::UnsetGhost()
{
	isGhost = false;
}

bool Player::IsAlive()
{
	if(!IsCorpse() && !IsGhost())
		return true;
	return false;
}

void Player::SetResurrectTime(int seconds)
{
	death_timer = seconds;
	death_timer_runback = seconds;
}

bool Player::CanResAtCorpse(int time_since_death)
{
	return time_since_death >= death_timer_runback;
}

bool Player::CanRes(int time_since_death)
{
	return time_since_death >= death_timer;
}

bool Player::CanWearArmor(int armortype)
{
	if (armortype < Item::TYPE_ARMOR_CLOTH || armortype > Item::TYPE_ARMOR_PLATE)
		return true;

	std::list<Player::ClassData>::iterator iter;
	for (iter = classList.begin(); iter != classList.end(); ++iter)
	{
		Class * curr = Game::GetGame()->GetClass(iter->id);
		int armorlevel = curr->GetArmorLevel(armortype);

		if (curr && armorlevel != 0 && armorlevel <= GetLevel())
			return true;
	}
	return false;
}

std::stringstream Player::FormatEquipment()
{
	std::stringstream equipment;

	equipment << std::left << std::setw(15) << "<|BHead|X>";
	equipped[Player::EQUIP_HEAD] ? equipment << equipped[Player::EQUIP_HEAD]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BNeck|X>";
	equipped[Player::EQUIP_NECK] ? equipment << equipped[Player::EQUIP_NECK]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BShoulder|X>";
	equipped[Player::EQUIP_SHOULDER] ? equipment << equipped[Player::EQUIP_SHOULDER]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BBack|X>";
	equipped[Player::EQUIP_BACK] ? equipment << equipped[Player::EQUIP_BACK]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BChest|X>";
	equipped[Player::EQUIP_CHEST] ? equipment << equipped[Player::EQUIP_CHEST]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BWrist|X>";
	equipped[Player::EQUIP_WRIST] ? equipment << equipped[Player::EQUIP_WRIST]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BHands|X>";
	equipped[Player::EQUIP_HANDS] ? equipment << equipped[Player::EQUIP_HANDS]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BWaist|X>";
	equipped[Player::EQUIP_WAIST] ? equipment << equipped[Player::EQUIP_WAIST]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BLegs|X>";
	equipped[Player::EQUIP_LEGS] ? equipment << equipped[Player::EQUIP_LEGS]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BFeet|X>";
	equipped[Player::EQUIP_FEET] ? equipment << equipped[Player::EQUIP_FEET]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BFinger|X>";
	equipped[Player::EQUIP_FINGER1] ? equipment << equipped[Player::EQUIP_FINGER1]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BFinger|X>";
	equipped[Player::EQUIP_FINGER2] ? equipment << equipped[Player::EQUIP_FINGER2]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BTrinket|X>";
	equipped[Player::EQUIP_TRINKET1] ? equipment << equipped[Player::EQUIP_TRINKET1]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BTrinket|X>";
	equipped[Player::EQUIP_TRINKET2] ? equipment << equipped[Player::EQUIP_TRINKET2]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BOffhand|X>";
	equipped[Player::EQUIP_OFFHAND] ? equipment << equipped[Player::EQUIP_OFFHAND]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << std::setw(15) << "<|BMainhand|X>";
	equipped[Player::EQUIP_MAINHAND] ? equipment << equipped[Player::EQUIP_MAINHAND]->GetColoredName() << "|X\r\n" : equipment << "None\r\n";

	equipment << "\r\n";

	return equipment;
}

void Player::HandleNPCKillRewards(Character * killed)
{
	int exp = Game::CalculateExperience(this, killed);
    if (exp > 0)
    {
        Send("|BYou have gained |Y" + Utilities::itos(exp) + "|B experience.|X\r\n");
        ApplyExperience(exp);
    }
	QuestCompleteObjective(Quest::OBJECTIVE_KILLNPC, (void*)killed);
}

double Player::GetDodge()
{
	if (GetAgility() * DODGE_FROM_AGILITY > DODGE_MAX)
		return DODGE_MAX;
	return GetAgility() * DODGE_FROM_AGILITY;
}

double Player::GetCrit()
{
	if (GetAgility() * CRIT_FROM_AGILITY > CRIT_MAX)
		return CRIT_MAX;
	return GetAgility() * CRIT_FROM_AGILITY;
}

double Player::GetParry()
{
	return 5;
}

Player * Player::LoadPlayer(std::string name, User * user)
{
	StoreQueryResult characterres = Server::sqlQueue->Read("select * from players where name='" + name + "'");
	if (characterres.empty())
		return nullptr;

	Row row = *characterres.begin();

	Player * loaded = Game::GetGame()->NewPlayer(name, user);

	loaded->name = row["name"];
	loaded->gender = row["gender"];
	loaded->race = row["race"];
	loaded->title = row["title"];
	loaded->level = row["level"];
	loaded->agility = row["agility"];
	loaded->intellect = row["intellect"];
	loaded->strength = row["strength"];
	loaded->stamina = row["stamina"];
	loaded->wisdom = row["wisdom"];
	loaded->spirit = row["spirit"];
	loaded->health = row["health"];
	loaded->mana = row["mana"];

	loaded->room = Game::GetGame()->GetRoom(row["room"]);

	loaded->currentClass = Game::GetGame()->GetClass(row["class"]);
	loaded->password = row["password"];
	loaded->immlevel = row["immlevel"];
	loaded->experience = row["experience"];
	loaded->recall = row["recall"];
	if (row["ghost"])
	{
		loaded->SetGhost();
		loaded->corpse_room = row["corpse_room"];
		loaded->graveyard_room = row["room"];
		loaded->deathTime = row["ghost"];
		loaded->death_timer = Player::DEFAULT_DEATH_TIME;
		loaded->death_timer_runback = Player::DEFAULT_DEATH_TIME_RUNBACK;
	}
	loaded->statPoints = row["stat_points"];

	StoreQueryResult playerclassres = Server::sqlQueue->Read("SELECT * FROM player_class_data where player='" + loaded->name + "'");
	StoreQueryResult::iterator iter;
	for (iter = playerclassres.begin(); iter != playerclassres.end(); ++iter)
	{
		Row classrow = *iter;
		int id = classrow["class"];
		int level = classrow["level"];
		loaded->AddClass(id, level);
	}

	//Skills saved with a player no longer a thing, determine skills from classes. Still load them for the session with AddSkill
	loaded->AddClassSkills();
	//For imms also add every skill there is...
	if (loaded && loaded->IsImmortal())
	{
		for (auto iter = Game::GetGame()->skills.begin(); iter != Game::GetGame()->skills.end(); ++iter)
		{
			loaded->AddSkill(iter->second);
		}
	}

	StoreQueryResult playerinventoryres = Server::sqlQueue->Read("SELECT * FROM player_inventory where player='" + loaded->name + "'");
	for (iter = playerinventoryres.begin(); iter != playerinventoryres.end(); ++iter)
	{
		Row invrow = *iter;
		int id = invrow["item"];
		int count = invrow["count"];
		int location = invrow["location"]; //Location means equipped, in inventory, in bank...
		switch (location)
		{
		case DB_INVENTORY_EQUIPPED:
		{
			Item * equip = Game::GetGame()->GetItem(id);

            int equiploc = loaded->GetEquipLocation(equip);
            //Can't wear this...even though it was saved while worn. Item must have been changed
            if (equiploc == Player::EQUIP_LAST) 
            {
                continue;
            }
            //Can't wear this...even though it was saved while worn. Item must have been changed
            if ((equip->type == Item::TYPE_ARMOR_CLOTH || equip->type == Item::TYPE_ARMOR_LEATHER
                || equip->type == Item::TYPE_ARMOR_MAIL || equip->type == Item::TYPE_ARMOR_PLATE) && !loaded->CanWearArmor(equip->type))
            {
                continue;
            }
            //If we're trying to equip a mainhand but the slot is already occupied by a onehand, and the offhand is empty, move the onehand to the offhand
            if (equiploc == Player::EQUIP_MAINHAND
                && loaded->equipped[Player::EQUIP_MAINHAND]
                && !loaded->equipped[Player::EQUIP_OFFHAND]
                && loaded->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_ONEHAND)
            {   
                loaded->equipped[Player::EQUIP_OFFHAND] = loaded->equipped[Player::EQUIP_MAINHAND];
                loaded->equipped[Player::EQUIP_MAINHAND] = nullptr;
            }
			if(loaded->EquipItem(equip))
			    loaded->AddEquipmentStats(equip);
			break;
		}

		case DB_INVENTORY_INVENTORY:
		{
			for (int i = 0; i < count; i++)
			{
				loaded->AddItemInventory(Game::GetGame()->GetItem(id));
			}
			break;
		}

		case DB_INVENTORY_BANK:
		{
			//todo
			break;
		}
		}
	}

    //Set these again since wearing our items probably raised our maxes
    loaded->health = row["health"];
    loaded->mana = row["mana"];

	loaded->ResetMaxStats(); //Set maxhealth/mana/energy/rage/combos based on post equipment stats

	StoreQueryResult playerqcres = Server::sqlQueue->Read("SELECT * FROM player_completed_quests where player='" + loaded->name + "'");
	for (iter = playerqcres.begin(); iter != playerqcres.end(); ++iter)
	{
		Row qcrow = *iter;
		int id = qcrow["quest"];
		loaded->completedQuests.insert(id);
	}

	StoreQueryResult playerqares = Server::sqlQueue->Read("SELECT * FROM player_active_quests where player='" + loaded->name + "'");
	int i = 0;
	for (iter = playerqares.begin(); iter != playerqares.end(); ++iter)
	{
		Row qarow = *iter;
		int id = qarow["quest"];
		string objectives = (string)qarow["objectives"];

		Quest * q;
		if ((q = Game::GetGame()->GetQuest(id)) != nullptr)
		{
			loaded->questLog.push_back(q);
			std::vector<int> playerObjectives;
			loaded->questObjectives.push_back(playerObjectives);

			int count;
			int first = 0;
			for (int last = (int)objectives.find(","); last != std::string::npos; last = (int)objectives.find(",", first))
			{
				count = Utilities::atoi(objectives.substr(first, last - first));
				loaded->questObjectives[i].push_back(count);
				first = last + 1;
			}
			count = Utilities::atoi(objectives.substr(first, objectives.length() - first));
			loaded->questObjectives[i].push_back(count);
			i++;
		}
	}

	StoreQueryResult playeraliasres = Server::sqlQueue->Read("SELECT * FROM player_alias where player='" + loaded->name + "'");
	for (iter = playeraliasres.begin(); iter != playeraliasres.end(); ++iter)
	{
		Row aliasrow = *iter;
		string word = (string)aliasrow["word"];
		string substitution = (string)aliasrow["substitution"];
		loaded->alias[word] = substitution;
	}

	return loaded;
}

void Player::Save()
{
	string sql;
		string fixpassword = Utilities::SQLFixQuotes(password);
		string fixtitle = Utilities::SQLFixQuotes(title);

		sql = "INSERT INTO players (name, password, immlevel, title, experience, room, level, gender, race, agility, intellect, strength, stamina, ";
		sql += "wisdom, spirit, health, mana, class, recall, ghost, corpse_room, stat_points) values ('";
		sql += name + "','" + fixpassword + "'," + Utilities::itos(GetImmLevel());
		sql += ",'" + fixtitle + "'," + Utilities::itos(experience) + "," + Utilities::itos(room->id);
		sql += "," + Utilities::itos(level) + "," + Utilities::itos(gender) + "," + Utilities::itos(race) + ",";
		sql += Utilities::itos(agility) + "," + Utilities::itos(intellect) + "," + Utilities::itos(strength) + ",";
		sql += Utilities::itos(stamina) + "," + Utilities::itos(wisdom) + "," + Utilities::itos(spirit) + ",";
		sql += Utilities::itos(health) + "," + Utilities::itos(mana);
		sql += "," + Utilities::itos(currentClass->GetID()) + "," + Utilities::itos(recall) + ", ";
		if (IsGhost() || IsCorpse())
		{
			sql += Utilities::dtos(deathTime, 0) + "," + Utilities::itos(corpse_room) + ",";
		}
		else
		{
			sql += "0,0,";
		}
		sql += Utilities::itos(statPoints) + ")";

		sql += " ON DUPLICATE KEY UPDATE name=VALUES(name), password=VALUES(password), immlevel=VALUES(immlevel), title=VALUES(title), ";
		sql += "experience=VALUES(experience), room=VALUES(room), level=VALUES(level), gender=VALUES(gender), race=VALUES(race), agility=VALUES(agility), ";
		sql += "intellect=VALUES(intellect), strength=VALUES(strength), stamina=VALUES(stamina), wisdom=VALUES(wisdom), spirit=VALUES(spirit), ";
		sql += "health=VALUES(health), mana=VALUES(mana), ";
		sql += "class=VALUES(class), ";
		sql += "recall=VALUES(recall), ghost=VALUES(ghost), corpse_room=VALUES(corpse_room),stat_points=VALUES(stat_points)";

		//player_completed_quests
		std::set<int>::iterator questiter;
		for (questiter = completedQuests.begin(); questiter != completedQuests.end(); ++questiter)
		{
			string qcsql = "INSERT IGNORE INTO player_completed_quests (player, quest) values ";
			qcsql += "('" + name + "', " + Utilities::itos(*questiter) + ")";
			Server::sqlQueue->Write(qcsql);
		}

		//player_active_quests
		Server::sqlQueue->Write("DELETE FROM player_active_quests where player='" + name + "'");
		for (int i = 0; i < (int)questLog.size(); i++)
		{
			string qasql = "INSERT INTO player_active_quests (player, quest, objectives) values ('" + name + "',";
			qasql += Utilities::itos(questLog[i]->id) + ",'";
			for (int j = 0; j < (int)questObjectives[i].size();)
			{
				qasql += Utilities::itos(questObjectives[i][j]);
				j++;
				if (j < (int)questObjectives[i].size())
					qasql += ",";
			}
			qasql += "')";
			Server::sqlQueue->Write(qasql);
		}

		//player_inventory
		Server::sqlQueue->Write("DELETE FROM player_inventory where player='" + name + "'");
		for (int i = 0; i < (int)equipped.size(); i++)
		{
			if (equipped[i] != nullptr)
			{
				string equippedsql = "INSERT INTO player_inventory (player, item, count, location) values ('" + name + "',";
				equippedsql += Utilities::itos(equipped[i]->GetID()) + ",1," + Utilities::itos(DB_INVENTORY_EQUIPPED) + ")";
				Server::sqlQueue->Write(equippedsql);
			}
		}
		for (auto inviter = inventory.begin(); inviter != inventory.end(); ++inviter)
		{
			string equippedsql = "INSERT INTO player_inventory (player, item, count, location) values ('" + name + "',";
			equippedsql += Utilities::itos(inviter->first->GetID()) + "," + Utilities::itos(inviter->second) + "," + Utilities::itos(DB_INVENTORY_INVENTORY) + ")";
			Server::sqlQueue->Write(equippedsql);
		}

		//player_class_data
		Server::sqlQueue->Write("DELETE FROM player_class_data where player='" + name + "'");
		std::list<Player::ClassData>::iterator classiter;
		for (classiter = classList.begin(); classiter != classList.end(); ++classiter)
		{
			string classsql = "INSERT INTO player_class_data (player, class, level) values ('" + name + "',";
			classsql += Utilities::itos((*classiter).id) + "," + Utilities::itos((*classiter).level) + ")";
			Server::sqlQueue->Write(classsql);
		}

		//player aliases
		Server::sqlQueue->Write("DELETE FROM player_alias where player='" + name + "'");
		for (auto aliasiter = alias.begin(); aliasiter != alias.end(); ++aliasiter)
		{
			string aliassql = "INSERT INTO player_alias (player, word, substitution) values ('" + name + "','";
			aliassql += Utilities::SQLFixQuotes(aliasiter->first) + "','" + Utilities::SQLFixQuotes(aliasiter->second) + "')";
			Server::sqlQueue->Write(aliassql);
		}

		saved = (int)Game::GetGame()->currentTime;

	Server::sqlQueue->Write(sql);
}

int Player::AddLootRoll(int corpse_id, NPC * corpse)
{
	LootRoll lr;
	lr.corpse = corpse;
	lr.corpse_id = corpse_id;

	int empty_id = 1;
	for (auto iter = begin(pending_loot_rolls); iter != end(pending_loot_rolls); ++iter)
	{
		if (iter->my_id != empty_id)
			break;
		empty_id++;
	}
	lr.my_id = empty_id;
	pending_loot_rolls.push_back(lr);
	corpse->AddSubscriber(this);
	return empty_id;
}

bool Player::HasLootRoll(Character * corpse, int corpse_id)
{
	for (auto iter = pending_loot_rolls.begin(); iter != pending_loot_rolls.end(); ++iter)
	{
		if (iter->corpse == corpse && iter->corpse_id == corpse_id)
			return true;
	}
	return false;
}

void Player::RemoveLootRoll(int my_id)
{
	for (auto iter = begin(pending_loot_rolls); iter != end(pending_loot_rolls);)
	{
		if (iter->my_id == my_id)
		{
			iter->corpse->RemoveSubscriber(this);
			iter = pending_loot_rolls.erase(iter);
			break;
		}
		else
		{
			++iter;
		}
	}
}

void Player::RemoveLootRoll(Character * corpse)
{
	for (auto iter = begin(pending_loot_rolls); iter != end(pending_loot_rolls);)
	{
		if (iter->corpse == corpse)
		{
			corpse->RemoveSubscriber(this);
			iter = pending_loot_rolls.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

void Player::RemoveLootRoll(Character * corpse, int corpse_id)
{
	for (auto iter = begin(pending_loot_rolls); iter != end(pending_loot_rolls);)
	{
		if (iter->corpse == corpse && iter->corpse_id == corpse_id)
		{
			corpse->RemoveSubscriber(this);
			iter = pending_loot_rolls.erase(iter);
			//break; //a corpse/corpse_id combo should be unique, but continue looping just in case
		}
		else
		{
			++iter;
		}
	}
}

void Player::RemoveAllLootRolls() //for subscriber/subscribermanager considerations
{
	for (auto iter = begin(pending_loot_rolls); iter != end(pending_loot_rolls);)
	{
		iter->corpse->RemoveSubscriber(this);
		iter = pending_loot_rolls.erase(iter);
	}
	pending_loot_rolls.clear();
}

bool Player::IsInventoryFull()
{
    if(inventorySize >= maxInventorySize)
        return true;
    return false;
}

void Player::SetLevel(int newlevel)
{
	if (GetLevel() == newlevel || newlevel > Game::MAX_LEVEL || newlevel < 1)
		return;

	Send("|W***You have reached level " + Utilities::itos(newlevel) + "!***|X\r\n");


		SetClassLevel(currentClass->GetID(),
			Utilities::MAX(0, GetClassLevel(currentClass->GetID()) + (newlevel - level)));

		statPoints += Player::STATS_PER_LEVEL;
		Send("|WYou gain " + Utilities::itos(Player::STATS_PER_LEVEL) + " attribute points. Assign attribute points with the \"train\" command.|X\r\n");

		std::list<Class::SkillData>::iterator newskills;
		for (newskills = currentClass->classSkills.begin(); newskills != currentClass->classSkills.end(); newskills++)
		{
			if (newskills->level == GetClassLevel(currentClass->GetID()) && !HasSkill(newskills->skill)) //Found a new skill to add
			{
				AddSkill(newskills->skill);
				Send("|WYou have learned the skill \"" + newskills->skill->long_name + "\"|X\r\n");
			}
		}
	level = newlevel;
	SetHealth(maxHealth);
	SetMana(maxMana);
	SetEnergy(maxEnergy);
}

void Player::SaveCooldowns()
{
	string sql = "DELETE FROM player_cooldowns WHERE player='" + name + "';";
	Server::sqlQueue->Write(sql);

	std::map<string, Skill *>::iterator iter;
	for (iter = knownSkills.begin(); iter != knownSkills.end(); ++iter)
	{
		std::map<int, double>::iterator iter2;
		iter2 = cooldowns.find((*iter).second->id);
		if (iter2 == cooldowns.end()) //not on cooldown
			continue;
		if ((*iter2).second <= Game::currentTime) //not on cooldown
			continue;

		sql = "INSERT INTO player_cooldowns (player_cooldowns.player, player_cooldowns.skill, player_cooldowns.timestamp) VALUES ('";
		sql += name + "', " + Utilities::itos((*iter).second->id) + ", " + Utilities::dtos((*iter2).second, 2) + ");";
		Server::sqlQueue->Write(sql);
	}
}

void Player::LoadCooldowns()
{
	StoreQueryResult cooldownres;
	try {
		cooldownres = Server::sqlQueue->Read("SELECT * FROM player_cooldowns WHERE player='" + name + "';");
		if (!cooldownres || cooldownres.empty())
			return;
	}
	catch (std::exception e)
	{
		LogFile::Log("error", e.what());
		return;
	}

	Row row;
	StoreQueryResult::iterator i;
	for (i = cooldownres.begin(); i != cooldownres.end(); i++)
	{
		row = *i;
		Skill * sk = Game::GetGame()->GetSkill(row["skill"]);
		if (sk != nullptr)
		{
			cooldowns[sk->id] = row["timestamp"];
		}
	}

	string sql = "DELETE FROM player_cooldowns WHERE player='" + name + "';";
	Server::sqlQueue->Write(sql);
}

void Player::SaveSpellAffects()
{
	string deletesql = "DELETE FROM player_spell_affects WHERE player = '" + name + "';";
	Server::sqlQueue->Write(deletesql);

	std::list<SpellAffect*>::iterator iter;
	for (iter = spell_affects.begin(); iter != spell_affects.end(); ++iter)
	{
		(*iter)->Save(name);
	}
}

void Player::LoadSpellAffects()
{
	SpellAffect::Load(this);
}

double Player::GetMainhandWeaponSpeed()
{
	if (equipped[Player::EQUIP_MAINHAND] != nullptr
		&& equipped[Player::EQUIP_MAINHAND]->speed > 0)
	{
		return equipped[Player::EQUIP_MAINHAND]->speed;
	}
	return 0;
}

double Player::GetOffhandWeaponSpeed()
{
	if (equipped[Player::EQUIP_OFFHAND] != nullptr
		&& equipped[Player::EQUIP_OFFHAND]->speed > 0)
	{
		return equipped[Player::EQUIP_OFFHAND]->speed;
	}
	return 0;
}


int Player::GetMainhandDamageRandomHit()
{

	int damage = 1;
	if (equipped[Player::EQUIP_MAINHAND] != nullptr
		&& equipped[Player::EQUIP_MAINHAND]->speed > 0
		&& equipped[Player::EQUIP_MAINHAND]->damageHigh > 0)
	{
		int high = equipped[Player::EQUIP_MAINHAND]->damageHigh;
		int low = equipped[Player::EQUIP_MAINHAND]->damageLow;
		if (high != low)
			damage = (Server::rand() % (high + 1 - low)) + low;
		else
			damage = low;
	}
	else
	{
		return 0;
	}
	return damage;
}

double Player::GetMainhandDamagePerSecond()
{

	double dps = 1;
	if (equipped[Player::EQUIP_MAINHAND] != nullptr
		&& equipped[Player::EQUIP_MAINHAND]->speed > 0
		&& equipped[Player::EQUIP_MAINHAND]->damageHigh > 0)
	{
		double weaponSpeed_main = equipped[Player::EQUIP_MAINHAND]->speed;
		int high = equipped[Player::EQUIP_MAINHAND]->damageHigh;
		int low = equipped[Player::EQUIP_MAINHAND]->damageLow;
		dps = ((low + high) / 2.0) / weaponSpeed_main;
	}
	else
	{
		return 0;
	}
	return dps;
}

int Player::GetOffhandDamageRandomHit()
{

	int damage = 1;
	if (equipped[Player::EQUIP_OFFHAND] != nullptr
		&& equipped[Player::EQUIP_OFFHAND]->speed > 0
		&& equipped[Player::EQUIP_OFFHAND]->damageHigh > 0)
	{
		int high = equipped[Player::EQUIP_OFFHAND]->damageHigh;
		int low = equipped[Player::EQUIP_OFFHAND]->damageLow;
		if (high != low)
			damage = (Server::rand() % (high + 1 - low)) + low;
		else
			damage = low;
	}
	else
	{
		return 0;
	}
	return damage;
}

double Player::GetOffhandDamagePerSecond()
{

	double dps = 1;
	if (equipped[Player::EQUIP_OFFHAND] != nullptr
		&& equipped[Player::EQUIP_OFFHAND]->speed > 0
		&& equipped[Player::EQUIP_OFFHAND]->damageHigh > 0)
	{
		double weaponSpeed_main = equipped[Player::EQUIP_OFFHAND]->speed;
		int high = equipped[Player::EQUIP_OFFHAND]->damageHigh;
		int low = equipped[Player::EQUIP_OFFHAND]->damageLow;
		dps = ((low + high) / 2.0) / weaponSpeed_main;
	}
	else
	{
		return 0;
	}
	return dps;
}

void Player::SetComboPoints(int howmany)
{
	if (howmany < 0 || howmany > maxComboPoints)
		return;
	comboPoints = howmany;
}

void Player::GenerateComboPoint(Character * target)
{
	if (target == nullptr)
		return;

	if (!target->IsAlive())
	{
		LogFile::Log("error", "GenerateComboPoint on !IsAlive target");
		return;
	}

	if (target != comboPointTarget)	//Changing our combo target
	{
		if (comboPointTarget != nullptr)	//If we had a previous combo target...
		{
			comboPointTarget->RemoveSubscriber(this);
		}
		comboPointTarget = target;
		comboPoints = 1;
		target->AddSubscriber(this);
	}
	else if (comboPoints < maxComboPoints)
	{
		comboPoints++;
	}
}

int Player::SpendComboPoints(Character * target)
{
	if (target == nullptr || comboPointTarget == nullptr || target != comboPointTarget || comboPoints <= 0)
		return 0;

	comboPointTarget->RemoveSubscriber(this);
	comboPointTarget = nullptr;
	int combos = comboPoints;
	comboPoints = 0;
	return combos;
}

void Player::ClearComboPointTarget()
{
	if (comboPointTarget == nullptr)
		return;

	comboPointTarget->RemoveSubscriber(this);
	comboPointTarget = nullptr;
	comboPoints = 0;
}

void Player::MakeCorpse()
{
	Item * thecorpse = new Item("The corpse of " + GetName(), 0);
	thecorpse->keywords = "corpse " + GetName();
	Utilities::FlagSet(thecorpse->flags, Item::FLAG_ROOMONLY);
	//room->AddItem(thecorpse); //cant use this function since it assumes a valid ID and does item stacking!!
	room->items.push_back(std::make_pair(thecorpse, 1)); //need new room function AddCorpseItem??
}

void Player::RemoveCorpse()
{
	if (corpse_room == 0)
	{
        //Not really an error... sometimes call RemoveCorpse when we're not sure if it exists or not
		//LogFile::Log("error", "Character::RemoveCorpse() with bad player->corpse_room");
		return;
	}

	Room * corpseroom = Game::GetGame()->GetRoom(corpse_room);

	for (auto itemiter = corpseroom->items.begin(); itemiter != corpseroom->items.end(); itemiter++)
	{
		if (itemiter->first->keywords.find(GetName()) != std::string::npos)
		{
			//corpseroom->RemoveItem(itemiter->first); //cant use this function since it assumes a valid ID and does item stacking!!
			delete itemiter->first;
			corpseroom->items.erase(itemiter);

			if (room && room == corpseroom)
			{
				Message(GetName() + "'s corpse crumbles into dust.", Character::MessageType::MSG_ROOM_NOTCHAR);
			}
			else
			{
				corpseroom->Message(GetName() + "'s corpse crumbles into dust.");
			}

			break;
		}
	}
}

void Player::StartGlobalCooldown()
{
	globalCooldown = Game::currentTime + 1.5;
    json cooldown = { { "name", "global_cooldown" },
                      { "time", 1.5 } };
    SendGMCP("char.cooldown " + cooldown.dump());
}

void Player::ApplyExperience(int amount)
{
	SetExperience(experience + amount);

	while (experience < Game::ExperienceForLevel(level - 1))
	{
		//lose levels
		SetLevel(level - 1);
	}
	while (level < Game::MAX_LEVEL && experience >= Game::ExperienceForLevel(level + 1))
	{
		//gain levels
		SetLevel(level + 1);
	}
}

void Player::Notify(SubscriberManager * lm)
{
	Character::Notify(lm);

	if (comboPointTarget && lm == comboPointTarget)
	{
		comboPointTarget->RemoveSubscriber(this);
		comboPointTarget = nullptr;
	}

    if (HasQuery())
    {
        for (auto iter = begin(queryList); iter != end(queryList);)
        {
            if ((Character *)iter->queryDataPtr == (Character *)lm) //We have a query pending where the 'data' payload is the Character being deleted (group invite)
            {
                lm->RemoveSubscriber(this);
                queryList.erase(iter++);
            }
        }
    }
	RemoveLootRoll((Character *)lm);
}

void Player::AddClassSkills()
{
	std::list<Player::ClassData>::iterator iter;
	for (iter = classList.begin(); iter != classList.end(); ++iter) //For every class this player has multiclassed into...
	{
		Class * iclass = Game::GetGame()->classes.at(iter->id);	//Grab that class from the game...
		std::list<Class::SkillData>::iterator csiter;
		for (csiter = iclass->classSkills.begin(); csiter != iclass->classSkills.end(); csiter++) //For every skill players of that class get...
		{
			if (csiter->level <= iter->level || IsImmortal()) //if that skill's level < our level of the class, add it
			{
				AddSkill(csiter->skill);
			}
		}
	}
}