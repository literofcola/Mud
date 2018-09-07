#include "CHelp.h"
#include "utils.h"
#include "CServer.h"
#include <string>

Help::Help()
{
	id = -1;
	title = "";
	text = "";
	changed = false;
	remove = false;
}

Help::Help(std::string title_, std::string search_string_, int id_)
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

	std::string sql = "INSERT INTO help (id, title, search_string, text)";
	sql += " values (" + Utilities::itos(id);
    sql += ",'" + Utilities::SQLFixQuotes(title) + "','" + Utilities::SQLFixQuotes(title) + "','" + Utilities::SQLFixQuotes(text);
    sql += "')";

    sql += " ON DUPLICATE KEY UPDATE id=VALUES(id), title=VALUES(title), search_string=VALUES(search_string), text=VALUES(text)";

    Server::sqlQueue->Write(sql);

    changed = false;
}
