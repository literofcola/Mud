#include "stdafx.h"
#include "CGame.h"
#include "CServer.h"
#include "CClient.h"
#include "CUser.h"
#include "CPlayer.h"
#include "CCharacter.h"
#include "CNPC.h"
#include "CRoom.h"
#include "CTrigger.h"
#include "CReset.h"
#include "CSpellAffect.h"
#include "CClass.h"
#include "CQuest.h"
#include "CSkill.h"
#include "CGroup.h"
#include "CItem.h"
#include "CArea.h"
#include "CCommand.h"
#include "CNPCIndex.h"
#include "CHelp.h"
#include "utils.h"
#include "CLogFile.h"
#include "mud.h"

using json = nlohmann::json;

using std::string;

const std::string login_art = R"(
                                               _   __,----'~~~~~~~~~`-----.__)" "\r" R"(
                                        .  .    `//====-              ____,-'~`)" "\r" R"(
                        -.            \_|// .   /||\\  `~~~~`---.___./)" "\r" R"(
                  ______-==.       _-~o  `\/    |||  \\           _,'`)" "\r" R"(
            __,--'   ,=='||\=_    ;_,_,/ _-'|-   |`\   \\        ,')" "\r" R"(
         _-'      ,='    | \\`.    '',/~7  /-   /  ||   `\.     / )" "\r" R"(
       .'       ,'       |  \\  \_  "  /  /-   /   ||      \   /)" "\r" R"(
      / _____  /         |     \\.`-_/  /|- _/   ,||       \ / )" "\r" R"(
     ,-'     `-|--'~~`--_ \     `==-/  `| \'--===-'       _/`)" "\r" R"(
               '         `-|      /|    )-'\~'      _,--"')" "\r" R"(
                           '-~^\_/ |    |   `\_   ,^             /\)" "\r" R"(
                                /  \     \__   \/~               `\__)" "\r" R"(
                            _,-' _/'\ ,-'~____-'`-/                 ``===\)" "\r" R"(
                           ((->/'    \|||' `.     `\.  ,                _|| )" "\r" R"(
             ./                       \_     `\      `~---|__i__i__\--~'_/)" "\r" R"(
            <_n_                     __-^-_    `)  \-.______________,-~')" "\r" R"(
             `0'\)                  ///,-'~`__--^-  |-------~~~~^')" "\r" R"(
             /^>                           ///,--~`-\)" "\r" R"(
            `  `)" "\r" R"(
)";

double Game::currentTime = 0;
extern Game * thegame;

Game * Game::GetGame()
{
    /*static Game * theGame = nullptr;
    if(theGame == nullptr)
    {
        theGame = new Game();
    }
    return theGame;*/
	return thegame;
}

//TODO hmm
void Game::DeleteGame()
{
    Game * deleteme = GetGame();
    delete deleteme;
    deleteme = nullptr;
}

Game::Game()
{
    newplayerRoom = 1;
    total_past_connections = 0;
    max_players_since_boot = 0;
    total_players_since_boot = 0;
	totalBytesCompressed = totalBytesUncompressed = 0;
    shutdown = false;
    InitializeCriticalSection(&userListCS);
}

Game::~Game()
{
    DeleteCriticalSection(&userListCS);

    for(auto iter = characters.begin(); iter != characters.end(); ++iter)
    {
        (*iter)->NotifySubscribers();
    }
    while (!characters.empty())
    {
        delete characters.front();
        characters.pop_front();
    }
    while (!users.empty())
    {
        delete users.front();
        users.pop_front();
    }

    std::map<int, NPCIndex *>::iterator iter2;
    for(iter2 = npcIndex.begin(); iter2 != npcIndex.end(); ++iter2)
    {
        delete (*iter2).second;
    }
	npcIndex.clear();

    std::map<int, Skill *>::iterator iter;
    for (iter = skills.begin(); iter != skills.end(); ++iter)
    {
        delete (*iter).second;
    }
    skills.clear();
    
    std::map<int, Room *>::iterator iter3;
    for(iter3 = rooms.begin(); iter3 != rooms.end(); ++iter3)
    {
        delete (*iter3).second;
    }
    rooms.clear();

	std::map<int, Area *>::iterator iter4;
    for(iter4 = areas.begin(); iter4 != areas.end(); ++iter4)
    {
        delete (*iter4).second;
    }
    areas.clear();

	std::map<int, Quest *>::iterator iter5;
    for(iter5 = quests.begin(); iter5 != quests.end(); ++iter5)
    {
        delete (*iter5).second;
    }
    quests.clear();

	std::map<int, Item *>::iterator iter6;
    for(iter6 = items.begin(); iter6 != items.end(); ++iter6)
    {
        delete (*iter6).second;
    }
	items.clear();

	std::map<int, Help *>::iterator iter7;
    for(iter7 = helpIndex.begin(); iter7 != helpIndex.end(); ++iter7)
    {
        delete (*iter7).second;
    }
    helpIndex.clear();

	std::map<int, Class *>::iterator iter8;
    for(iter8 = classes.begin(); iter8 != classes.end(); ++iter8)
    {
        delete (*iter8).second;
    }
    classes.clear();
}

void Game::GameLoop(Server * server)
{
    //timer.Reset();

    LogFile::Log("status", "GameLoop, LoadServerStats()");
    LoadGameStats(server);
    LogFile::Log("status", "GameLoop, LoadAreas()");
    LoadAreas(server);
    LogFile::Log("status", "GameLoop, LoadRooms()");
    LoadRooms(server);  //Can't load exits while loading rooms since destination may not yet be loaded
    LogFile::Log("status", "GameLoop, LoadExits()");
    LoadExits(server);
    LogFile::Log("status", "GameLoop, LoadSkills()");
    LoadSkills(server);
    LogFile::Log("status", "GameLoop, LoadItems()");
    LoadItems(server);
    LogFile::Log("status", "GameLoop, LoadNPCS()");
    LoadNPCS(server);
    LogFile::Log("status", "GameLoop, LoadResets()");
    LoadResets(server);
    LogFile::Log("status", "GameLoop, LoadQuests()");
    LoadQuests(server);
    LogFile::Log("status", "GameLoop, LoadClasses()");
    LoadClasses(server);
    LogFile::Log("status", "GameLoop, LoadTriggers()");
    LoadTriggers(server);
	LogFile::Log("status", "GameLoop, LoadHelp()");
    LoadHelp(server);

    LogFile::Log("status", "GameLoop() start");

	server->acceptReady = true;

    std::list<User *>::iterator iter;
    //__int64 duration = 0;
    _timeb time;
    time_t time_secs;
    unsigned short time_millis;
    while(!shutdown)
	{
		//input
        EnterCriticalSection(&userListCS); //Locks out the AcceptThread from adding a new user to the userlist
		iter = users.begin();
		while(iter != users.end())
		{
			User * user = *iter;
            ++iter;

			if(user == nullptr)
				continue;

			if (user->lastInput + IDLE_TIMEOUT <= Game::currentTime)
			{
				if (!user->character
					|| (user->character && !user->character->IsImmortal()))
				{
					user->Send("Idle timeout exceeded. Disconnecting.\r\n");
					user->SetDisconnect();
				}
			}

			if(!user->IsConnected() || user->remove)
			{
				//stop taking commands
				user->ClearClientCommandQueue();
				continue;
			}

			user->wasInput = false;

			user->GetOneCommandFromNetwork();

			if(user->HasCommandReady())
			{
				user->lastInput = Game::currentTime; //idle timeout reset
                string command = user->commandQueue.front();

                //Check for telnet IAC response for MXP and MCCP
                if(command.length() > 0 && command[0] == Server::IAC[0])
                {
                    string iac_response = command.substr(0, 3);
                    command = command.substr(3, command.length()-3);

                    if(iac_response == Server::MXP_DO)
                    {
                        //turn on mxp
                        user->Send("MXP Enabled\r\n");
						//Send immediately (only really necessary for MCCP)
						server->deliver(user->GetClient(), Server::MXP_START);
						server->deliver(user->GetClient(), MXP_LOCKLOCKED);
                        user->mxp = true;
                    }
					else if(iac_response == Server::GMCP_DO)
					{
						user->Send("GMCP Enabled\r\n");
						server->deliver(user->GetClient(), Server::GMCP_START);
						user->gmcp = true;
					}
					else if(iac_response == Server::MCCP_DO)
					{
						//turn on mccp
						user->Send("MCCP2 Enabled\r\n");
						/* allocate deflate state */
						user->z_strm.zalloc = Z_NULL;
						user->z_strm.zfree = Z_NULL;
						user->z_strm.opaque = Z_NULL;
						user->z_ret = deflateInit(&user->z_strm, Z_DEFAULT_COMPRESSION);
						if(user->z_ret != Z_OK)
						{
							LogFile::Log("error", "Game::GameLoop; Could not init zlib");
						}
						//Send immediately
						server->deliver(user->GetClient(), Server::MCCP_START);
						user->mccp = true;
					}

					//only handle one IAC response at a time so a naughty client doesnt lock up the gameloop
					//need to fix up the command queue with whatever's left after removing this response
					user->commandQueue.pop_front();
					user->commandQueue.push_front(command);
					continue;
                }

                if(user->stringEdit)    //Working in the string editor
                {
                    StringEdit::string_add(user, command);
                }
                else if(user->connectedState == User::CONN_PLAYING && user->character) //Normal command interpreting
				{
					if (Command::Interpret(user->character, command))
					{
						user->wasInput = true; //Allow the call to GeneratePrompt
					}
                }
                else    //Logging in
                {
				    LoginHandler(server, user, command);
				}
				user->commandQueue.pop_front();
			}
            if(user->character && !user->character->movementQueue.empty() && user->connectedState == User::CONN_PLAYING)
            {
                if(user->character->CanMove())
                {
                    //Command::Interpret(server, user, user->movementQueue.front());
                    //Already ran interpret on this command... just call the function
                    (user->character->movementQueue.front())(user->character, "");
                    user->character->movementQueue.pop_front();
                    user->character->lastMoveTime = currentTime;
                    user->wasInput = true; //Allow the call to GeneratePrompt
                }
            }
		}

        //we can probably accept new users while in the WorldUpdate
        LeaveCriticalSection(&userListCS); //Locks out the AcceptThread from adding a new user to the userlist
		WorldUpdate(server);
        EnterCriticalSection(&userListCS); //Locks out the AcceptThread from adding a new user to the userlist

		//output
        iter = users.begin();
		while(iter != users.end())
		{
			User * user = *iter;

            /*if(!user->client)
            {
                ++iter;
                continue;
            }*/

            //Disconnected but didn't "quit". Save the user
			if(!user->IsConnected() && !user->remove)
            {
                user->outputQueue.clear();
                if(user->connectedState > User::CONN_PLAYING) // ...unless we're not CONN_PLAYING yet, then drop everything
                {
                    if(user->character)
                    {
                        user->character->NotifySubscribers();
                        characters.remove(user->character);
                        //RemoveCharacter(user->character);
                    }
                    //RemoveUser(user);
					//user->Disconnect();
                    delete (*iter);
                    iter = users.erase(iter);
                    user = nullptr;
                }
                else
                {
                    ++iter;    
                }
                continue;
            }

            if(user->IsConnected() && !user->remove && (user->wasInput || !user->outputQueue.empty()) && user->IsPlaying() && user->character)
                user->character->GeneratePrompt(currentTime);

			string out = "";
			while(user->IsConnected() && (!user->outputQueue.empty() || (user->gmcp && !user->GMCPQueue.empty())))
			{
				//dont try a send greater than NETWORK_BUFFER_SIZE, but loop until everything is sent
				out.clear();
				while(user->IsConnected() && !user->outputQueue.empty())
				{
					if(out.length() + user->outputQueue.front().length() > NETWORK_BUFFER_SIZE)
						break;
					out += user->outputQueue.front();
					user->outputQueue.pop_front();
				}
				while(user->gmcp && user->IsConnected() && !user->GMCPQueue.empty())
				{
					if(out.length() + user->GMCPQueue.front().length() > NETWORK_BUFFER_SIZE)
						break;
					out += user->GMCPQueue.front();
					user->GMCPQueue.pop_front();
				}

				if(user->mccp)
				{
					memcpy(user->z_in, out.c_str(), out.length());
					user->z_strm.avail_in = (uInt)out.length();
					user->z_strm.next_in = user->z_in;
					user->z_strm.avail_out = user->Z_BUFSIZE;
					user->z_strm.next_out = user->z_out;

					user->z_ret = deflate(&user->z_strm, Z_SYNC_FLUSH);     

					int z_have = user->Z_BUFSIZE - user->z_strm.avail_out;
					totalBytesCompressed += z_have;			//server stats
					totalBytesUncompressed += out.length();
					server->deliver(user->GetClient(), user->z_out, z_have);
				}
				else
				{
					totalBytesCompressed += out.length(); //server stats
					totalBytesUncompressed += out.length();
					server->deliver(user->GetClient(), out);
				}
			}

			//Check for quit command 
			if(!user->IsConnected() || user->remove)
			{
                //LogFile::Log("status", "removing user via remove flag in gameloop");
                if(user->character)
                {
					if (!user->character->IsAlive())
						user->character->ChangeRooms(Game::GetGame()->GetRoom(user->character->graveyard_room));
					if (user->connectedState == User::CONN_PLAYING && user->character->GetLevel() > 1) //don't save fresh characters
					{
						user->character->SaveSpellAffects();
						user->character->SaveCooldowns();
						user->character->Save();
					}
					if (user->character->HasGroup())
						cmd_group(user->character, "leave");
					user->character->ExitCombat();
					user->character->ClearTarget();
                    if(user->character->HasQuery(cmd_groupQuery))
                        cmd_groupQuery(user->character, "decline");
					if (!user->character->IsGhost())
						user->character->Message(user->character->GetName() + " has left the game.", Character::MSG_ROOM_NOTCHAR);
					user->character->ChangeRooms(nullptr);
                    user->character->NotifySubscribers();
                    characters.remove(user->character);
                }
				if(user->mccp)
				{
					deflateEnd(&user->z_strm);
				}
				user->ImmediateDisconnect();
                delete (*iter);
                iter = users.erase(iter);
			}
            else
            {
                ++iter;
            }
		}
        LeaveCriticalSection(&userListCS); //Locks out the AcceptThread from adding a new user to the userlist
		Sleep(1);

		_ftime64_s(&time);
		time_secs = (time_t)time.time;
		time_millis = time.millitm;
		currentTime = ((int)time_secs + ((double)time_millis / 1000.0));	//Don't set currentTime before the first WorldUpdate so all the updates fire immediately
	}

    //Server is going down!
    iter = users.begin();
    while (iter != users.end())
    {
        User * user = (*iter);
        if (user->GetClient())
            closesocket(user->GetClient()->Socket());
        user->ImmediateDisconnect();

        //Save user/player
        if (user->character)
        {
            if (!user->character->IsAlive())
                user->character->ChangeRooms(Game::GetGame()->GetRoom(user->character->graveyard_room));
            if (user->connectedState == User::CONN_PLAYING && user->character->GetLevel() > 1) //don't save fresh characters
            {
                user->character->SaveSpellAffects();
                user->character->SaveCooldowns();
                user->character->Save();
            }
            if (user->character->HasGroup())
                cmd_group(user->character, "leave");
            user->character->ExitCombat();
            user->character->ClearTarget();
            if (user->character->HasQuery(cmd_groupQuery))
                cmd_groupQuery(user->character, "decline");
            if (!user->character->IsGhost())
                user->character->Message(user->character->GetName() + " has left the game.", Character::MSG_ROOM_NOTCHAR);
            user->character->ChangeRooms(nullptr);
            user->character->NotifySubscribers();
            characters.remove(user->character);
        }
        delete user;
        iter = users.erase(iter);
    }

    //duration = timer.ElapsedMicro();
    LogFile::Log("status", "GameLoop() end");
}

void Game::WorldUpdate(Server * server)
{
    static double twoSecondTick = Game::currentTime;	//yes static variable initialization is only done the first time this line is reached
    static double thirtySecondTick = Game::currentTime;
    static double worldupdateTimer = Game::currentTime;
    bool doTwoSecondTick = false;
    bool doThirtySecondTick = false;

    if(twoSecondTick + 2.0 <= currentTime)
    {
        twoSecondTick = Game::currentTime;
        doTwoSecondTick = true;
    }
    if(thirtySecondTick + 30.0 <= currentTime)
    {
        thirtySecondTick = Game::currentTime;
        doThirtySecondTick = true;
    }

    if(worldupdateTimer + 1.0 <= currentTime)
    {
        worldupdateTimer = Game::currentTime;
        worldupdateCount = 0;
    }
    worldupdateCount++;

    std::list<Character *>::iterator iter = characters.begin();
    while(iter != characters.end())
    {
        Character * currChar = (*iter);
		NPC * currNPC = nullptr;
		Player * currPlayer = nullptr;

		//polymorphism is loosely in effect...
		if (currChar->IsNPC())
			currNPC = (NPC*)currChar;
		else if (currChar->IsPlayer())
			currPlayer = (Player*)currChar;

        if(currChar->remove)
        {
            delete (*iter);
            iter = characters.erase(iter);
            continue;
        }
        ++iter;

        if(!currChar->IsAlive())
        {
			//todo: release spirit timer?
			if (!currChar->IsNPC() && currChar->IsGhost())
			{
                Room * corpseroom = GetRoom(currPlayer->corpse_room);

				//if we have a query and its 'acceptres' and we don't meet criteria, clear it
				if (currPlayer->HasQuery(acceptResQuery)
					&& ((!currPlayer->CanRes(currPlayer->TimeSinceDeath()) || currPlayer->room->id != currPlayer->graveyard_room)
					&&  (!currPlayer->CanResAtCorpse(currPlayer->TimeSinceDeath()) || FindDistance(currPlayer->room, corpseroom, 1) == -1)))
				{
					currPlayer->QueryClear(acceptResQuery);
				}
				//if we have a query and its 'returnToGYQuery' and we don't meet criteria, clear it
				if (currPlayer->HasQuery(returnToGYQuery)
					&& (currPlayer->room->id == currPlayer->graveyard_room)
					|| (currPlayer->room->id == currPlayer->corpse_room && currPlayer->CanResAtCorpse(currPlayer->TimeSinceDeath())))
				{
					currPlayer->QueryClear(returnToGYQuery);
				}

				if(!currPlayer->HasQuery(acceptResQuery)
			      &&((currPlayer->CanRes(currPlayer->TimeSinceDeath()) && currPlayer->room->id == currPlayer->graveyard_room)
				  ||(currPlayer->CanResAtCorpse(currPlayer->TimeSinceDeath()) && FindDistance(currPlayer->room, corpseroom, 1) != -1)))
				{
					currPlayer->AddQuery("Resurrect now? ('res') ", nullptr, acceptResQuery);
				}
				if (!currPlayer->HasQuery(returnToGYQuery) && currPlayer->room->id != currPlayer->graveyard_room)
				{
					currPlayer->AddQuery("Return to Graveyard? ('return') ", nullptr, returnToGYQuery);
				}
			}
			if(currChar->IsNPC())
			{
				if (!currNPC->loot.empty())
				{
					//Check for any roll timers expiring
					for (auto iter = begin(currNPC->loot); iter != end(currNPC->loot);)
					{
						NPC::OneLoot * oneloot = &(*iter);
						++iter; //incremented here because DoLootRoll might remove this loot item
						if (oneloot->roll_timer > 0 && oneloot->roll_timer < Game::currentTime) //expired!
							currNPC->DoLootRoll(oneloot);
					}
				}
				//Check NPC corpse despawn time
				if ((currNPC->loot.empty() && currNPC->TimeSinceDeath() >= 120) ||
				   (!currNPC->loot.empty() && currNPC->TimeSinceDeath() >= 360)) //todo: variable duration per NPC
				{
					currNPC->Message(currNPC->GetName() + "'s corpse crumbles into dust.", Character::MessageType::MSG_ROOM_NOTCHAR);
					RemoveCharacter(currNPC);
					currNPC->ChangeRooms(nullptr);
				}
			}
            continue;
        }
        //Tick, every 2 seconds
        if(doTwoSecondTick)
        {
			//Stat regeneration
			if (!currChar->InCombat() && currChar->GetHealth() < currChar->GetMaxHealth())
			{
				if (currChar->IsNPC())
					currChar->SetHealth(currChar->GetMaxHealth()); //NPC's heal immediately out of combat
				else
					currChar->AdjustHealth(nullptr, (int)ceil(10 * log10(currChar->GetLevel()) + 5));
			}
			if (currChar->GetMana() < currChar->GetMaxMana() && currChar->lastSpellCast + 5.0 <= Game::currentTime)
			{
				//if more than 5 seconds since last cast, regen 10% of spirit as mana, 1% for npcs
				if (currChar->IsNPC())
					currChar->AdjustMana(currChar, (int)ceil(currChar->GetMaxMana() * 0.01));
				else
					currPlayer->AdjustMana(currPlayer, (int)ceil(currPlayer->GetSpirit() * 0.1) + 10);
			}
			if (currChar->GetEnergy() < currChar->GetMaxEnergy())
			{
				currChar->AdjustEnergy(currChar, 20); //1 energy per .1 second regen
			}
			//Rage decay, 1 per second
			if (!currChar->InCombat() && currChar->GetRage() > 0)
			{
				currChar->AdjustRage(currChar, -2);
			}

            //Check NPC TIMER triggers
            Trigger * trig = nullptr;
            int ctr = 0;
            while(currChar->IsNPC() && (trig = currNPC->GetNPCIndex()->GetTrigger(ctr, Trigger::TIMER)) != nullptr)
            {
                ctr++;
                if(!trig->IsTimerExpired())
                    continue;
                
                trig->StartTimer();

                string func = trig->GetFunction();
                try
                {
                    //TODO: dont load the script every time?
                    //LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for NPC " + Utilities::itos(curr->id));
					Server::lua.script(trig->GetScript().c_str());
					sol::function lua_trig_func = Server::lua[func.c_str()];
					sol::protected_function_result result = lua_trig_func(currChar);
					if (!result.valid())
					{
						// Call failed
						sol::error err = result;
						std::string what = err.what();
						LogFile::Log("error", "NPC TIMER trigger call failed, sol::error::what() is: " + what);
					}
                }
                catch(const std::exception & e)
			    {
				    LogFile::Log("error", e.what());
			    }
            }
			//check npc aggro
			if (currChar->IsNPC() && currChar->room && !currChar->InCombat() && currChar->FlagIsSet(NPCIndex::FLAG_AGGRESSIVE))
			{
				for (std::list<Character*>::iterator aggroiter = currChar->room->characters.begin(); aggroiter != currChar->room->characters.end(); ++aggroiter)
				{
					//todo: decide who to attack in the room based off something else than being first on the list. level at least
					if (!(*aggroiter)->IsNPC() && (*aggroiter)->IsAlive() && !(*aggroiter)->IsImmortal())
					{
						currChar->EnterCombat((*aggroiter));
						(*aggroiter)->EnterCombat(currChar);
						currChar->AutoAttack((*aggroiter));
						break;
					}
				}
			}
        }
        
        //Delay Update
        if(currChar->HasActiveDelay())
        {
            if (currChar->IsCrowdControlled())
            {
                Skill * casting = currChar->GetDelaySkill();
                if (casting != nullptr)
                {
                    currChar->Send("|WYour " + casting->GetShortName() + " has been interrupted!|X\r\n");
                    currChar->Message("|W" + currChar->GetName() + "'s " + casting->GetShortName() + " has been interrupted!|X", Character::MSG_ROOM_NOTCHAR);
                }
                currChar->CancelActiveDelay();
            }
            else if (currChar->delay <= currentTime)
            {
                (*currChar->delayFunction)(currChar->delayData);
            }
        }
        //Combat update
        if(currChar->InCombat())
        {
            //Check COMBAT_TIMER triggers but only on 2 second tick
            if (doTwoSecondTick)
            {
                Trigger * trig = nullptr;
                int ctr = 0;
                while (currChar->IsNPC() && (trig = currNPC->GetNPCIndex()->GetTrigger(ctr, Trigger::COMBAT_TIMER)) != nullptr)
                {
                    ctr++;
                    if (!trig->IsTimerExpired())
                        continue;

                    trig->StartTimer();

                    string func = trig->GetFunction();
                    try
                    {
                        //TODO: dont load the script every time?
                        //LogFile::Log("status", "Loading lua trigger script " + Utilities::itos(trig->id) + " for NPC " + Utilities::itos(curr->id));
                        Server::lua.script(trig->GetScript().c_str());
                        sol::function lua_trig_func = Server::lua[func.c_str()];
                        sol::protected_function_result result = lua_trig_func(currChar);
                        if (!result.valid())
                        {
                            // Call failed
                            sol::error err = result;
                            std::string what = err.what();
                            LogFile::Log("error", "NPC COMBAT_TIMER trigger call failed, sol::error::what() is: " + what);
                        }
                    }
                    catch (const std::exception & e)
                    {
                        LogFile::Log("error", e.what());
                    }
                }
            }

			if (currChar->IsNPC() && currChar->GetTarget() && !currChar->GetTarget()->IsAlive())
			{
				currChar->ClearTarget();
			}

            if(!currChar->GetTarget() || currChar->GetTarget() == currChar)
            {  //Turn off auto attack. cmd_target should take care of this, but just in case
				currChar->CancelAutoAttack();
            }
            if(currChar->meleeActive && currChar->GetTarget() && currChar->GetTarget()->room == currChar->room && !currChar->HasActiveDelay())
            {   //Auto attack but not while casting
				currChar->AutoAttack(currChar->GetTarget());
            }
            else if (currChar->IsNPC() && !currChar->meleeActive && !currChar->IsCrowdControlled() && currChar->GetTarget() && currChar->GetTarget()->room == currChar->room)
            {   //Allows NPC to resume attacking after crowd control expires
                //currChar->meleeActive = true;
                currChar->AutoAttack(currChar->GetTarget());
            }
            /*
            else if(!currChar->meleeActive && currChar->GetTarget() && currChar->GetTarget()->GetTarget() && currChar->GetTarget()->GetTarget() == currChar
                && currChar->GetTarget()->meleeActive && currChar->GetTarget()->room == currChar->room)
            { //So... If we're not attacking, we have a target, our target's target is us, and is attacking us, and theyre in the same room, start attacking them back
				currChar->AutoAttack(currChar->GetTarget());
            }
            */
			//Players exit combat after 5 seconds of no activity AND when we have no npcs on our threat list
            if(currChar->IsPlayer() && currPlayer->lastCombatAction + 5 <= currentTime && !currPlayer->CheckThreatCombat())
            {   
				currPlayer->ExitCombat();
            }

			//Threat management, chasing/leashing (NPC's start chasing AFTER movementspeed delay)
			if (currChar->IsNPC() && !currNPC->GetTopThreat()) //We're a NPC in combat but have noone on our threat list. We must have killed them. Leave combat!
			{
				currNPC->ExitCombat();
				currNPC->ClearTarget();
				//See if we need to leash
				std::pair<Room *, int> path;
				while (!currNPC->leashPath.empty() && currNPC->room != currNPC->leashOrigin)
				{
					path = currNPC->leashPath.back();
					currNPC->leashPath.pop_back();

					//Fake enter/leave messages. We can't use ->Move() since that tests for valid exits
					//  among other things (think leashing back through 1 way exits)
					currNPC->Message(currNPC->GetName() + " leaves " + Exit::exitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
					currNPC->ChangeRooms(path.first);
					currNPC->Message(currNPC->GetName() + " has arrived from "
						+ ((path.second != Exit::DIR_UP && path.second != Exit::DIR_DOWN) ? "the " : "")
						+ Exit::reverseExitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
				}

			}
            else if(currChar->IsNPC() && currNPC->GetTopThreat())
            {
                if (currChar->IsCrowdControlled())
                {
                    currNPC->movementQueue.clear();
                    currChar->EnterCombat(currNPC->GetTopThreat()); //Enter Combat just so our aggro chains to anything else in the room
                }

				if (!currNPC->movementQueue.empty()) //We have a movement pending, see if we can move...
				{
					if(currNPC->CanMove()) //Move! (checks movespeed auras and movement speed timestamp)
					{
						currNPC->movementQueue.pop_front();
						//track target... Give up leashDistance rooms away from target... (todo, separate this from leashdist?)
						int leashdist = Reset::RESET_LEASH_DEFAULT;
						if (currNPC->reset && currNPC->reset->leashDistance != 0)
							leashdist = currNPC->reset->leashDistance;
						Exit::Direction chasedir = FindDirection(currNPC, currNPC->GetTopThreat(), leashdist);
						if (chasedir != Exit::DIR_LAST)
						{
							//record the path we take for backtracking
							currNPC->leashPath.push_back(std::make_pair(currNPC->room, chasedir));
                            currNPC->EnterCombat(currNPC->GetTopThreat()); //Enter Combat just so our aggro chains to anything else in the room
							currNPC->Move(chasedir);
							if(currNPC->GetTopThreat()->room == currNPC->room)
                                currNPC->AutoAttack(currNPC->GetTopThreat());
						}
						else
						{
							//target more than leashDistance rooms away, leash!
							std::pair<Room *, int> path;
							currNPC->ExitCombat();
							currNPC->ClearTarget();
							while (!currNPC->leashPath.empty() && currNPC->room != currNPC->leashOrigin)
							{
								path = currNPC->leashPath.back();
								currNPC->leashPath.pop_back();
								
								//Fake enter/leave messages. We can't use ->Move() since that tests for valid exits
								//  among other things (think leashing back through 1 way exits)
								currNPC->Message(currNPC->GetName() + " leaves " + Exit::exitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
								currNPC->ChangeRooms(path.first);
								currNPC->Message(currNPC->GetName() + " has arrived from "
									+ ((path.second != Exit::DIR_UP && path.second != Exit::DIR_DOWN) ? "the " : "") 
									+ Exit::reverseExitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
							}
						}
					}
				}
                else if(currNPC->GetTopThreat()->room != currNPC->room && currNPC->movementQueue.empty() /*&& !currChar->IsCrowdControlled()*/)
                { //We need to chase threat target, and not already pending a move...
					//Decide if we should try to chase based on how far we are from our reset
					int leashdist = Reset::RESET_LEASH_DEFAULT;
					if (currNPC->reset && currNPC->reset->leashDistance != 0)
						leashdist = currNPC->reset->leashDistance;

					int npcDistance = FindDistance(currNPC->leashOrigin, currNPC->GetTopThreat()->room, leashdist);
					if (npcDistance == -1) //Farther from our origin than leashdist, Leash!
					{
						currNPC->ExitCombat();
						currNPC->ClearTarget();
                        currNPC->RemoveAllSpellAffects();
						std::pair<Room *, int> path;
						while (!currNPC->leashPath.empty() && currNPC->room != currNPC->leashOrigin)
						{
							path = currNPC->leashPath.back();
							currNPC->leashPath.pop_back();

							//Fake enter/leave messages. We can't use ->Move() since that tests for valid exits
							//  among other things (think leashing back through 1 way exits)
							currNPC->Message(currNPC->GetName() + " leaves " + Exit::exitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
							currNPC->ChangeRooms(path.first);
							currNPC->Message(currNPC->GetName() + " has arrived from "
								+ ((path.second != Exit::DIR_UP && path.second != Exit::DIR_DOWN) ? "the " : "")
								+ Exit::reverseExitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
						}
					}
					else
					{
						//Just push a placeholder that indicates we have a movement pending, since we're going to "track" the target after movespeed delay
						currNPC->movementQueue.push_back(nullptr);
						currNPC->lastMoveTime = currentTime; //queue up the next move after delay
						currNPC->CancelAutoAttack();		  //target isn't in the room...
					}
                }
				//Check for taunt and highest threat
				SpellAffect * taunt = currNPC->GetFirstSpellAffectWithAura(SpellAffect::AURA_TAUNT);
				Character * topthreat = currNPC->GetTopThreat();
                //Being taunted
				if (taunt != nullptr && taunt->caster != nullptr)
				{
					if (currNPC->GetTarget() != taunt->caster)
					{
						taunt->caster->Send("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to YOU!|X\r\n");
						taunt->caster->Message("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to " + taunt->caster->GetName() + "!|X",
							Character::MessageType::MSG_ROOM_NOTCHARVICT, currNPC);
					}
					currNPC->SetTarget(taunt->caster);
				}
                //Aggro change without taunt
                else if(topthreat && topthreat != currNPC->GetTarget() && (currNPC->GetThreat(currNPC->GetTarget()) + currNPC->GetThreat(currNPC->GetTarget()) * .1) < currNPC->GetThreat(topthreat))
                {
					currNPC->SetTarget(currNPC->GetTopThreat());
					currNPC->GetTarget()->Send("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to YOU!|X\r\n");
					currNPC->Message("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to " + currNPC->GetTarget()->GetName() + "!|X",
						Character::MessageType::MSG_ROOM_NOTCHARVICT, currNPC->GetTarget());
                }
                //No target but have targets on threat meter (just killed top threat?), aquire new target
                else if (topthreat && currNPC->GetTarget() == nullptr)
                {
                    currNPC->SetTarget(currNPC->GetTopThreat());
                    currNPC->GetTarget()->Send("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to YOU!|X\r\n");
                    currNPC->Message("|R" + currNPC->GetName() + " changes " + currNPC->HisHer() + " target to " + currNPC->GetTarget()->GetName() + "!|X",
                        Character::MessageType::MSG_ROOM_NOTCHARVICT, currNPC->GetTarget());
                }
            }
        }
        
        //Buff/debuff update
        if(!currChar->buffs.empty())
        {
            std::list<SpellAffect*>::iterator buffiter = currChar->buffs.begin();
            while(buffiter != currChar->buffs.end())
            {
                SpellAffect * sa = (*buffiter);
                if(sa->ticksRemaining > 0 
                   && Game::currentTime > sa->appliedTime + ((sa->ticks - sa->ticksRemaining+1)*(sa->duration / sa->ticks)))
                {
                    if (!sa->caster)
                    {
                        sa->caster = GetPlayerByName(sa->casterName);
                    }
                    sa->ticksRemaining--;
                    
                    sa->skill->CallLuaTick(sa->caster, currChar, sa);

                    //Reset the loop, who knows what happened to our debuffs during the tick
                    buffiter = currChar->buffs.begin();
                    continue;
                }
                if(!sa->remove_me && sa->duration > 0 && Game::currentTime - sa->appliedTime > sa->duration) //Expired
                {
                    if(!sa->caster)
                    {
                        sa->caster = GetPlayerByName(sa->casterName);
                    }
                    sa->skill->CallLuaRemove(sa->caster, currChar, sa);

                    //Reset the loop, who knows what happened to our buffs during the remove
                    sa->remove_me = true;
                    buffiter = currChar->buffs.begin();
                    continue;
                } 
                //else
                ++buffiter;
            }
            //Loop them again to remove any flagged
            buffiter = currChar->buffs.begin();
            while(buffiter != currChar->buffs.end())
            {
                if((*buffiter)->remove_me) //Expired
                {
                    delete (*buffiter);
                    buffiter = currChar->buffs.erase(buffiter);
                }
                else
                {
                    ++buffiter;
                }
            }
        }
        if(!currChar->debuffs.empty())
        {
            std::list<SpellAffect*>::iterator debuffiter = currChar->debuffs.begin();
            while(debuffiter != currChar->debuffs.end())
            {
                SpellAffect * sa = (*debuffiter);

                if(sa->ticksRemaining > 0 
                   && Game::currentTime > sa->appliedTime + ((sa->ticks - sa->ticksRemaining+1)*(sa->duration / sa->ticks)))
                {
                    if (!sa->caster)
                    {
                        sa->caster = GetPlayerByName(sa->casterName);
                    }
                    sa->ticksRemaining--;

                    sa->skill->CallLuaTick(sa->caster, currChar, sa);
                    
                    //Reset the loop, who knows what happened to our debuffs during the tick
                    debuffiter = currChar->debuffs.begin();
                    continue;
                }
                if(!sa->remove_me && sa->duration > 0 && Game::currentTime - sa->appliedTime > sa->duration) //Expired
                {
                    if(!sa->caster)
                    {
                        sa->caster = GetPlayerByName(sa->casterName);
                    }

                    sa->skill->CallLuaRemove(sa->caster, currChar, sa);

                    //Reset the loop, who knows what happened to our debuffs during the remove
                    sa->remove_me = true;
                    debuffiter = currChar->debuffs.begin();
                    continue;
                }
                //else
                ++debuffiter;
            }
            //Loop them again to remove any flagged
            debuffiter = currChar->debuffs.begin();
            while(debuffiter != currChar->debuffs.end())
            {
                if((*debuffiter)->remove_me) //Expired
                {
                    delete (*debuffiter);
                    debuffiter = currChar->debuffs.erase(debuffiter);
                }
                else
                {
                    ++debuffiter;
                }
            }
        }
        //Target range update
        if(!currChar->IsImmortal() && !currChar->IsNPC() && currChar->GetTarget() && currChar->GetTarget()->room != currChar->room)
        {
            //Allow target to remain active in same room and up to two rooms away (for players, npcs keep target anywhere)
			Exit::Direction dir = FindDirection(currChar, currChar->GetTarget(), 3);
			if (dir == Exit::DIR_LAST) //Clear the target
			{
				currChar->Send("Target out of range.\r\n");
				currChar->ClearTarget();
			}
				
        }
    }

    if(doThirtySecondTick)
    {
        //Check resets
        for(std::map<int, Room*>::iterator roomiter = rooms.begin(); roomiter != rooms.end(); ++roomiter)
        {
            Room * currRoom = (*roomiter).second;
            for(std::map<int, Reset*>::iterator resetiter = currRoom->resets.begin(); resetiter != currRoom->resets.end(); ++resetiter)
            {
                Reset * currReset = (*resetiter).second;
                if(currReset->lastReset + currReset->interval <= Game::currentTime && !currReset->removeme)
                {
                    if(currReset->type == 1 && currReset->npc == nullptr) //npc reset type and npc no longer exists
                    {
                        currReset->lastReset = Game::currentTime;
                        //load it
                        NPCIndex * charIndex = Game::GetGame()->GetNPCIndex(currReset->targetID);
                        if(charIndex == nullptr)
                        {
                            LogFile::Log("error", "Reset " + Utilities::itos(currReset->id) + " in room " + Utilities::itos(currRoom->id) + ": npc does not exist.");
                            continue;
                        }
                        NPC * newChar = Game::GetGame()->NewNPC(charIndex);
						newChar->leashOrigin = currRoom;
                        newChar->ChangeRooms(currRoom);
                        newChar->Message("|W" + newChar->GetName() + " has arrived.|X", Character::MSG_ROOM_NOTCHAR);
                        newChar->reset = currReset;
                        //LogFile::Log("status", "Adding subscriber to " + newChar->name + " of reset id " + Utilities::itos(currReset->id));
                        newChar->AddSubscriber(currReset);
                        currReset->npc = newChar;
                    }
					else if (currReset->type == 2 && !currReset->inroom->HasItem(currReset->targetID))
					{
						currReset->lastReset = Game::currentTime;
						//load it
						Item * itemindex = Game::GetGame()->GetItem(currReset->targetID);
						if (itemindex == nullptr)
						{
							LogFile::Log("error", "Reset " + Utilities::itos(currReset->id) + " in room " + Utilities::itos(currRoom->id) + ": item " + Utilities::itos(currReset->targetID) + " does not exist.");
							continue;
						}
						currReset->inroom->AddItem(itemindex);
					}
                }
            }
        }
    }
}

void Game::LoginHandler(Server * server, User * user, string argument)
{
    string arg1;
    Utilities::one_argument(argument, arg1);

    switch(user->connectedState)
    {
        case User::CONN_GET_NAME:
		{
            if(arg1.empty())
            {
                user->Send("Player Name: ");
                return;
            }
			if(arg1.length() < 3)
			{
				user->Send("Too Short (3-12 characters)! Try again...\r\nPlayer Name: ");
				return;
			}
            if(arg1.length() > 12)
			{
				user->Send("Too Long (3-12 characters)! Try again...\r\nPlayer Name: ");
				return;
			}
            //TODO: better invalid name checking
            if(!Utilities::IsAlpha(arg1) || !Utilities::str_cmp(arg1, "self") || !Utilities::str_cmp(arg1, "me"))
            {
                user->Send("Illegal name, try another...\r\nPlayer Name: ");
                return;
            }
            if(arg1 == "GET") //stop http connections?
            {
				user->ImmediateDisconnect();
                return;
            }

            User * tempUser;
            arg1 = Utilities::ToLower(arg1);
            arg1[0] = Utilities::UPPER(arg1[0]);
            //arg1 = Utilities::ReplaceApostrophe(arg1);  //already called IsAlpha

            if(!Server::sqlQueue->Read("select name from players where name='" + arg1 + "'").empty())
			{	//player already exists
				user->Send("Welcome Back " + arg1 + "!\r\nEnter Password: ");
                user->character = new Player(arg1, user);
                user->character->password = server->SQLSelectPassword(arg1);
                user->connectedState = User::CONN_GET_OLD_PASSWORD;
			}
			else if((tempUser = Game::GetGame()->GetUserByPCName(arg1)) != nullptr 
                    && tempUser->connectedState >= User::CONN_GET_NEW_PASSWORD
				    && tempUser->connectedState <= User::CONN_CONFIRM_CLASS)
			{   //player exists, but hasnt finished creating
				user->Send("A character is currently being created with that name.\r\n");
				LogFile::Log("status", "Login attempt on creating character in progress : " + tempUser->character->GetName());
				user->SetDisconnect();
			}
            else if(tempUser != nullptr && (tempUser->connectedState == User::CONN_PLAYING || tempUser->connectedState > User::CONN_CONFIRM_CLASS))
			{   //player exists, but hasnt been saved yet
				user->Send("Welcome Back " + arg1 + "!\r\nEnter Password: ");
                user->connectedState = User::CONN_GET_OLD_PASSWORD;
                user->character = new Player(arg1, user);
                user->character->password = tempUser->character->password;
                LogFile::Log("status", "Reconnecting unsaved character : " + user->character->GetName());
			}
			else // user dosn't exist - create and ask for password
			{
				user->Send("User " + arg1 + " Does Not Exist - Creating\r\nEnter Password:\r\n");
                user->connectedState = User::CONN_GET_NEW_PASSWORD;
                user->character = Game::GetGame()->NewPlayer(arg1, user);
                LogFile::Log("status", "Creating character : " + user->character->GetName());
			}
			break;
		}

        case User::CONN_GET_OLD_PASSWORD:
		{
            if(server->EncryptDecrypt(arg1) != user->character->password) //never decrypt password, just encrypt the input
            {
				user->passwordAttempts++;
				if (user->passwordAttempts >= User::MAX_PASSWORD_TRIES)
				{
					user->Send("Incorrect password - disconnecting...");
					user->SetDisconnect();
				}
				else
				{
					user->Send("Incorrect password - try again...\r\nEnter Password: ");
				}
                break;
            }

            User * existingUser = Game::GetGame()->DuplicatePlayerCheck(user->character->GetName());

            if(existingUser == nullptr)
            {   //Need to load from the db
                LogFile::Log("status", "Loading character : " + user->character->GetName());
                Player * c = Player::LoadPlayer(user->character->GetName(), user);
                delete user->character;
                user->character = c;
                user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
                user->connectedState = User::CONN_MENU;
            }
            else
            {   //Already loaded player, swap with tempUser
                delete user->character;
                user->character = existingUser->character;
                user->character->user = user;
                existingUser->character = nullptr;
                if(existingUser->IsConnected())
                {
                    existingUser->Send("\r\nMultiple login detected. Disconnecting...\r\n");
                    existingUser->SetDisconnect();

                }
                user->connectedState = existingUser->connectedState;
                if(user->connectedState >= User::CONN_CHANGEPW1 && user->connectedState <= User::CONN_DELETE2)
                {
					//If we took over someone not CONN_PLAYING, drop them in CONN_MENU
                    user->connectedState = User::CONN_MENU;
                }
                user->Send("Reconnecting...\r\n");
                user->character->Message(user->character->GetName() + " has reconnected.", Character::MSG_ROOM_NOTCHAR);
            }
            break;
		}

        case User::CONN_GET_NEW_PASSWORD:
		{
			if(arg1.length() < 5)
			{
				user->Send("Too Short (5-15 characters)! Try again...\r\nEnter Password: ");
				return;
			}
			else if(arg1.length() > 15)
			{
				user->Send("Too Long (5-15 characters)! Try again...\r\nEnter Password: ");
				return;
			}
            user->character->password = server->EncryptDecrypt(arg1);
            user->connectedState = User::CONN_CONFIRM_NEW_PASSWORD;
            user->Send("Confirm Password: \r\n");
            break;
		}

        case User::CONN_CONFIRM_NEW_PASSWORD:
		{
		    if(server->EncryptDecrypt(arg1) == user->character->password)
            {
				string raceChoice = "|YChoose your race (";
				for (int i = 0; Character::race_table[i].id != -1; i++)
				{
					raceChoice += " " + Character::race_table[i].name;
				}
				raceChoice += " ):|X ";
				user->Send(raceChoice);
                user->connectedState = User::CONN_GET_RACE;
            }
            else // No Match
            {
				user->Send("Password does not match - try again...\r\nEnter Password: ");
				user->character->password.clear();
                user->connectedState = User::CONN_GET_NEW_PASSWORD;
            }
            break;
        }

        case User::CONN_GET_RACE:
        {
			bool match = false;
			int raceid = 0;
			for (raceid = 0; Character::race_table[raceid].id != -1; raceid++)
			{
				if (!Utilities::str_cmp(Character::race_table[raceid].name, arg1))
				{
					match = true;
					break;
				}
			}

            if(match)
            {
                user->Send("|YAre you sure you want this race (y/n):|X ");
				user->character->race = raceid;
				user->connectedState = User::CONN_CONFIRM_RACE;
            }
            else
            {
				user->Send("|YInvalid choice.|X\r\n");
				string raceChoice = "|YChoose your race (";
				for (int i = 0; Character::race_table[i].id != -1; i++)
				{
					raceChoice += " " + Character::race_table[i].name;
				}
				raceChoice += " ):|X ";
				user->Send(raceChoice);
				user->connectedState = User::CONN_GET_RACE;
            }
            break;
        }

        case User::CONN_CONFIRM_RACE:
        {
			if (!Utilities::str_cmp(arg1, "y"))
			{
				std::string classChoice = "|YChoose your class (";
				std::map<int, Class *>::iterator iter;
				for (iter = classes.begin(); iter != classes.end(); ++iter)
				{
					classChoice += " " + (*iter).second->name;
				}
				classChoice += " ):|X ";
				user->Send(classChoice);
				user->connectedState = User::CONN_GET_CLASS;
			}
			else
			{
				string raceChoice = "|YChoose your race (";
				for (int i = 0; Character::race_table[i].id != -1; i++)
				{
					raceChoice += " " + Character::race_table[i].name;
				}
				raceChoice += " ):|X ";
				user->Send(raceChoice);
				user->connectedState = User::CONN_GET_RACE;
			}
			break;
        }
        
        case User::CONN_GET_CLASS:
        {
			bool match = false;
			std::map<int, Class *>::iterator iter;
			for (iter = classes.begin(); iter != classes.end(); ++iter)
			{
				if (!Utilities::str_cmp((*iter).second->name, arg1))
					match = true;
			}

            if(match)
            {
                user->Send("|YAre you sure you want this class (y/n):|X ");
                arg1 = Utilities::ToLower(arg1);
                arg1[0] = Utilities::UPPER(arg1[0]);
                user->character->currentClass = GetClassByName(arg1);
                user->connectedState = User::CONN_CONFIRM_CLASS;
            }
            else
            {
				user->Send("|YInvalid choice.|X\r\n");
				std::string classChoice = "|YChoose your class (";
				std::map<int, Class *>::iterator iter;
				for (iter = classes.begin(); iter != classes.end(); ++iter)
				{
					classChoice += " " + (*iter).second->name;
				}
				classChoice += " ):|X ";
				user->Send(classChoice);
                user->connectedState = User::CONN_GET_CLASS;
            }
            break;
        }

		case User::CONN_CONFIRM_CLASS:
		{
			if (!Utilities::str_cmp(arg1, "y"))
			{
				user->character->AddClass(user->character->currentClass->GetID(), 1);
				user->character->AddClassSkills();
				//function-ize the default items (or parse it into a container upon loading...)
				string classitems = user->character->currentClass->items;
				int first = 0, last = 0, comma = 0;
				while (first < (int)classitems.length())
				{
					last = (int)classitems.find(";", first);
					if (last == std::string::npos)
						break;
					comma = (int)classitems.find(",", first);
					int id = Utilities::atoi(classitems.substr(first, comma - first));
					int count = Utilities::atoi(classitems.substr(comma+1, last - comma+1));
					first = last + 1;
					Item * itemIndex = GetItem(id);
					if (itemIndex == nullptr)
					{
						LogFile::Log("error", "Item " + Utilities::itos(id) + " does not exist.");
						continue;
					}
					for (int i = 0; i < count; i++)
					{
                        //We're making the assumption here that we're not giving a new player an armor type they can't wear
						user->character->AddItemInventory(itemIndex);
                        int equiploc = user->character->GetEquipLocation(itemIndex);
                        if (equiploc == Player::EQUIP_LAST) //Can't wear this
                        {
                            continue;
                        }
                        if(equiploc == Player::EQUIP_MAINHAND 
                            && user->character->equipped[Player::EQUIP_MAINHAND]
                            && !user->character->equipped[Player::EQUIP_OFFHAND]
                            && user->character->equipped[Player::EQUIP_MAINHAND]->equipLocation == Item::EQUIP_ONEHAND)
                        {   //If we're trying to equip a mainhand but the slot is already occupied by a onehand, and the offhand is empty, move the onehand to the offhand
                            user->character->equipped[Player::EQUIP_OFFHAND] = user->character->equipped[Player::EQUIP_MAINHAND];
                            user->character->equipped[Player::EQUIP_MAINHAND] = nullptr;
                        }
                        if (user->character->EquipItemFromInventory(itemIndex))
                        {
                            user->character->AddEquipmentStats(itemIndex);
                        }
					}
				}

				user->Send("|YEnter character gender: 'M' / 'F' :|X ");
				user->connectedState = User::CONN_GET_GENDER;
			}
			else
			{
				std::string classChoice = "|YChoose your class (";
				std::map<int, Class *>::iterator iter;
				for (iter = classes.begin(); iter != classes.end(); ++iter)
				{
					classChoice += " " + (*iter).second->name;
				}
				classChoice += " ):|X ";
				user->Send(classChoice);
				user->connectedState = User::CONN_GET_CLASS;
			}
			break;
		}

		case User::CONN_GET_GENDER:
		{
			bool valid = false;
			if (!Utilities::str_cmp(arg1, "m"))
			{
				valid = true;
				user->character->SetGender(1);
			}
			else if (!Utilities::str_cmp(arg1, "f"))
			{
				valid = true;
				user->character->SetGender(2);
			}
			else
			{
				user->Send("|YEnter character gender: 'M' / 'F' :|X ");
				user->connectedState = User::CONN_GET_GENDER;
			}

			if (valid)
			{
				user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
				user->connectedState = User::CONN_MENU;
			}
			break;
		}

        case User::CONN_CHANGEPW1:
        {
            if(server->EncryptDecrypt(arg1) == user->character->password)
			{
                user->Send("Enter new password: ");
                user->connectedState = User::CONN_CHANGEPW2;
            }
            else
            {
                user->Send("Incorrect password.\r\n\r\n");
                user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
                user->connectedState = User::CONN_MENU;
            }
            break;
        }

        case User::CONN_CHANGEPW2:
        {
            if(arg1.empty())
            {
                user->Send("Enter new password: ");
                user->connectedState = User::CONN_CHANGEPW2;
                return;
            }
            user->character->pwtemp = arg1;
            user->Send("Confirm new password: ");
            user->connectedState = User::CONN_CHANGEPW3;
            
            break;
        }

        case User::CONN_CHANGEPW3:
        {
            if(arg1 == user->character->pwtemp)
            {
                user->Send("Password changed.\r\n");
                user->character->password = server->EncryptDecrypt(argument);
				Server::sqlQueue->Write("UPDATE players SET password = '" + user->character->password + "' WHERE name='" + user->character->GetName() + "'");
                user->character->pwtemp.clear();
            }
            else
            {
                user->Send("Password doesn't match, NOT changed.\r\n");
                user->character->pwtemp.clear();
            }
            user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
            user->connectedState = User::CONN_MENU;
            break;
        }

        case User::CONN_DELETE1:
        {
            if(server->EncryptDecrypt(arg1)== user->character->password)
            {
                user->Send("|RDelete this character? (y/n):|X ");
                user->connectedState = User::CONN_DELETE2;
            }
            else
            {
                user->Send("Incorrect password.\r\n\r\n");
                user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
                user->connectedState = User::CONN_MENU;
            }
            break;
        }

        case User::CONN_DELETE2:
        {
            if(arg1[0] == 'y' || arg1[0] == 'Y')
            {
                Server::sqlQueue->Write("delete from players where name='" + user->character->GetName() + "';");
                Server::sqlQueue->Write("delete from player_spell_affects where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_class_data where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_completed_quests where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_cooldowns where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_active_quests where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_inventory where player='" + user->character->GetName() + "';");
				Server::sqlQueue->Write("delete from player_alias where player='" + user->character->GetName() + "';");
				user->SetDisconnect();
            }
            else
            {
                user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
                user->connectedState = User::CONN_MENU;
            }
            break;
        }
        
        case User::CONN_MENU:
        {
            if(arg1.empty())
            {
                user->Send("|B.|C1 |MEnter world\r\n |C2|B.|MChange password\r\n|B.|C3 |MDelete this character\r\n |C4|B.|MQuit|X\r\n: ");
                user->connectedState = User::CONN_MENU;
                break;
            }
            if(arg1[0] == '1') //enter game
            {
                user->Send("|MThere are currently |X" + Utilities::itos((int)rooms.size()) + " |Munique locations, |X" +
                    Utilities::itos((int)characters.size() - (int)users.size()) + "|M non player characters and |X" +
                    Utilities::itos((int)quests.size()) + "|M quests.|X\r\n");
                user->Send("|MThere are currently |X" + Utilities::itos((int)users.size()) + "|M players online.|X\r\n");
                user->Send("|MThere have been |X" + Utilities::itos(total_players_since_boot) + 
                    "|M players connected to the server since last boot.|X\r\n");
                user->Send("|MThe most players that have been online at one time since last boot is |X" + 
                    Utilities::itos(max_players_since_boot) + "|M.|X\r\n");
                user->Send("|MYou are player [|X" + Utilities::itos(++total_past_connections) + "|M] connected since May 19th, 2010.|X\r\n\r\n");
                //user->Send("Logged In!\r\n");

				json vitals = { { "hp", user->character->GetHealth() },{ "hpmax", user->character->GetMaxHealth() },{ "mp", user->character->GetMana() },{ "mpmax", user->character->GetMaxMana() },
								{ "en", user->character->GetEnergy() }, { "enmax", user->character->GetMaxEnergy() },{ "rage", user->character->GetRage() } ,{ "ragemax", user->character->GetMaxRage() } };
				user->SendGMCP("char.vitals " + vitals.dump());

                if(user->character->room == nullptr)
                {
					Room * toroom = GetRoom(newplayerRoom);
					if (!toroom)
					{
						user->character->ChangeRooms(rooms.begin()->second);
					}
					else
					{
						user->character->ChangeRoomsID(newplayerRoom);
						if(user->character)
							user->character->recall = newplayerRoom;
					}
                }
                else
                {
                    user->character->ChangeRooms(user->character->room);
                }
                user->connectedState = User::CONN_PLAYING;
                user->character->LoadSpellAffects();
                user->character->LoadCooldowns();
                if(!user->character->IsGhost())
					user->character->Message(user->character->GetName() + " has entered the game.", Character::MSG_ROOM_NOTCHAR);				
                user->character->Look("");
                break;
            }
            else if(arg1[0] == '2') //change password
            {
                user->connectedState = User::CONN_CHANGEPW1;
                user->Send("Confirm current password: ");
                break;
            }
            else if(arg1[0] == '3') //delete
            {
                user->connectedState = User::CONN_DELETE1;
                user->Send("Password: ");
            }
            else if(arg1[0] == '4') //quit
            {
                user->Send("Farewell...");
				user->SetDisconnect();
				//user->remove = true;
                //user->client->disconnect = true;
                //server->remove_client(user->client);
            }
            
            break;
        }
    }
}

void Game::LoadGameStats(Server * server)
{
	std::ifstream in;
    in.open("serverstats.txt");
    if(!in.is_open())
    {
        LogFile::Log("error", "Could not load game stats from serverstats.txt");
        return;
    }
    in >> total_past_connections;
    in >> newplayerRoom;
}

void Game::SaveGameStats()
{
    std::ofstream out;
    out.open("serverstats.txt");
    out << total_past_connections << std::endl;
    out << newplayerRoom << std::endl;
}

void Game::LoadRooms(Server * server)
{
    StoreQueryResult roomres = server->sqlQueue->Read("select * from rooms");
	if (roomres.empty())
	{
		LogFile::Log("error", "Warning! No rooms loaded from database, creating default room. Set the new player room number in serverstats.txt line 2");
		Room * r = new Room(1, "The One Room", "One room to rule them all.\r\n");
		rooms.insert(std::pair<int, Room *>(r->id, r));
		return;
	}

	int first, last;

    Row row;
    StoreQueryResult::iterator i;
    for(i = roomres.begin(); i != roomres.end(); i++)
    {
        row = *i;
        Room * r = new Room(row["id"], (string)row["name"], (string)row["description"]);
        r->area = row["area"];
		
		string flagtext = (string)row["flags"];
		if (flagtext != "NULL")
		{
			first = last = 0;
			while (first < (int)flagtext.length())
			{
				last = (int)flagtext.find(";", first);
				int flag = Utilities::atoi(flagtext.substr(first, last - first));
				Utilities::FlagSet(r->flags, flag);
				first = last + 1;
			}
		}

        rooms.insert(std::pair<int, Room *>(r->id, r));

        //m_rooms.insert(Room(row["id"], (string)row["name"], (string)row["description"]));
    }
}


void Game::SaveRooms()
{
    std::map<int, Room *>::iterator iter;
    for(iter = rooms.begin(); iter != rooms.end(); ++iter)
    {
        (*iter).second->Save();
    }
}

void Game::LoadExits(Server * server)
{
    StoreQueryResult exitres = server->sqlQueue->Read("select * from exits order by exits.from");
    if(exitres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    std::map<int, Room *>::iterator iter;
    for(i = exitres.begin(); i != exitres.end(); i++)
    {
        row = *i;
        Exit * e = new Exit(Game::GetGame()->GetRoom(row["from"]), Game::GetGame()->GetRoom(row["to"]), (int)row["direction"]);
        e->from->exits[e->direction] = e;
    }
}

void Game::LoadTriggers(Server * server)
{
    StoreQueryResult triggerres = server->sqlQueue->Read("select * from triggers order by triggers.parent_type, triggers.parent_id, triggers.id");
    if(triggerres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    std::map<int, Trigger>::iterator iter;
    for(i = triggerres.begin(); i != triggerres.end(); i++)
    {
        row = *i;
        Trigger new_trig;
        new_trig.SetType(row["type"]);
        new_trig.SetArgument((string)row["argument"]);
        new_trig.SetScript((string)row["script"]);
        new_trig.SetFunction((string)row["function"]);

        if((int)row["parent_type"] == Trigger::PARENT_ROOM)
        {
            Room * pRoom = GetRoom(row["parent_id"]);
            if(pRoom == nullptr)
                continue;

            pRoom->AddTrigger(new_trig);
        }
        if((int)row["parent_type"] == Trigger::PARENT_NPC)
        {
            NPCIndex * charindex = GetNPCIndex(row["parent_id"]);
            if(charindex == nullptr)
                continue;

            charindex->AddTrigger(new_trig);
        }
    }
}

void Game::LoadResets(Server * server)
{
    StoreQueryResult resetres = server->sqlQueue->Read("select * from resets order by room_id");
    if(resetres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    std::map<int, Reset *>::iterator iter;
    for(i = resetres.begin(); i != resetres.end(); i++)
    {
        row = *i;
        Room * r = Game::GetGame()->GetRoom(row["room_id"]);
        if(r == nullptr)
            continue;

        Reset * reset = new Reset();
        reset->id = row["id"];
        reset->interval = row["interval"];
        reset->leashDistance = row["leash_dist"];
        reset->type = row["type"];
        reset->wanderDistance = row["wander_dist"];
        reset->npc = nullptr;
        reset->targetID = row["target_id"];
		reset->inroom = r;
        r->resets[reset->id] = reset;
    }
}

void Game::LoadSkills(Server * server)
{
    StoreQueryResult skillres = server->sqlQueue->Read("select * from skills");
    if(skillres.empty())
        return;

	int first, last;
    Row row;
    StoreQueryResult::iterator i;
    for(i = skillres.begin(); i != skillres.end(); i++)
    {
        row = *i;
        Skill * s = new Skill(row["id"], (string)row["long_name"]);
        s->name = (string)row["name"];
        s->function_name = (string)row["function_name"];
        s->targetType = (Skill::TargetType)(int)row["target_type"];
		s->costScript = row["cost_script"];
        s->castScript = (string)row["cast_script"];
        s->applyScript = (string)row["apply_script"];
        s->tickScript = (string)row["tick_script"];
        s->removeScript = (string)row["remove_script"];
		s->cooldown = row["cooldown"];
        s->description = (string)row["description"];
		s->costDescription = (string)row["cost_description"];
        s->castTime = row["cast_time"];
		string flagtext = (string)row["flags"];
		if (flagtext != "NULL")
		{
			first = last = 0;
			while (first < (int)flagtext.length())
			{
				last = (int)flagtext.find(";", first);
				int flag = Utilities::atoi(flagtext.substr(first, last - first));
				Utilities::FlagSet(s->flags, flag);
				first = last + 1;
			}
		}
		
		string iflagtext = (string)row["interrupt_flags"];
		if (iflagtext != "NULL")
		{
			int first = 0, last = 0;
			while (first < (int)iflagtext.length())
			{
				last = (int)iflagtext.find(";", first);
				int flag = Utilities::atoi(iflagtext.substr(first, last - first));
				s->interruptFlags.set(flag);
				first = last + 1;
			}
		}

        skills.insert(std::pair<int, Skill *>(s->id, s));
		try {
			Server::lua.script(s->costScript.c_str());
			Server::lua.script(s->castScript.c_str());
			Server::lua.script(s->applyScript.c_str());
			Server::lua.script(s->tickScript.c_str());
			Server::lua.script(s->removeScript.c_str());
		}
		catch (const std::exception & e)
		{
			LogFile::Log("error", e.what());
		}
    }

	//Load functions in lua_skills.lua (override any from db)
    try
    {
        Server::lua.script_file("lua_skills.lua");
    }
    catch (const std::exception & e)
    {
        LogFile::Log("error", "Loading lua_skills.lua FAILED: " + std::string(e.what()));
    }
}

void Game::LoadQuests(Server * server)
{
    StoreQueryResult questres = server->sqlQueue->Read("select * from quests");
    if(questres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = questres.begin(); i != questres.end(); i++)
    {
        row = *i;
        Quest * q = new Quest((string)row["name"], row["id"]);
        q->shortDescription = (string)row["short_description"];
        q->longDescription = (string)row["long_description"];
        q->progressMessage = (string)row["progress_message"];
        q->completionMessage = (string)row["completion_message"];
        q->questRequirement = row["quest_requirement"];
        q->questRestriction = row["quest_restriction"];
        q->experienceReward = row["exp_reward"];
        q->moneyReward = row["money_reward"];
        q->level = row["level"];
        q->levelRequirement = row["level_requirement"];
        q->shareable = row["shareable"];
        q->start = row["start"];
		NPCIndex * qstart = GetNPCIndex(q->start);
        if(qstart != nullptr)
			qstart->questStart.push_back(q);
        q->end = row["end"];
		NPCIndex * qend = GetNPCIndex(q->end);
		if (qend != nullptr)
			qend->questEnd.push_back(q);

		StoreQueryResult objectiveres = server->sqlQueue->Read("select * from quest_objectives where quest=" + Utilities::itos(q->id));
		StoreQueryResult::iterator j;
		for (j = objectiveres.begin(); j != objectiveres.end(); ++j)
		{
			row = *j;
			q->AddObjective(row["type"], row["count"], row["id"], (string)row["description"]);
		}

		StoreQueryResult itemres = server->sqlQueue->Read("select * from quest_item_rewards where quest=" + Utilities::itos(q->id));
		for (j = itemres.begin(); j != itemres.end(); ++j)
		{
			row = *j;
			q->itemRewards.push_back(row["item"]);
		}

		quests.insert(std::pair<int, Quest *>(q->id, q));
    }
}

void Game::LoadItems(Server * server)
{
    StoreQueryResult itemres = server->sqlQueue->Read("select * from items");
    if(itemres.empty())
        return;

    Row row;
    StoreQueryResult::iterator iter;
    for(iter = itemres.begin(); iter != itemres.end(); ++iter)
    {
        row = *iter;
        Item * i = new Item((string)row["name"], row["id"]);
		i->keywords = (string)row["keywords"];
		i->inroom_name = (string)row["inroom_name"];
        i->charLevel = row["char_level"];
        i->quality = row["quality"];
        i->itemLevel = row["item_level"];
        i->equipLocation = row["equip_location"];
        i->binds = row["binds"];
        i->type = row["type"];
		i->useSkillID = row["skill_id"];
        i->quest = row["quest"];
        i->armor = row["armor"];
        i->durability = row["durability"];
        i->unique = row["unique"];
        i->damageLow = row["damage_low"];
        i->damageHigh = row["damage_high"];
        i->value = row["value"];
        i->speed = row["speed"];
		i->agility = row["agility"];
		i->intellect = row["intellect"];
		i->strength = row["strength"];
		i->stamina = row["stamina"];
		i->wisdom = row["wisdom"];
		i->spirit = row["spirit"];

        items[i->GetID()] = i;
    }
}

void Game::LoadClasses(Server * server)
{
    StoreQueryResult classres = server->sqlQueue->Read("select * from classes");

	if (classres.empty())
	{
		LogFile::Log("error", "Warning! No classes loaded from database. Creating default class \"warrior\".");
		Class * c = new Class(1);
		//c->id = 1;
		c->name = "warrior";
		c->color = "|M";
		classes[c->GetID()] = c;
		return;
	}

    Row row;
    StoreQueryResult::iterator iter;
    for(iter = classres.begin(); iter != classres.end(); ++iter)
    {
        row = *iter;
        Class * c = new Class(row["id"]);
        //c->id = row["id"];
        c->name = (string)row["name"];
        c->color = (string)row["color"];
        c->items = row["items"];

		string armor = (string)row["armor"];
		int first = 0, last = 0, comma = 0;
		while (first < (int)armor.length())
		{
			last = (int)armor.find(";", first);
			if (last == std::string::npos)
				break;
			comma = (int)armor.find(",", first);
			int armor_type = Utilities::atoi(armor.substr(first, comma - first));
			int level = Utilities::atoi(armor.substr(comma + 1, last - comma + 1));
			c->armor[armor_type] = level;
			first = last + 1;
		}

		StoreQueryResult skillsres = server->sqlQueue->Read("SELECT * from class_skills where class=" + Utilities::itos(c->GetID()));
		StoreQueryResult::iterator j;
		for (j = skillsres.begin(); j != skillsres.end(); j++)
		{
			row = *j;
			Class::SkillData skd;
			skd.skill = Game::GetGame()->GetSkill(row["skill"]);
			if (skd.skill == nullptr)
			{
				LogFile::Log("error", "Class::AddSkill, could not find skill id: " + Utilities::itos(row["skill"]));
				continue;
			}
			skd.level = row["level"];
			c->classSkills.push_back(skd);
		}
        classes[c->GetID()] = c;
    }
}

void Game::LoadAreas(Server * server)
{
    StoreQueryResult areares = server->sqlQueue->Read("select * from areas");
    if(areares.empty())
        return;

    Row row;
    StoreQueryResult::iterator iter;
    for(iter = areares.begin(); iter != areares.end(); ++iter)
    {
        row = *iter;
        Area * a = new Area(row["id"], (string)row["name"], row["pvp"], row["death_room"], row["level_range_low"], row["level_range_high"]);
        areas[a->GetID()] = a;
    }
}

void Game::LoadHelp(Server * server)
{
    StoreQueryResult helpres = server->sqlQueue->Read("select * from help");
    if(helpres.empty())
        return;

    Row row;
    StoreQueryResult::iterator iter;
    for(iter = helpres.begin(); iter != helpres.end(); ++iter)
    {
        row = *iter;
        Help * h = new Help();
		h->id = row["id"];
		h->title = (string)row["title"];
		h->search_string = (string)row["search_string"];
		h->text = (string)row["text"];
		helpIndex[h->id] = h;
    }
}

void Game::SaveQuests()
{
    std::map<int, Quest *>::iterator iter;
    for(iter = quests.begin(); iter != quests.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveSkills()
{
    std::map<int, Skill *>::iterator iter;
    for(iter = skills.begin(); iter != skills.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveNPCIndex()
{
    for(auto iter = npcIndex.begin(); iter != npcIndex.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveItems()
{
    std::map<int, Item *>::iterator iter;
    for(iter = items.begin(); iter != items.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveClasses()
{
    std::map<int, Class *>::iterator iter;
    for(iter = classes.begin(); iter != classes.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveAreas()
{
    std::map<int, Area *>::iterator iter;
    for(iter = areas.begin(); iter != areas.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveHelp()
{
	std::map<int, Help *>::iterator iter = helpIndex.begin();
	while(iter != helpIndex.end())
	{
		Help * h = (*iter).second;
		++iter;
		if(h->remove)
		{
			string sql = "DELETE FROM help where help.id="+Utilities::itos(h->id);
			Server::sqlQueue->Write(sql);
            helpIndex.erase(h->id);
			delete h;
			h = nullptr;
		}
		else
		{
			h->Save();
		}
	}
}

void Game::LoadNPCS(Server * server)
{
    StoreQueryResult characterres = server->sqlQueue->Read("select * from npcs");
    if(characterres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = characterres.begin(); i != characterres.end(); i++)
    {
        row = *i;
        NPCIndex * loaded = new NPCIndex(row["id"], (string)row["name"]);

		loaded->keywords = row["keywords"];
        loaded->title = row["title"];
        loaded->gender = row["gender"];
		loaded->race = row["race"];
        loaded->level = row["level"];
		loaded->maxHealth = row["health"];
		loaded->maxMana = row["mana"];
		loaded->maxRage = row["rage"];
		loaded->maxEnergy = row["energy"];
		loaded->armor = row["armor"];
        loaded->npcAttackSpeed = row["attack_speed"];
        loaded->npcDamageHigh = row["damage_high"];
        loaded->npcDamageLow = row["damage_low"];
		loaded->speechText = row["speechtext"];

		string flagtext = (string)row["flags"];
		int first, last;
		first = last = 0;
		while (first < (int)flagtext.length())
		{
			last = (int)flagtext.find(";", first);
			int flag = Utilities::atoi(flagtext.substr(first, last - first));
			Utilities::FlagSet(loaded->flags, flag);
			first = last + 1;
		}

		StoreQueryResult npcdropsres = server->sqlQueue->Read("select * from npc_drops where npc=" + Utilities::itos(loaded->id));
		for (auto j = npcdropsres.begin(); j != npcdropsres.end(); j++)
		{
			row = *j;
			string drops = (string)row["items"];
			int percent = row["percent"];

			NPCIndex::DropData dd;
			dd.percent = percent;
			
			first = last = 0;
			while (first < (int)drops.length())
			{
				last = (int)drops.find(";", first);
				dd.id.push_back(Utilities::atoi(drops.substr(first, last)));
				first = last + 1;
			}
			loaded->drops.push_back(dd);
		}
        npcIndex[loaded->id] = loaded;
    }
}

void Game::NewUser(std::shared_ptr<Client> client)
{
    User * u = new User(client);
	//client->SetUser(u);
    EnterCriticalSection(&userListCS); //Protect access to "users", NewUser is called from the socket AcceptThread
	users.push_back(u);
	LeaveCriticalSection(&userListCS);
    total_players_since_boot++;
    if((int)users.size() > max_players_since_boot)
        max_players_since_boot = (int)users.size();
	u->SendBW(login_art);
	u->Send(Server::MXP_WILL);
	u->Send(Server::GMCP_WILL);
	u->Send(Server::MCCP_WILL);
    u->Send("Player Name: ");
}

void Game::RemoveUser(User * user)
{
    /*std::list<User *>::iterator iter;
    iter = find( users.begin(), users.end(), user); 
    if(iter != users.end())
    {
        delete (*iter);
        users.remove(*iter);
    }*/
	user->remove = true;
}

void Game::RemoveUser(Client * client)
{
	EnterCriticalSection(&userListCS);
    std::list<User *>::iterator iter;
	for(iter = users.begin(); iter != users.end(); ++iter)
	{
		User * u = *iter;
		if(u->GetClient() == client)
		{
			/*delete (*iter);
			users.erase(iter);*/
            //u->Disconnect();
			u->remove = true;
			break;
		}
	}
	LeaveCriticalSection(&userListCS);
}

Player * Game::NewPlayer(std::string name, User * user)
{
    Player * ch = new Player(name, user);
    characters.push_front(ch);
    return ch;
}

NPC * Game::NewNPC(NPCIndex * index)
{
    NPC * ch = new NPC(index);
    characters.push_front(ch);
    return ch;
}

void Game::RemoveCharacter(Character * ch)
{
    ch->NotifySubscribers();
    ch->remove = true;
    //characters.remove(ch);
    //delete ch;
}

User * Game::DuplicatePlayerCheck(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->GetName(), name) 
            && ((*iter)->connectedState == User::CONN_PLAYING || (*iter)->connectedState > User::CONN_CONFIRM_NEW_PASSWORD))
            return (*iter);
    }
    return nullptr;
}

//Not case sensitive
User * Game::GetUserByPCName(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->GetName(), name))
            return (*iter);
    }
    return nullptr;
}

Player * Game::GetPlayerByName(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->GetName(), name))
            return (*iter)->character;
    }
    return nullptr;
}

Room * Game::GetRoom(int id)
{
    std::map<int,Room *>::iterator it = rooms.find(id);
    if(it != rooms.end())
        return it->second;
    return nullptr;
}

Skill * Game::GetSkill(int id)
{
    std::map<int,Skill *>::iterator it = skills.find(id);
    if(it != skills.end())
        return (*it).second;
    return nullptr;
}

Quest * Game::GetQuest(int id)
{
    std::map<int,Quest *>::iterator it = quests.find(id);
    if(it != quests.end())
        return (*it).second;
    return nullptr;
}

Area * Game::GetArea(int id)
{
    std::map<int,Area *>::iterator it = areas.find(id);
    if(it != areas.end())
        return (*it).second;
    return nullptr;
}

Help * Game::GetHelpByName(string name)
{
	std::map<int, Help*>::iterator iter;
    for(iter = helpIndex.begin(); iter != helpIndex.end(); ++iter)
    {
        if(!Utilities::str_cmp((*iter).second->title, name))
        {
            return (*iter).second;
        }
    }
    return nullptr;
}

Help * Game::GetHelp(int id)
{
	std::map<int,Help *>::iterator it = helpIndex.find(id);
    if(it != helpIndex.end())
        return (*it).second;
    return nullptr;
}

NPCIndex * Game::GetNPCIndex(int id)
{
    std::map<int,NPCIndex*>::iterator it = npcIndex.find(id);
    if(it != npcIndex.end())
        return (*it).second;
    return nullptr;
}

Item * Game::GetItem(int id)
{
    std::map<int,Item*>::iterator it = items.find(id);
    if(it != items.end())
        return (*it).second;
    return nullptr;
}

Room * Game::CreateRoomAnyID()
{
    int ctr = 1;
    std::map<int,Room *>::iterator iter;
    for(iter = rooms.begin(); iter != rooms.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Room * pRoom = new Room(ctr);
    pRoom->changed = true;
    rooms.insert(std::pair<int, Room *>(pRoom->id, pRoom));
    return pRoom;
}

Skill * Game::CreateSkillAnyID(string arg)
{
	std::map<int, Skill *>::iterator iter;
	iter = std::find_if(skills.begin(), skills.end(), Skill::CompareSkillToString(arg));
	
	if (iter != skills.end())
	{
		return nullptr;
	}

    int ctr = 1;
	
    for(iter = skills.begin(); iter != skills.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }
    Skill * pSkill = new Skill(ctr, arg);
    pSkill->changed = true;
    skills.insert(std::pair<int, Skill *>(pSkill->id, pSkill));
    return pSkill;
}

NPCIndex * Game::CreateNPCAnyID(std::string arg)
{
    int ctr = 1;
    for(auto iter = npcIndex.begin(); iter != npcIndex.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    NPCIndex * pChar = new NPCIndex(ctr, arg);
    pChar->changed = true;
    npcIndex.insert(std::pair<int, NPCIndex *>(pChar->id, pChar));
    return pChar;
}

Quest * Game::CreateQuestAnyID(string arg)
{
    int ctr = 1;
    std::map<int,Quest *>::iterator iter;
    for(iter = quests.begin(); iter != quests.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Quest * pQuest = new Quest(arg, ctr);
    pQuest->changed = true;
    quests.insert(std::pair<int, Quest *>(pQuest->id, pQuest));
    return pQuest;
}

Area * Game::CreateAreaAnyID(string name)
{
    int ctr = 1;
    std::map<int,Area *>::iterator iter;
    for(iter = areas.begin(); iter != areas.end(); ++iter)
    {
        if(ctr != iter->second->GetID())
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Area * pArea = new Area(ctr);
    pArea->name = name;
    pArea->changed = true;
    areas.insert(std::pair<int, Area *>(pArea->GetID(), pArea));
    return pArea;
}

Item * Game::CreateItemAnyID(string arg)
{
    int ctr = 1;
    std::map<int,Item *>::iterator iter;
    for(iter = items.begin(); iter != items.end(); ++iter)
    {
        if(ctr != iter->second->GetID())
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Item * pItem = new Item(arg, ctr);
    pItem->changed = true;
	items.insert(std::make_pair(pItem->GetID(), pItem));
    return pItem;
}

Room * Game::CreateRoom(int value)
{
    if(GetRoom(value) != nullptr)
    {
        LogFile::Log("error", "CreateRoom, room already exists");
        return nullptr;
    }
    Room * pRoom = new Room(value);
    pRoom->changed = true;

    rooms.insert(std::pair<int, Room *>(pRoom->id, pRoom));
    return pRoom;
}

Help * Game::CreateHelpAnyID(string name)
{
    int ctr = 1;
    std::map<int,Help *>::iterator iter;
    for(iter = helpIndex.begin(); iter != helpIndex.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Help * pHelp = new Help(name, name, ctr);
    pHelp->changed = true;
    helpIndex.insert(std::pair<int, Help *>(pHelp->id, pHelp));
    return pHelp;
}

//the total exp needed for any level
int Game::ExperienceForLevel(int level)
{
	if (level < 0)
		level = 0;
	else if (level > Game::MAX_LEVEL)
		level = Game::MAX_LEVEL;

	return (int)ceil(4.16667 * pow(level, 3) + 62.5 * pow(level, 2) + 458.333 * level - 525);
}

int Game::CalculateExperience(Character * ch, Character * victim)
{
	if (ch->HasGroup() && ch->GetGroup()->IsRaidGroup()) //zero experience in a raid group
		return 0;

	if (LevelDifficulty(ch->GetLevel(), victim->GetLevel()) == 0) //zero experience for "gray" level npcs
		return 0;
	
	double xp = ch->GetLevel() * 5 + 45;	//base xp
	if (victim->GetLevel() > ch->GetLevel())
	{
		double difference = victim->GetLevel() - ch->GetLevel();
		if (difference > 10)
			difference = 10.0;
		xp = xp * (1.0 + 0.05*difference);	//higher level victim, slightly higher exp
	}
	else if (victim->GetLevel() < ch->GetLevel())
	{
		double difference = ch->GetLevel() - victim->GetLevel();
		xp = xp * (1.0 - (difference / 7.0));		//lower level victim, less exp
	}

	//Group adjustment, equal share + 10%
	if (ch->HasGroup())
	{
		int group_size = ch->GetGroup()->GetMemberCount();
		xp /= group_size;
		xp += xp * 0.1;
	}

	return (int)ceil(xp);
}

int Game::LevelDifficulty(int ch, int vict)
{
    if(vict >= ch + 12)       //Red / ??
        return 5;
    else if(vict >= ch + 7)   //Red
        return 4;
    else if(vict >= ch + 5)   //Magenta
        return 3;
    else if(vict >= ch - 3)   //Yellow
        return 2;
    else if(vict >= ch - 6)   //Green
        return 1;
    else                            //Gray
        return 0;
}

std::string Game::LevelDifficultyColor(int leveldifficulty)
{
	switch (leveldifficulty)
	{
	case 0:
		return "|D";
	case 1:
		return "|G";
	case 2:
		return "|Y";
	case 3:
		return "|M";
	case 4:
	case 5:
		return "|R";
	}
	return "";
}

std::string Game::LevelDifficultyLightColor(int leveldifficulty)
{
	switch (leveldifficulty)
	{
	case 0:
		return "|d";
	case 1:
		return "|g";
	case 2:
		return "|y";
	case 3:
		return "|m";
	case 4:
	case 5:
		return "|r";
	}
	return "";
}

Character * Game::LoadNPCRoom(int id, Room * toroom)
{
    NPCIndex * charIndex = Game::GetGame()->GetNPCIndex(id);
    if(charIndex == nullptr)
    {
        //ch->Send("NPC " + arg2 + " does not exist.\r\n");
        return nullptr;
    }
    Character * newChar = Game::GetGame()->NewNPC(charIndex);
    newChar->ChangeRooms(toroom);
    return newChar;
}

Player * Game::GetPlayerWorld(Player * ch, string name)
{
    if(!Utilities::str_cmp(name, "self") || !Utilities::str_cmp(name, "me"))
		return ch;
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    std::list<User *>::iterator iter;
	for(iter = users.begin(); iter != users.end(); ++iter)
	{
        if((*iter)->connectedState == User::CONN_PLAYING && (*iter)->character != nullptr && !Utilities::str_str(name, (*iter)->character->GetName()))
		{
            if(ctr++ == numberarg)
			    return (*iter)->character;
		}
	}
	return nullptr;
}

void Game::GlobalMessage(string msg)
{
	std::list<User *>::iterator iter;
    for(iter = Game::GetGame()->users.begin(); iter != Game::GetGame()->users.end(); ++iter)
    {
        if((*iter)->connectedState == User::CONN_PLAYING)
		    (*iter)->Send(msg);
    }
}

Class * Game::GetClassByName(string name)
{
    std::map<int, Class*>::iterator iter;
    for(iter = classes.begin(); iter != classes.end(); ++iter)
    {
        if(!Utilities::str_cmp((*iter).second->name, name))
        {
            return (*iter).second;
        }
    }
    return nullptr;
}

Class * Game::GetClass(int id)
{
    std::map<int,Class *>::iterator it = classes.find(id);
    if(it != classes.end())
        return (*it).second;
    return nullptr;
}
/*
//DIDNT WORK  BOOOOOOOOOOO
template <typename T1>
int Game::DoSearch(T1 table_name, std::string field_name, std::string argument, int data_type, std::string & result)
{
    std::map<int, T1 *>::iterator iterator;
    int value = 0;
    if(data_type == 1)
        value = Utilities::atoi(argument);

    int results_found = 0;
    for(iterator = table_name.begin(); iterator != table_name.end(); iterator++)
    {
        if(data_type == 1)
        {
            if(*(iterator->second->intTable[field_name]) == value)
            {
                result += "[" + Utilities::itos(iterator->second->id) + "] " + iterator->second->name + "\r\n";
                ++results_found;
            }
        }
        else if(data_type == 2)
        {
            if(!Utilities::str_str(argument, *(iterator->second->stringTable[field_name])))
            {
                result += "[" + Utilities::itos(iterator->second->id) + "] " + iterator->second->name + "\r\n";
                ++results_found;
            }
        }
    }
    return results_found;
}
*/

bool Game::SearchComparisonInt(int field_value, int search_value, int conditional_type)
{
    switch(conditional_type)
    {
        case 1: 
            if(field_value == search_value)
                return true;
            return false;
        case 2:
            if(field_value != search_value)
                return true;
            return false;
        case 3:
            if(field_value < search_value)
                return true;
            return false;
        case 4:
            if(field_value > search_value)
                return true;
            return false;
        case 5:
            return false;
        default:
            return false;
    }
    return false;
}

bool Game::SearchComparisonDouble(double field_value, double search_value, int conditional_type)
{
	switch (conditional_type)
	{
	case 1:
		if (field_value == search_value)	//hmm, not likely for a double
			return true;
		return false;
	case 2:
		if (field_value != search_value)
			return true;
		return false;
	case 3:
		if (field_value < search_value)
			return true;
		return false;
	case 4:
		if (field_value > search_value)
			return true;
		return false;
	case 5:
		return false;
	default:
		return false;
	}
	return false;
}

bool Game::SearchComparisonString(string field_value, string search_value, int conditional_type)
{
    switch(conditional_type)
    {
        case 1: 
            if(!Utilities::str_cmp(field_value, search_value))
                return true;
            return false;
        case 2:
            if(Utilities::str_cmp(field_value, search_value))
                return true;
            return false;
        case 3:
            return false;
        case 4:
            return false;
        case 5:
            if(!Utilities::str_str(search_value, field_value))
                return true;
            return false;
        default:
            return false;
    }
    return false;
}

int Game::Search(string table_name, string field_name, int conditional_type, string argument, int data_type, string & result)
{
	//There has to be a way to reduce the code replication in this function but I haven't found it
    /*
    template experiment didn't work but then I know jack about templates
    if(!Utilities::str_cmp(table_name, "rooms"))
    {
        std::map<int, Room *>::iterator iter;
        return DoSearch(rooms, field_name, argument, data_type, result);
    }
    else if(!Utilities::str_cmp(table_name, "items"))
    {
        std::map<int, Item *>::iterator iter;
        return DoSearch(itemIndex, field_name, argument, data_type, result);
    }
    else if(!Utilities::str_cmp(table_name, "characters"))
    {
        std::map<int, Character *>::iterator iter;
        return DoSearch(characters, field_name, argument, data_type, result);
    }*/

    //TODO: typeid keyword!!!!!!!

    int value = 0;
	int whichtable = 0;
    if(data_type == 1)
        value = Utilities::atoi(argument);

    int results_found = 0;
    if(!Utilities::str_cmp(table_name, "rooms"))
    {
		if (rooms.begin() == rooms.end())
		{
			result += "No rooms in the room list.\r\n";
			return 0;
		}
		if (data_type == 1 && rooms.begin()->second->intTable.find(field_name) != rooms.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 2 && rooms.begin()->second->stringTable.find(field_name) != rooms.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\r\n";
			return 0;
		}

		std::map<int, Room *>::iterator iter;
		for (iter = rooms.begin(); iter != rooms.end(); iter++)
		{
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\r\n";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\r\n";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "items"))
    {
		if (items.begin() == items.end())
		{
			result += "No items in the item list.\r\n";
			return 0;
		}
		if (data_type == 1 && items.begin()->second->intTable.find(field_name) != items.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && items.begin()->second->doubleTable.find(field_name) != items.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if(data_type == 2 && items.begin()->second->stringTable.find(field_name) != items.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\r\n";
			return 0;
		}

        std::map<int, Item *>::iterator iter;
        for(iter = items.begin(); iter != items.end(); iter++)
        {
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->GetID()) + "] " + iter->second->GetName() + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\r\n";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->GetID()) + "] " + iter->second->GetName() + ":  " + Utilities::dtos(*(iter->second->doubleTable[field_name]), 2) + "\r\n";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->GetID()) + "] " + iter->second->GetName() + ":  " + *(iter->second->stringTable[field_name]) + "\r\n";
					++results_found;
				}
				break;
			}
        }
        return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "npcs"))
    {
		if (npcIndex.begin() == npcIndex.end())
		{
			result += "No npcs in the npc index list.\r\n";
			return 0;
		}
		if (data_type == 1 && npcIndex.begin()->second->intTable.find(field_name) != npcIndex.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && npcIndex.begin()->second->doubleTable.find(field_name) != npcIndex.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if (data_type == 2 && npcIndex.begin()->second->stringTable.find(field_name) != npcIndex.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\r\n";
			return 0;
		}

		std::map<int, NPCIndex *>::iterator iter;
		for (iter = npcIndex.begin(); iter != npcIndex.end(); iter++)
		{
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\r\n";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::dtos(*(iter->second->doubleTable[field_name]), 2) + "\r\n";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\r\n";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "quests"))
    {
		if (quests.begin() == quests.end())
		{
			result += "No quests in the quest list.\r\n";
			return 0;
		}
		if (data_type == 1 && quests.begin()->second->intTable.find(field_name) != quests.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && quests.begin()->second->doubleTable.find(field_name) != quests.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if (data_type == 2 && quests.begin()->second->stringTable.find(field_name) != quests.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\r\n";
			return 0;
		}

		std::map<int, Quest *>::iterator iter;
		for (iter = quests.begin(); iter != quests.end(); iter++)
		{
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\r\n";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::dtos(*(iter->second->doubleTable[field_name]), 2) + "\r\n";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\r\n";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "skills"))
    {
		if (skills.begin() == skills.end())
		{
			result += "No skills in the skill list.\r\n";
			return 0;
		}
		if (data_type == 1 && skills.begin()->second->intTable.find(field_name) != skills.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && skills.begin()->second->doubleTable.find(field_name) != skills.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if (data_type == 2 && skills.begin()->second->stringTable.find(field_name) != skills.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\r\n";
			return 0;
		}

		std::map<int, Skill *>::iterator iter;
		for (iter = skills.begin(); iter != skills.end(); iter++)
		{
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\r\n";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + Utilities::dtos(*(iter->second->doubleTable[field_name]), 2) + "\r\n";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + *(iter->second->stringTable[field_name]) + "\r\n";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else
    {
        result += "Invalid table name.\r\n";
    }
    return 0;
}