#include "stdafx.h"
#include "CSubscriber.h"
#include "CSubscriberManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
#include "CHighResTimer.h"
#include "CHelp.h"
#include "CTrigger.h"
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

using namespace std;

Quest::Quest()
{
    id = 0;
    level = 1;
    questRequirement = 0;
    levelRequirement = 0;
    questRestriction = 0;
    start = NULL;
    end = NULL;
    experienceReward = 0;
    moneyReward = 0;
    shareable = false;
    changed = false;

    intTable["id"] = &id;
    intTable["level"] = &level;
    intTable["questRequirement"] = &questRequirement;
    intTable["levelRequirement"] = &levelRequirement;
    intTable["questRestriction"] = &questRestriction;
    intTable["experienceReward"] = &experienceReward;
    intTable["moneyReward"] = &moneyReward;
    stringTable["name"] = &name;
    stringTable["shortDescription"] = &shortDescription;
    stringTable["longDescription"] = &longDescription;
    stringTable["progressMessage"] = &progressMessage;
    stringTable["completionMessage"] = &completionMessage;
}

Quest::Quest(string name_, int id_)
{
    id = id_;
    name = name_;
    level = 1;
    questRequirement = 0;
    levelRequirement = 0;
    questRestriction = 0;
    start = NULL;
    end = NULL;
    experienceReward = 0;
    moneyReward = 0;
    shareable = false;
    changed = false;

    intTable["id"] = &id;
    intTable["level"] = &level;
    intTable["questRequirement"] = &questRequirement;
    intTable["levelRequirement"] = &levelRequirement;
    intTable["questRestriction"] = &questRestriction;
    intTable["experienceReward"] = &experienceReward;
    intTable["moneyReward"] = &moneyReward;
    stringTable["name"] = &name;
    stringTable["shortDescription"] = &shortDescription;
    stringTable["longDescription"] = &longDescription;
    stringTable["progressMessage"] = &progressMessage;
    stringTable["completionMessage"] = &completionMessage;
}

Quest::~Quest()
{
    objectives.clear();
}

void Quest::AddObjective(int type, int count, int id, string desc)
{
    QuestObjective t;
    t.type = (ObjectiveType)type;
    t.count = count;
    t.description = desc;
    switch(type)
    {
        case Quest::OBJECTIVE_ROOM:
            t.objective = Game::GetGame()->GetRoom(id);
            break;
            
        case Quest::OBJECTIVE_VISITNPC:
        case Quest::OBJECTIVE_KILLNPC:
            t.objective = Game::GetGame()->GetCharacterIndex(id);
            break;

        case Quest::OBJECTIVE_ITEM:
            t.objective = Game::GetGame()->GetItem(id);
            break;
    }
    objectives.push_back(t);
}

void Quest::RemoveObjective(int index) //non-zero based index
{
    if(index > (int)objectives.size())
    {
        LogFile::Log("error", "Quest::RemoveObjective, index >= objectives.size");
        return;
    }

    int i = 1;
    std::vector<QuestObjective>::iterator iter;
    for(iter = objectives.begin(); iter < objectives.end(); ++iter)
    {
        if(i == index)
        {
            objectives.erase(iter);
            return;
        }
        i++;
    }
}

int Quest::GetDifficulty(int playerlevel, int questlevel)
{
    if(playerlevel < questlevel - 10) //ineligible
    {
        return 6;
    }
    else if(playerlevel >= questlevel - 10 && playerlevel < questlevel - 6) //red difficulty
    {
        return 5;
    }
    else if(playerlevel >= questlevel - 6 && playerlevel < questlevel - 2) //orange difficulty
    {
        return 4;
    }
    else if(playerlevel >= questlevel - 2 && playerlevel < questlevel + 2) //yellow difficulty
    {
        return 3;
    }
    else if(playerlevel >= questlevel + 2 && playerlevel < questlevel + 6) //blue difficulty
    {
        return 2;
    }
    else if(playerlevel >= questlevel + 6 && playerlevel < questlevel + 10) //green difficulty
    {
        return 1;
    }
    else //gray
    {
        return 0;
    }
}

