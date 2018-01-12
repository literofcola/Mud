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

Help::Help()
{
	id = -1;
	name = "";
	text = "";
	changed = false;
	remove = false;
}

Help::Help(string name_, int id_)
{
	name = name_;
	id = id_;
	text = "";
	changed = false;
	remove = false;
}

Help::~Help()
{

}

void Help::Save()
{
    if(!changed)
        return;

    string sql = "INSERT INTO help (id, name, text)";
	sql += " values (" + Utilities::itos(id);
    sql += ",'" + Utilities::SQLFixQuotes(name) + "','" + Utilities::SQLFixQuotes(text);
    sql += "')";

    sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), name=VALUES(name), text=VALUES(text)";

    Server::sqlQueue->Write(sql);

    changed = false;
}
