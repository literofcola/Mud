#include "stdafx.h"
#include "CListener.h"
#include "CListenerManager.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"
#include "CClient.h"
typedef boost::shared_ptr<Client> Client_ptr;
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
typedef boost::shared_ptr<Server> Server_ptr;
#include "utils.h"


std::string Exit::exitNames[Exit::DIR_LAST] = {
    "north", "northeast", "east", "southeast", "south", "southwest", "west", "northwest", "up", "down" };
std::string Exit::reverseExitNames[Exit::DIR_LAST] = {
    "south", "southwest", "west", "southwest", "north", "northeast", "east", "southeast", "below", "above" };
int Exit::exitOpposite[Exit::DIR_LAST] = { 4, 5, 6, 7, 0, 1, 2, 3, 9, 8 };

Exit::Exit()
{
    from = to = NULL;
    direction = DIR_LAST;
    removeme = false;
}

Exit::Exit(Room * from_, Room * to_, int dir)
{
    from = from_;
    to = to_;
    direction = (Exit::Direction)dir;
    removeme = false;
}

Exit::~Exit()
{

}