string Quest::GetDifficultyColor(int playerlevel, int questlevel)
{
    if(playerlevel < questlevel - 10) //ineligible
    {
        return "|X";
    }
    else if(playerlevel >= questlevel - 10 && playerlevel < questlevel - 6) //red difficulty
    {
        return "|R";
    }
    else if(playerlevel >= questlevel - 6 && playerlevel < questlevel - 2) //orange difficulty
    {
        return "|M";
    }
    else if(playerlevel >= questlevel - 2 && playerlevel < questlevel + 2) //yellow difficulty
    {
        return "|Y";
    }
    else if(playerlevel >= questlevel + 2 && playerlevel < questlevel + 6) //blue difficulty
    {
        return "|C";
    }
    else if(playerlevel >= questlevel + 6 && playerlevel < questlevel + 10) //green difficulty
    {
        return "|G";
    }
    else //gray
    {
        return "|D";
    }
}

void Quest::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO quests (id, name, short_description, long_description, progress_message, completion_message, level, quest_requirement, ";
    sql += "start, end, exp_reward, money_reward, shareable, level_requirement, quest_restriction) values (";
    sql += Utilities::itos(id) + ",'" + Utilities::SQLFixQuotes(name) + "','" + Utilities::SQLFixQuotes(shortDescription);
    sql += "','" + Utilities::SQLFixQuotes(longDescription) + "','" + Utilities::SQLFixQuotes(progressMessage);
    sql += "','" + Utilities::SQLFixQuotes(completionMessage) + "','" + Utilities::itos(level) + "',";
    sql += Utilities::itos(questRequirement) + "," + (start ? Utilities::itos(start->id) : "0") + ",";
    sql += (end ? Utilities::itos(end->id) : "0") + ",";
    sql += Utilities::itos(experienceReward) + "," + Utilities::itos(moneyReward) + "," + Utilities::itos(shareable) + ", ";
    sql += Utilities::itos(levelRequirement) + "," + Utilities::itos(questRestriction) + ")";

    sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), short_description=VALUES(short_description), long_description=VALUES(long_description), ";
    sql += "progress_message=VALUES(progress_message), completion_message=VALUES(completion_message), level=VALUES(level), ";
    sql += "quest_requirement=VALUES(quest_requirement), start=VALUES(start), end=VALUES(end), exp_reward=VALUES(exp_reward), ";
    sql += "money_reward=VALUES(money_reward), shareable=VALUES(shareable), level_requirement=VALUES(level_requirement), ";
    sql += "quest_restriction=VALUES(quest_restriction)";

    Server::sqlQueue->Write(sql);

	Server::sqlQueue->Write("DELETE FROM quest_objectives where quest=" + Utilities::itos(id));
	std::vector<QuestObjective>::iterator iter;
	for(iter = objectives.begin(); iter != objectives.end(); ++iter)
	{
		int saveid = 0;
		switch((*iter).type)
		{
			case Quest::OBJECTIVE_ITEM: saveid = ((Item*)((*iter).objective))->id; break;
			case Quest::OBJECTIVE_KILLNPC:
			case Quest::OBJECTIVE_VISITNPC: saveid = ((Character*)((*iter).objective))->id; break;
			case Quest::OBJECTIVE_ROOM: saveid = ((Room*)((*iter).objective))->id; break;
		}
		string objectivesql = "INSERT INTO quest_objectives (quest, type, id, count, description) values (";
		objectivesql += Utilities::itos(id) + ", " + Utilities::itos((*iter).type) + ", " + Utilities::itos(saveid);
		objectivesql += ", " + Utilities::itos((*iter).count) + ",'" + Utilities::SQLFixQuotes((*iter).description) + "')";

		objectivesql += " ON DUPLICATE KEY UPDATE quest=VALUES(quest), type=VALUES(type), id=VALUES(id), count=VALUES(count), description=VALUES(description)";

		Server::sqlQueue->Write(objectivesql);
	}

	Server::sqlQueue->Write("DELETE FROM quest_item_rewards where quest=" + Utilities::itos(id));
	std::vector<int>::iterator itemiter;
	for (itemiter = itemRewards.begin(); itemiter != itemRewards.end(); ++itemiter)
	{
		string itemsql = "INSERT INTO quest_item_rewards (quest, item) values (";
		itemsql += Utilities::itos(id) + ", " + Utilities::itos((*itemiter)) + ")";

		itemsql += " ON DUPLICATE KEY UPDATE quest=VALUES(quest), item=VALUES(item)";

		Server::sqlQueue->Write(itemsql);
	}

    changed = false;
}