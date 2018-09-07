#include "stdafx.h"
#include "CClass.h"
#include "CServer.h"
#include "CGame.h"
#include "CSkill.h"
#include "CLogFile.h"
#include "utils.h"
#include <string>

using std::string;

Class::Class()
{
    id = 0;
    name = "";
    color = "";
    items = "";

    changed = false;
}

Class::~Class()
{
    classSkills.clear();
}

void Class::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO classes (id, name, color, items) values(";
    sql += Utilities::itos(id) + ",'" + Utilities::SQLFixQuotes(name) + "','" + Utilities::SQLFixQuotes(color) + "','" + Utilities::SQLFixQuotes(items) + "'";

	sql += ") ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), color=VALUES(color), items=VALUES(items)";

    Server::sqlQueue->Write(sql);

	Server::sqlQueue->Write("DELETE FROM class_skills where class=" + Utilities::itos(id));
	std::list<SkillData>::iterator iter;
	for (iter = classSkills.begin(); iter != classSkills.end(); ++iter)
	{
		string skillssql = "INSERT INTO class_skills (class, skill, level) values (";
		skillssql += Utilities::itos(id) + ", " + Utilities::itos((*iter).skill->id) + ", " + Utilities::itos((*iter).level) + ")";
		skillssql += " ON DUPLICATE KEY UPDATE class=VALUES(class), skill=VALUES(skill), level=VALUES(level)";
		Server::sqlQueue->Write(skillssql);
	}

    changed = false;
}

/*
struct SkillData
{
	Skill * skill;
	int level;
};
std::list<SkillData> classSkills;
*/
bool Class::HasSkill(int id)
{
	auto result = std::find_if(classSkills.begin(), classSkills.end(), CompareClassSkillBySkillID(id));
	if (result == classSkills.end())
		return false;
	return true;
}

void Class::AddSkill(int id, int level)
{
	if (!HasSkill(id))
	{
		Class::SkillData skd;
		skd.skill = Game::GetGame()->GetSkill(id);
		if (skd.skill == nullptr)
		{
			LogFile::Log("error", "Class::AddSkill, could not find skill id: " + Utilities::itos(id));
			return;
		}
		skd.level = level;
		classSkills.push_back(skd);
		return;
	}
	LogFile::Log("error", "Class::AddSkill, tried to add duplicate skill id: " + Utilities::itos(id));
}

void Class::RemoveSkill(int id)
{
	auto result = std::find_if(classSkills.begin(), classSkills.end(), CompareClassSkillBySkillID(id));
	if (result == classSkills.end())
	{
		LogFile::Log("error", "Class::RemoveSkill, could not find skill id: " + Utilities::itos(id));
		return;
	}
	classSkills.erase(result);
}

int Class::GetArmorLevel(int itemtype)
{
	auto found = armor.find(itemtype);
	if (found != armor.end())
	{
		return found->second;
	}
	return 0;
}

bool Class::CompareClassSkillBySkillID::operator()(SkillData & elem) const
{
	return value == elem.skill->id;
}
