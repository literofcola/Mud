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
#include "CCommand.h"
#include "mud.h"
#include "utils.h"

Reset::Reset()
{
    id = 0;
    type = 1;
    npc = NULL;
    interval = 30;
    wanderDistance = 0;
    leashDistance = 0;
    lastReset = 0;
	count = 1;
    removeme = false;
}

Reset::Reset(Room * r_, int type_, int targetid_)
{
	inroom = r_;
	type = type_;
	targetID = targetid_;

	id = 0;
	npc = NULL;
	interval = 30;
	wanderDistance = 0;
	leashDistance = 0;
	lastReset = 0;
	count = 1;
	removeme = false;
}

Reset::~Reset()
{

}

//Character/Object this reset refers to has been deleted
void Reset::Notify(SubscriberManager * lm)
{
    //LogFile::Log("status", "Reset " + Utilities::itos(id) + " setting npc = NULL");
	//cout << "RESET Subscriber " << this->id << " removed from manager " << npc->name;
	//cout << " Count: " << subscribercount-- << endl;
	npc->RemoveSubscriber(this);
    npc = NULL;
    lastReset = Game::currentTime;
}