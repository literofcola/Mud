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
	title = "";
	text = "";
	changed = false;
	remove = false;
}

Help::Help(string title_, string search_string_, int id_)
{
	title = title_;
	search_string = search_string_;
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

    string sql = "INSERT INTO help (id, title, search_string, text)";
	sql += " values (" + Utilities::itos(id);
    sql += ",'" + Utilities::SQLFixQuotes(title) + "','" + Utilities::SQLFixQuotes(title) + "','" + Utilities::SQLFixQuotes(text);
    sql += "')";

    sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), title=VALUES(title), search_string=VALUES(search_string), text=VALUES(text)";

    Server::sqlQueue->Write(sql);

    changed = false;
}
