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

using namespace std;

extern "C" 
{
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

const std::string login_art = R"(
                                               _   __,----'~~~~~~~~~`-----.__
                                        .  .    `//====-              ____,-'~`
                        -.            \_|// .   /||\\  `~~~~`---.___./
                  ______-==.       _-~o  `\/    |||  \\           _,'`
            __,--'   ,=='||\=_    ;_,_,/ _-'|-   |`\   \\        ,'
         _-'      ,='    | \\`.    '',/~7  /-   /  ||   `\.     /
       .'       ,'       |  \\  \_  "  /  /-   /   ||      \   /
      / _____  /         |     \\.`-_/  /|- _/   ,||       \ /
     ,-'     `-|--'~~`--_ \     `==-/  `| \'--===-'       _/`
               '         `-|      /|    )-'\~'      _,--"'
                           '-~^\_/ |    |   `\_   ,^             /\
                                /  \     \__   \/~               `\__
                            _,-' _/'\ ,-'~____-'`-/                 ``===\
                           ((->/'    \|||' `.     `\.  ,                _||
             ./                       \_     `\      `~---|__i__i__\--~'_/
            <_n_                     __-^-_    `)  \-.______________,-~'
             `0'\)                  ///,-'~`__--^-  |-------~~~~^'
             /^>                           ///,--~`-\
            `  `
)";

double Game::currentTime = 0;

Game * Game::GetGame()
{
    /*static Game * theGame = NULL;
    if(theGame == NULL)
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
    deleteme = NULL;
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
    std::map<int, Skill *>::iterator iter;
    for(iter = skills.begin(); iter != skills.end(); ++iter)
    {
        delete (*iter).second;
    }
    skills.clear();

    std::map<int, Character *>::iterator iter2;
    for(iter2 = characterIndex.begin(); iter2 != characterIndex.end(); ++iter2)
    {
        delete (*iter2).second;
    }
    characterIndex.clear();

    while(!characters.empty())
    {
        delete characters.front();
        characters.pop_front();
    }
    while(!users.empty())
    {
        delete users.front();
        users.pop_front();
    }
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
    for(iter6 = itemIndex.begin(); iter6 != itemIndex.end(); ++iter6)
    {
        delete (*iter6).second;
    }
    itemIndex.clear();

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

			if(user == NULL)
				continue;

			if (user->lastInput + IDLE_TIMEOUT <= Game::currentTime)
			{
				if (!user->character
					|| (user->character && user->character->player && !user->character->player->IMMORTAL()))
				{
					user->Send("Idle timeout exceeded. Disconnecting.\n\r");
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
				user->lastInput = Game::currentTime; //idle timeout
                string command = user->commandQueue.front();

                //Check for telnet IAC response for MXP and MCCP
                if(command.length() > 0 && command[0] == Server::IAC[0])
                {
                    string iac_response = command.substr(0, 3);
                    command = command.substr(3, command.length()-3);

                    if(iac_response == Server::MXP_DO)
                    {
                        //turn on mxp
                        user->Send("MXP Enabled\n\r");
						//Send immediately (only really necessary for MCCP)
						server->deliver(user->GetClient(), Server::MXP_START);
						server->deliver(user->GetClient(), MXP_LOCKLOCKED);
                        //user->Send(MXP_START);
                        //user->Send(MXP_LOCKLOCKED);
                        user->mxp = true;
                    }
					else if(iac_response == Server::GMCP_DO)
					{
						user->Send("GMCP Enabled\n\r");
						server->deliver(user->GetClient(), Server::GMCP_START);
						user->gmcp = true;
					}
					else if(iac_response == Server::MCCP_DO)
					{
						//turn on mccp
						user->Send("MCCP2 Enabled\n\r");
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
						//user->Send(MCCP_START);
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

        //todo: we can probably accept new users while in the WorldUpdate
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
                        user->character->NotifyListeners();
                        characters.remove(user->character);
                        //RemoveCharacter(user->character);
                    }
                    //RemoveUser(user);
					//user->Disconnect();
                    delete (*iter);
                    iter = users.erase(iter);
                    user = NULL;
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
					user->z_strm.avail_in = out.length();
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
					if (user->connectedState == User::CONN_PLAYING && user->character->level > 1) //don't save fresh characters
					{
						user->character->SaveSpellAffects();
						user->character->SaveCooldowns();
						user->character->Save();
					}

					user->character->Message(user->character->name + " has left the game.", Character::MSG_ROOM_NOTCHAR);
					user->character->ExitCombat();
					user->character->ClearTarget();
					user->character->ChangeRooms(NULL);

                    user->character->NotifyListeners();
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

	

    //Server is going down! Cleanup in main() with the server
    
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
    while(iter != characters.end()) //TODO, is this ridiculous?
    {
        Character * curr = (*iter);

        if(curr->remove)
        {
            delete (*iter);
            iter = characters.erase(iter);
            continue;
        }
        ++iter;

        if(curr->IsCorpse() || curr->IsGhost())
        {
            continue;
        }
        //Tick, every 2 seconds
        if(doTwoSecondTick)
        {
            //Stat regeneration
            if(!curr->combat && curr->health < curr->maxHealth)
            {
				if (curr->IsNPC())
					curr->SetHealth(curr, curr->maxHealth); //NPC's heal immediately out of combat
				else
					curr->AdjustHealth(NULL, (int)ceil(curr->GetLevel()*0.1 + 2.5)); 
            }
			if(curr->mana < curr->maxMana && curr->lastSpellCast + 5.0  <= Game::currentTime)
            {
                 //if more than 5 seconds since last cast, regen 20% of wisdom as mana
			    curr->AdjustMana(curr, (int)ceil(curr->wisdom * 0.2));
            }
			if(curr->energy < curr->maxEnergy)
            {
				curr->AdjustEnergy(curr, 20); //1 energy per .1 second regen
            }
			//Rage decay, 1 per second
			if (!curr->combat && curr->rage > 0)
			{
				curr->AdjustRage(curr, -2);
			}

            //Check NPC TIMER triggers
            Trigger * trig = NULL;
            int ctr = 0;
            while((trig = curr->GetTrigger(ctr, Trigger::TIMER)) != NULL)
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
                    //string nil = trig->GetFunction() + " = nil;";
                    //luaL_dostring(Server::luaState, nil.c_str());
					Server::lua.script(trig->GetScript().c_str());
					Server::lua[func.c_str()](curr);
                    //luaL_dostring(Server::luaState, trig->GetScript().c_str());
                    //luabind::call_function<void>(Server::luaState, func.c_str(), curr);
                }
                catch(const std::exception & e)
			    {
				    LogFile::Log("error", e.what());
				    //const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				    /*if(logstring != NULL)
					    LogFile::Log("error", logstring);*/
			    }
                catch(...)
	            {
		            LogFile::Log("error", "call_function unhandled exception ENTER_PC ENTER_NPC");
	            }
            }
        }
        
        //Delay Update
        if(curr->delay_active && curr->delay <= currentTime)
        {
            curr->delay_active = false;
            (*curr->delayFunction)(curr->delayData);
        }
        //Combat update
        if(curr->combat)
        { //in addition to our list of aggressors will need to keep a list of chars whose aggro list we're on (for npc combat) (2/28/18: don't know what this means or why)
            if(!curr->GetTarget() || curr->GetTarget() == curr)
            {  //Turn off auto attack. cmd_target should take care of this, but just in case
                curr->meleeActive = false;
            }
            else if(curr->meleeActive && curr->GetTarget()->room == curr->room && !curr->delay_active) //No auto attack while casting
            {
                curr->AutoAttack(curr->GetTarget());
            }
            else if(!curr->meleeActive && curr->GetTarget() && curr->GetTarget()->GetTarget() && curr->GetTarget()->GetTarget() == curr
                && curr->GetTarget()->meleeActive && curr->GetTarget()->room == curr->room)
            { //So... If we're not attacking, we have a target, our target's target is us, and is attacking us, and theyre in the same room, start attacking them back
                curr->AutoAttack(curr->GetTarget());
            }

			//Players exit combat after 5 seconds of no activity AND (TODO) when we have no npcs on our threat list
            if(curr->player && curr->player->lastCombatAction + 5 <= currentTime && !curr->CheckThreatCombat())
            {   
                curr->ExitCombat();
            }

			//Threat management, chasing/leashing (NPC's start chasing AFTER movementspeed delay)
            if(curr->IsNPC() && curr->GetTopThreat()) 
            {
				if (!curr->movementQueue.empty()) //We have a movement pending
				{
					if(curr->CanMove()) //Move! (checks movespeed auras and movement speed timestamp)
					{
						curr->movementQueue.pop_front();
						//track target... Give up leashDistance rooms away from target... (todo, separate this from leashdist?)
						int leashdist = Reset::RESET_LEASH_DEFAULT;
						if (curr->reset && curr->reset->leashDistance != 0)
							leashdist = curr->reset->leashDistance;
						Exit::Direction chasedir = FindDirection(curr, curr->GetTopThreat(), leashdist);
						if (chasedir != Exit::DIR_LAST)
						{
							//record the path we take for backtracking
							curr->leashPath.push_back(std::make_pair(curr->room, chasedir));
							curr->Move(chasedir);
							if(curr->GetTopThreat()->room == curr->room)
								curr->EnterCombat(curr->GetTopThreat());
						}
						else
						{
							//target more than leashDistance rooms away, leash!
							std::pair<Room *, int> path;
							curr->ExitCombat();
							curr->ClearTarget();
							while (!curr->leashPath.empty())
							{
								path = curr->leashPath.back();
								curr->leashPath.pop_back();
								
								//Fake enter/leave messages. We can't use ->Move() since that tests for valid exits
								//  among other things (think leashing back through 1 way exits)
								curr->Message(curr->name + " leaves " + Exit::exitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
								curr->ChangeRooms(path.first);
								curr->Message(curr->name + " has arrived from " 
									+ ((path.second != Exit::DIR_UP && path.second != Exit::DIR_DOWN) ? "the " : "") 
									+ Exit::reverseExitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
							}
						}
					}
				}
                else if(curr->GetTopThreat()->room != curr->room && curr->movementQueue.empty()) //We need to chase threat target, and not already pending a move...
                {
					//Decide if we should try to chase based on how far we are from our reset
					int leashdist = Reset::RESET_LEASH_DEFAULT;
					if (curr->reset && curr->reset->leashDistance != 0)
						leashdist = curr->reset->leashDistance;
					if (curr->leashPath.size() >= leashdist) //Leash!
					{
						curr->ExitCombat();
						curr->ClearTarget();
						std::pair<Room *, int> path;
						while (!curr->leashPath.empty())
						{
							path = curr->leashPath.back();
							curr->leashPath.pop_back();

							//Fake enter/leave messages. We can't use ->Move() since that tests for valid exits
							//  among other things (think leashing back through 1 way exits)
							curr->Message(curr->name + " leaves " + Exit::exitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
							curr->ChangeRooms(path.first);
							curr->Message(curr->name + " has arrived from "
								+ ((path.second != Exit::DIR_UP && path.second != Exit::DIR_DOWN) ? "the " : "")
								+ Exit::reverseExitNames[Exit::exitOpposite[path.second]] + ".", Character::MSG_ROOM_NOTCHAR);
						}
					}
					else
					{
						//Just push a placeholder that indicates we have a movement pending, since we're going to "track" the target after movespeed delay
						curr->movementQueue.push_back(nullptr);
						curr->lastMoveTime = currentTime; //queue up the next move after delay
						curr->meleeActive = false;		  //target isn't in the room...
					}
                }
				//New top threat, change target
                if(curr->GetTopThreat() && curr->GetTopThreat() != curr->GetTarget())
                {
                    curr->SetTarget(curr->GetTopThreat());
                }
            }
        }
        
        //Buff/debuff update
        if(!curr->buffs.empty())
        {
            std::list<SpellAffect*>::iterator buffiter = curr->buffs.begin();
            while(buffiter != curr->buffs.end())
            {
                SpellAffect * sa = (*buffiter);
                if(sa->ticksRemaining > 0 
                   && Game::currentTime > sa->appliedTime + ((sa->ticks - sa->ticksRemaining+1)*(sa->duration / sa->ticks)))
                {
                    string func = sa->skill->function_name + "_tick";
                    if(!sa->caster)
                    {
                        sa->caster = GetCharacterByPCName(sa->casterName);
                    }
                    sa->ticksRemaining--;
                    try
                    {
						Server::lua[func.c_str()](sa->caster, curr, sa);
                        //luabind::call_function<void>(Server::luaState, func.c_str(), sa->caster, curr, sa);
                    }
                    catch(const std::exception & e)
			        {
				        LogFile::Log("error", e.what());
				        /*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				        if(logstring != NULL)
					        LogFile::Log("error", logstring);*/
			        }
                    catch(...)
	                {
		                LogFile::Log("error", "call_function unhandled exception _tick");
	                }
                    
                    //Reset the loop, who knows what happened to our debuffs during the tick
                    buffiter = curr->buffs.begin();
                    continue;
                }
                if(!sa->remove_me && sa->duration > 0 && Game::currentTime - sa->appliedTime > sa->duration) //Expired
                {
                    string func = sa->skill->function_name + "_remove";
                    sa->remove_me = true;
                    if(!sa->caster)
                    {
                        sa->caster = GetCharacterByPCName(sa->casterName);
                    }
                    try
                    {
						Server::lua[func.c_str()](sa->caster, curr, sa);
                        //luabind::call_function<void>(Server::luaState, func.c_str(), sa->caster, curr, sa);
                    }
                    catch(const std::exception & e)
			        {
				        LogFile::Log("error", e.what());
				        /*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				        if(logstring != NULL)
					        LogFile::Log("error", logstring);*/
			        }
                    catch(...)
	                {
		                LogFile::Log("error", "call_function unhandled exception _remove");
	                }
                    //Reset the loop, who knows what happened to our buffs during the remove
                    sa->remove_me = true;
                    buffiter = curr->buffs.begin();
                    continue;
                } 
                //else
                ++buffiter;
            }
            //Loop them again to remove any flagged
            buffiter = curr->buffs.begin();
            while(buffiter != curr->buffs.end())
            {
                if((*buffiter)->remove_me) //Expired
                {
                    delete (*buffiter);
                    buffiter = curr->buffs.erase(buffiter);
                }
                else
                {
                    ++buffiter;
                }
            }
        }
        if(!curr->debuffs.empty())
        {
            std::list<SpellAffect*>::iterator debuffiter = curr->debuffs.begin();
            while(debuffiter != curr->debuffs.end())
            {
                SpellAffect * sa = (*debuffiter);

                if(sa->ticksRemaining > 0 
                   && Game::currentTime > sa->appliedTime + ((sa->ticks - sa->ticksRemaining+1)*(sa->duration / sa->ticks)))
                {
                    sa->ticksRemaining--;
                    string func = sa->skill->function_name + "_tick";
                    if(!sa->caster)
                    {
                        sa->caster = GetCharacterByPCName(sa->casterName);
                    }
                    try
                    {
						Server::lua[func.c_str()](sa->caster, curr, sa);
                        //luabind::call_function<void>(Server::luaState, func.c_str(), sa->caster, curr, sa);
                    }
                    catch(const std::exception & e)
			        {
				        LogFile::Log("error", e.what());
				        /*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				        if(logstring != NULL)
					        LogFile::Log("error", logstring);*/
			        }
                    catch(...)
	                {
		                LogFile::Log("error", "call_function unhandled exception _tick");
	                }
                    
                    //Reset the loop, who knows what happened to our debuffs during the tick
                    debuffiter = curr->debuffs.begin();
                    continue;
                }
                if(!sa->remove_me && sa->duration > 0 && Game::currentTime - sa->appliedTime > sa->duration) //Expired
                {
                    sa->remove_me = true;
                    string func = sa->skill->function_name + "_remove";
                    if(!sa->caster)
                    {
                        sa->caster = GetCharacterByPCName(sa->casterName);
                    }
                    try
                    {
						Server::lua[func.c_str()](sa->caster, curr, sa);
                        //luabind::call_function<void>(Server::luaState, func.c_str(), sa->caster, curr, sa);
                    }
                    catch(const std::exception & e)
			        {
				        LogFile::Log("error", e.what());
				        /*const char * logstring = lua_tolstring(Server::luaState, -1, NULL);
				        if(logstring != NULL)
					        LogFile::Log("error", logstring);*/
			        }
                    catch(...)
	                {
		                LogFile::Log("error", "call_function unhandled exception _remove");
	                }
                    //Reset the loop, who knows what happened to our debuffs during the remove
                    sa->remove_me = true;
                    debuffiter = curr->debuffs.begin();
                    continue;
                }
                //else
                ++debuffiter;
            }
            //Loop them again to remove any flagged
            debuffiter = curr->debuffs.begin();
            while(debuffiter != curr->debuffs.end())
            {
                if((*debuffiter)->remove_me) //Expired
                {
                    delete (*debuffiter);
                    debuffiter = curr->debuffs.erase(debuffiter);
                }
                else
                {
                    ++debuffiter;
                }
            }
        }
        //Target range update
        if(!(curr->player && curr->player->IMMORTAL()) && curr->GetTarget() && curr->GetTarget()->room != curr->room)
        {
            //Allow target to remain active in same room and adjacent rooms
            bool found = false;
            for(int i = 0; i < Exit::DIR_LAST; i++)
            {
                if(curr->room->exits[i] && curr->room->exits[i]->to == curr->GetTarget()->room)
                {
                    found = true;
                    break;
                }
            }
            if(!found) //Clear the target
            {
                curr->Send("Target out of range.\n\r");
                curr->ClearTarget();
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
                    if(currReset->type == 1 && currReset->npc == NULL) //npc reset and npc no longer exists
                    {
                        currReset->lastReset = Game::currentTime;
                        //load it
                        Character * charIndex = Game::GetGame()->GetCharacterIndex(currReset->npcID);
                        if(charIndex == NULL)
                        {
                            LogFile::Log("error", "Reset " + Utilities::itos(currReset->id) + " in room " + Utilities::itos(currRoom->id) + ": npc does not exist.");
                            continue;
                        }
                        Character * newChar = Game::GetGame()->NewCharacter(charIndex);
                        newChar->ChangeRooms(currRoom);
                        newChar->Message("|W" + newChar->name + " has arrived.|X", Character::MSG_ROOM_NOTCHAR);
                        newChar->reset = currReset;
                        //LogFile::Log("status", "Adding listener to " + newChar->name + " of reset id " + Utilities::itos(currReset->id));
                        newChar->AddListener(currReset);
                        currReset->npc = newChar;
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
                user->character = new Character(arg1, user);
                user->character->player->password = server->SQLSelectPassword(arg1);
                user->connectedState = User::CONN_GET_OLD_PASSWORD;
			}
			else if((tempUser = Game::GetGame()->GetUserByPCName(arg1)) != NULL 
                    && tempUser->connectedState <= User::CONN_CONFIRM_NEW_PASSWORD) 
			{   //player exists, but hasnt finished creating
				user->Send("A character is currently being created with that name.\n\r");
				user->SetDisconnect();
			}
            else if(tempUser != NULL && tempUser->connectedState > User::CONN_CONFIRM_NEW_PASSWORD)
			{   //player exists, but hasnt been saved yet
				user->Send("Welcome Back " + arg1 + "!\r\nEnter Password: ");
                user->connectedState = User::CONN_GET_OLD_PASSWORD;
                user->character = new Character(arg1, user);
                user->character->player->password = tempUser->character->player->password;
                LogFile::Log("status", "Reconnecting unsaved character : " + user->character->name);
			}
			else // user dosn't exist - create and ask for password
			{
				user->Send("User " + arg1 + " Does Not Exist - Creating\r\nEnter Password:\r\n");
                user->connectedState = User::CONN_GET_NEW_PASSWORD;
                user->character = Game::GetGame()->NewCharacter(arg1, user);
                LogFile::Log("status", "Creating character : " + user->character->name);
			}
			break;
		}

        case User::CONN_GET_OLD_PASSWORD:
		{
            if(server->EncryptDecrypt(arg1) != user->character->player->password) //never decrypt password, just encrypt the input
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

            User * existingUser = Game::GetGame()->DuplicatePlayerCheck(user->character->name);

            if(existingUser == NULL)
            {   //Need to load from the db
                LogFile::Log("status", "Loading character : " + user->character->name);
                Character * c = Character::LoadPlayer(user->character->name, user);
                delete user->character;
                user->character = c;
                user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
                user->connectedState = User::CONN_MENU;
            }
            else
            {   //Already loaded player, swap with tempUser
                delete user->character;
                user->character = existingUser->character;
                user->character->player->user = user;
                existingUser->character = NULL;
                if(existingUser->IsConnected())
                {
                    existingUser->Send("\n\rMultiple login detected. Disconnecting...\n\r");
                    existingUser->SetDisconnect();

                }
                user->connectedState = existingUser->connectedState;
                if(user->connectedState >= User::CONN_CHANGEPW1 && user->connectedState <= User::CONN_DELETE2)
                {
					//If we took over someone not CONN_PLAYING, drop them in CONN_MENU
                    user->connectedState = User::CONN_MENU;
                }
                user->Send("Reconnecting...\r\n");
                user->character->Message(user->character->name + " has reconnected.", Character::MSG_ROOM_NOTCHAR);
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
            user->character->player->password = server->EncryptDecrypt(arg1);
            user->connectedState = User::CONN_CONFIRM_NEW_PASSWORD;
            user->Send("Confirm Password: \r\n");
            break;
		}

        case User::CONN_CONFIRM_NEW_PASSWORD:
		{
		    if(server->EncryptDecrypt(arg1) == user->character->player->password)
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
				user->character->player->password.clear();
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
				user->Send("|YInvalid choice.|X\n\r");
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
                user->character->player->currentClass = GetClassByName(arg1);
                user->connectedState = User::CONN_CONFIRM_CLASS;
            }
            else
            {
				user->Send("|YInvalid choice.|X\n\r");
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
				user->character->player->AddClass(user->character->player->currentClass->id, 1);
				user->character->AddClassSkills();
				//function-ize the default items
				string classitems = user->character->player->currentClass->items;
				int first = 0, last = 0;
				while (first < (int)classitems.length())
				{
					last = (int)classitems.find(";", first);
					if (last == std::string::npos)
						break;
					int id = Utilities::atoi(classitems.substr(first, last - first));
					first = last + 1;
					Item * itemIndex = GetItemIndex(id);
					if (itemIndex == NULL)
					{
						LogFile::Log("error", "Item " + Utilities::itos(id) + " does not exist.");
						continue;
					}
					itemIndex = user->character->player->NewItemInventory(itemIndex);
					user->character->player->EquipItemFromInventory(itemIndex, user->character->player->GetEquipLocation(itemIndex));
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
				user->character->gender = 1;
			}
			else if (!Utilities::str_cmp(arg1, "f"))
			{
				valid = true;
				user->character->gender = 2;
			}
			else
			{
				user->Send("|YEnter character gender: 'M' / 'F' :|X ");
				user->connectedState = User::CONN_GET_GENDER;
			}

			if (valid)
			{
				user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
				user->connectedState = User::CONN_MENU;
			}
			break;
		}

        case User::CONN_CHANGEPW1:
        {
            if(server->EncryptDecrypt(arg1) == user->character->player->password)
			{
                user->Send("Enter new password: ");
                user->connectedState = User::CONN_CHANGEPW2;
            }
            else
            {
                user->Send("Incorrect password.\n\r\n\r");
                user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
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
            user->character->player->pwtemp = arg1;
            user->Send("Confirm new password: ");
            user->connectedState = User::CONN_CHANGEPW3;
            
            break;
        }

        case User::CONN_CHANGEPW3:
        {
            if(arg1 == user->character->player->pwtemp)
            {
                user->Send("Password changed.\n\r");
                user->character->player->password = server->EncryptDecrypt(argument);
				Server::sqlQueue->Write("UPDATE players SET password = '" + user->character->player->password + "' WHERE name='" + user->character->name + "'");
                user->character->player->pwtemp.clear();
            }
            else
            {
                user->Send("Password doesn't match, NOT changed.\n\r");
                user->character->player->pwtemp.clear();
            }
            user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
            user->connectedState = User::CONN_MENU;
            break;
        }

        case User::CONN_DELETE1:
        {
            if(server->EncryptDecrypt(arg1)== user->character->player->password)
            {
                user->Send("|RDelete this character? (y/n):|X ");
                user->connectedState = User::CONN_DELETE2;
            }
            else
            {
                user->Send("Incorrect password.\n\r\n\r");
                user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
                user->connectedState = User::CONN_MENU;
            }
            break;
        }

        case User::CONN_DELETE2:
        {
            if(arg1[0] == 'y' || arg1[0] == 'Y')
            {
                Server::sqlQueue->Write("delete from players where name='" + user->character->name + "';");
                Server::sqlQueue->Write("delete from player_spell_affects where player='" + user->character->name + "';");
				Server::sqlQueue->Write("delete from player_class_data where player='" + user->character->name + "';");
				Server::sqlQueue->Write("delete from player_completed_quests where player='" + user->character->name + "';");
				Server::sqlQueue->Write("delete from player_cooldowns where player='" + user->character->name + "';");
				Server::sqlQueue->Write("delete from player_active_quests where player='" + user->character->name + "';");
				Server::sqlQueue->Write("delete from player_inventory where player='" + user->character->name + "';");
				user->SetDisconnect();
            }
            else
            {
                user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
                user->connectedState = User::CONN_MENU;
            }
            break;
        }
        
        case User::CONN_MENU:
        {
            if(arg1.empty())
            {
                user->Send("|B.|C1 |MEnter world\n\r |C2|B.|MChange password\n\r|B.|C3 |MDelete this character\n\r |C4|B.|MQuit|X\n\r: ");
                user->connectedState = User::CONN_MENU;
                break;
            }
            if(arg1[0] == '1') //enter game
            {
                user->Send("|MThere are currently |X" + Utilities::itos((int)rooms.size()) + " |Munique locations, |X" +
                    Utilities::itos((int)characters.size() - (int)users.size()) + "|M non player characters and |X" +
                    Utilities::itos((int)quests.size()) + "|M quests.|X\n\r");
                user->Send("|MThere are currently |X" + Utilities::itos((int)users.size()) + "|M players online.|X\n\r");
                user->Send("|MThere have been |X" + Utilities::itos(total_players_since_boot) + 
                    "|M players connected to the server since last boot.|X\n\r");
                user->Send("|MThe most players that have been online at one time since last boot is |X" + 
                    Utilities::itos(max_players_since_boot) + "|M.|X\n\r");
                user->Send("|MYou are player [|X" + Utilities::itos(++total_past_connections) + "|M] connected since May 19th, 2010.|X\n\r\n\r");
                //user->Send("Logged In!\r\n");
                if(user->character->room == NULL)
                {
					Room * toroom = GetRoom(newplayerRoom);
					if (!toroom)
					{
						user->character->ChangeRooms(rooms.begin()->second);
					}
					else
					{
						user->character->ChangeRoomsID(newplayerRoom);
						if(user->character->player)
							user->character->player->recall = newplayerRoom;
					}
                }
                else
                {
                    user->character->ChangeRooms(user->character->room);
                }
                user->connectedState = User::CONN_PLAYING;
                user->character->LoadSpellAffects();
                user->character->LoadCooldowns();
                user->character->Message(user->character->name + " has entered the game.", Character::MSG_ROOM_NOTCHAR);				
                cmd_look(user->character, "");
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
    ifstream in;
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
    ofstream out;
    out.open("serverstats.txt");
    out << total_past_connections << endl;
    out << newplayerRoom << endl;
}

void Game::LoadRooms(Server * server)
{
    StoreQueryResult roomres = server->sqlQueue->Read("select * from rooms");
	if (roomres.empty())
	{
		LogFile::Log("error", "Warning! No rooms loaded from database, creating default room. Set the new player room number in serverstats.txt line 2");
		Room * r = new Room(1, "The One Room", "One room to rule them all.\n\r");
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
            if(pRoom == NULL)
                continue;

            pRoom->AddTrigger(new_trig);
        }
        if((int)row["parent_type"] == Trigger::PARENT_NPC)
        {
            Character * charindex = GetCharacterIndex(row["parent_id"]);
            if(charindex == NULL)
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
        if(r == NULL)
            continue;

        Reset * reset = new Reset();
        reset->id = row["id"];
        reset->interval = row["interval"];
        reset->leashDistance = row["leash_dist"];
        reset->type = row["type"];
        reset->wanderDistance = row["wander_dist"];
        reset->npc = NULL;
        reset->npcID = row["target_id"];
        /*switch(reset->type)
        {
        case 1:
            reset->npc = GetCharacterIndex(row["target_id"]);
            break;
        }*/
        r->resets[reset->id] = reset;
    }
}

void Game::LoadSkills(Server * server)
{
    StoreQueryResult skillres = server->sqlQueue->Read("select * from skills");
    if(skillres.empty())
        return;

    Row row;
    StoreQueryResult::iterator i;
    for(i = skillres.begin(); i != skillres.end(); i++)
    {
        row = *i;
        Skill * s = new Skill(row["id"], (string)row["long_name"]);
        s->name = (string)row["name"];
        s->function_name = (string)row["function_name"];
        s->targetType = (Skill::TargetType)(int)row["target_type"];
        s->castScript = (string)row["cast_script"];
        s->applyScript = (string)row["apply_script"];
        s->tickScript = (string)row["tick_script"];
        s->removeScript = (string)row["remove_script"];
        s->description = (string)row["description"];
		s->costDescription = (string)row["cost_description"];
        s->castTime = row["cast_time"];
		
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

        s->cooldown = row["cooldown"];
        s->costFunction = row["cost_script"];
        
        skills.insert(std::pair<int, Skill *>(s->id, s));
		try {
			Server::lua.script(s->castScript.c_str());
			Server::lua.script(s->applyScript.c_str());
			Server::lua.script(s->tickScript.c_str());
			Server::lua.script(s->removeScript.c_str());
			Server::lua.script(s->costFunction.c_str());
		}
		catch (const std::exception & e)
		{
			LogFile::Log("error", e.what());
		}
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
        Quest * q = new Quest();
        q->id = row["id"];
        q->name = (string)row["name"];
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
        q->start = GetCharacterIndex(row["start"]);
        if(q->start != NULL)
            q->start->questStart.push_back(q);
        q->end = GetCharacterIndex(row["end"]);
        if(q->end != NULL)
            q->end->questEnd.push_back(q);

		StoreQueryResult objectiveres = server->sqlQueue->Read("select * from quest_objectives where quest=" + Utilities::itos(q->id));
		StoreQueryResult::iterator j;
		for (j = objectiveres.begin(); j != objectiveres.end(); j++)
		{
			row = *j;
			q->AddObjective(row["type"], row["count"], row["id"], (string)row["description"]);
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
        Item * i = new Item();
        i->id = row["id"];
        i->name = (string)row["name"];
		i->keywords = (string)row["keywords"];
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

        itemIndex[i->id] = i;
    }
}

void Game::LoadClasses(Server * server)
{
    StoreQueryResult classres = server->sqlQueue->Read("select * from classes");

	if (classres.empty())
	{
		LogFile::Log("error", "Warning! No classes loaded from database. Creating default class \"warrior\".");
		Class * c = new Class();
		c->id = 1;
		c->name = "warrior";
		c->color = "|M";
		classes[c->id] = c;
		return;
	}

    Row row;
    StoreQueryResult::iterator iter;
    for(iter = classres.begin(); iter != classres.end(); ++iter)
    {
        row = *iter;
        Class * c = new Class();
        c->id = row["id"];
        c->name = (string)row["name"];
        c->color = (string)row["color"];
        c->items = row["items"];

		StoreQueryResult skillsres = server->sqlQueue->Read("SELECT * from class_skills where class=" + Utilities::itos(c->id));
		StoreQueryResult::iterator j;
		for (j = skillsres.begin(); j != skillsres.end(); j++)
		{
			row = *j;
			Class::SkillData skd;
			skd.skill = Game::GetGame()->GetSkill(row["skill"]);
			skd.level = row["level"];
			c->classSkills.push_back(skd);
		}
        classes[c->id] = c;
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
        Area * a = new Area(row["id"], (string)row["name"], /*row["pvp"],*/ row["level_range_low"], row["level_range_high"]);
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

void Game::SaveCharacterIndex()
{
    std::map<int, Character *>::iterator iter;
    for(iter = characterIndex.begin(); iter != characterIndex.end(); ++iter)
    {
        iter->second->Save();
    }
}

void Game::SaveItemIndex()
{
    std::map<int, Item *>::iterator iter;
    for(iter = itemIndex.begin(); iter != itemIndex.end(); ++iter)
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
			h = NULL;
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
        Character * loaded = new Character();

        loaded->id = row["id"];
        loaded->name = row["name"];
		loaded->keywords = row["keywords"];
        loaded->title = row["title"];
        loaded->gender = row["gender"];
		loaded->race = row["race"];
        loaded->level = row["level"];
        loaded->agility = row["agility"];
        loaded->intellect = row["intellect"];
        loaded->strength = row["strength"];
        loaded->stamina = row["stamina"];
        loaded->wisdom = row["wisdom"];
        loaded->maxHealth = loaded->health = row["health"];
        loaded->maxMana = loaded->mana = row["mana"];
        loaded->maxEnergy = loaded->energy = row["energy"];
		loaded->maxRage = row["rage"];
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

		StoreQueryResult::iterator j;
		StoreQueryResult npcskillres = server->sqlQueue->Read("select * from npc_skills where npc=" + Utilities::itos(loaded->id));
		for (j = npcskillres.begin(); j != npcskillres.end(); j++)
		{
			row = *j;
			int skillid = row["skill"];
			loaded->AddSkill(Game::GetGame()->GetSkill(skillid));
		}

		StoreQueryResult npcdropsres = server->sqlQueue->Read("select * from npc_drops where npc=" + Utilities::itos(loaded->id));
		for (j = npcdropsres.begin(); j != npcdropsres.end(); j++)
		{
			row = *j;
			string drops = (string)row["items"];
			int percent = row["percent"];

			Character::DropData dd;
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
        characterIndex[loaded->id] = loaded;
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

Character * Game::NewCharacter(std::string name, User * user)
{
    Character * ch = new Character(name, user);
    characters.push_front(ch);
    return ch;
}

Character * Game::NewCharacter()
{
    Character * ch = new Character();
    characters.push_front(ch);
    return ch;
}

Character * Game::NewCharacter(Character * copy)
{
    Character * ch = new Character(*copy);
    characters.push_front(ch);
    return ch;
}

void Game::RemoveCharacter(Character * ch)
{
    ch->NotifyListeners();
    ch->remove = true;
    //characters.remove(ch);
    //delete ch;
}

User * Game::DuplicatePlayerCheck(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->name, name) 
            && ((*iter)->connectedState == User::CONN_PLAYING || (*iter)->connectedState > User::CONN_CONFIRM_NEW_PASSWORD))
            return (*iter);
    }
    return NULL;
}

//Not case sensitive
User * Game::GetUserByPCName(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->name, name))
            return (*iter);
    }
    return NULL;
}

Character * Game::GetCharacterByPCName(string name)
{
    std::list<User *>::iterator iter;
    for(iter = users.begin(); iter != users.end(); ++iter)
    {
        if((*iter)->character && !Utilities::str_cmp((*iter)->character->name, name))
            return (*iter)->character;
    }
    return NULL;
}

Room * Game::GetRoom(int id)
{
    std::map<int,Room *>::iterator it = rooms.find(id);
    if(it != rooms.end())
        return it->second;
    return NULL;
}

Skill * Game::GetSkill(int id)
{
    std::map<int,Skill *>::iterator it = skills.find(id);
    if(it != skills.end())
        return (*it).second;
    return NULL;
}

Quest * Game::GetQuest(int id)
{
    std::map<int,Quest *>::iterator it = quests.find(id);
    if(it != quests.end())
        return (*it).second;
    return NULL;
}

Area * Game::GetArea(int id)
{
    std::map<int,Area *>::iterator it = areas.find(id);
    if(it != areas.end())
        return (*it).second;
    return NULL;
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
    return NULL;
}

Help * Game::GetHelp(int id)
{
	std::map<int,Help *>::iterator it = helpIndex.find(id);
    if(it != helpIndex.end())
        return (*it).second;
    return NULL;
}

Character * Game::GetCharacterIndex(int id)
{
    std::map<int,Character*>::iterator it = characterIndex.find(id);
    if(it != characterIndex.end())
        return (*it).second;
    return NULL;
}

Item * Game::GetItemIndex(int id)
{
    std::map<int,Item*>::iterator it = itemIndex.find(id);
    if(it != itemIndex.end())
        return (*it).second;
    return NULL;
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

Character * Game::CreateNPCAnyID(std::string arg)
{
    int ctr = 1;
    std::map<int,Character *>::iterator iter;
    for(iter = characterIndex.begin(); iter != characterIndex.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Character * pChar = new Character(arg, ctr);
    pChar->changed = true;
    characterIndex.insert(std::pair<int, Character *>(pChar->id, pChar));
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
    for(iter = itemIndex.begin(); iter != itemIndex.end(); ++iter)
    {
        if(ctr != iter->second->id)
        {
            //found the first integer not in the map
            break;
        }
        ctr++;
    }

    Item * pItem = new Item(arg, ctr);
    pItem->changed = true;
    itemIndex.insert(std::pair<int, Item *>(pItem->id, pItem));
    return pItem;
}

Room * Game::CreateRoom(int value)
{
    if(GetRoom(value) != NULL)
    {
        LogFile::Log("error", "CreateRoom, room already exists");
        return NULL;
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
	//TODO
	static std::vector<int> experience_table = { 400, 900, 1500, 2100 };
	/*
	character is level 32-59):

XP to Level = (65x2 - 165x - 6750) × .82

XP to Level for levels 11-27 can be found by:

XP to Level = -.4x3 + 40.4x2 + 396x

For all levels lower than 11 the XP to Level can be expressed as the second grade function:

XP to Level = 40x2 + 360x 

*/

    if(level < 1)
        level = 1;
    else if(level > Game::MAX_LEVEL)
        level = Game::MAX_LEVEL;

    //cout << "ExperienceForLevel(" << level << ") returning " << (int)pow((double)(level),2) * 300 << endl;
    int temp = (int)(pow((double)level-1, 1.9) * 500);
    return temp - (temp % 100);
}

int Game::CalculateExperience(Character * ch, Character * victim)
{
	if (LevelDifficulty(ch->level, victim->level) == 0) //no xp for "gray" con npcs
		return 0;
	//todo: use threat table to determine if this character was helped outside of tapped group
	// and group modifiers if helpers wouldnt get exp from the npc...
	double xp = ch->level * 5 + 45;	//base xp
	if (victim->level > ch->level)
	{
		double difference = victim->level - ch->level;
		if (difference > 10)
			difference = 10.0;
		xp = xp * (1.0 + 0.05*difference);	//higher level victim, slightly higher exp
	}
	else if (victim->level < ch->level)
	{
		double difference = ch->level - victim->level;
		xp = xp * (1.0 - (difference / 7.0));		//lower level victim, less exp
	}
	return ceil(xp);
}

int Game::LevelDifficulty(int level1, int level2)
{
    if(level2 >= level1 + 10)       //Red / ??
        return 5;
    else if(level2 >= level1 + 5)   //Red
        return 4;
    else if(level2 >= level1 + 3)   //Magenta
        return 3;
    else if(level2 >= level1 - 2)   //Yellow
        return 2;
    else if(level2 >= level1 - 4)   //Green
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

Character * Game::LoadNPCRoom(int id, Room * toroom)
{
    Character * charIndex = Game::GetGame()->GetCharacterIndex(id);
    if(charIndex == NULL)
    {
        //ch->Send("NPC " + arg2 + " does not exist.\n\r");
        return NULL;
    }
    Character * newChar = Game::GetGame()->NewCharacter(charIndex);
    newChar->ChangeRooms(toroom);
    return newChar;
}

Character * Game::GetPlayerWorld(Character * ch, string name)
{
    if(!Utilities::str_cmp(name, "self") || !Utilities::str_cmp(name, "me"))
		return ch;
    int numberarg = Utilities::number_argument(name);
    int ctr = 1;
    std::list<User *>::iterator iter;
	for(iter = users.begin(); iter != users.end(); ++iter)
	{
        if((*iter)->connectedState == User::CONN_PLAYING && (*iter)->character != NULL && !Utilities::str_str(name, (*iter)->character->name))
		{
            if(ctr++ == numberarg)
			    return (*iter)->character;
		}
	}
	return NULL;
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
    return NULL;
}

Class * Game::GetClass(int id)
{
    std::map<int,Class *>::iterator it = classes.find(id);
    if(it != classes.end())
        return (*it).second;
    return NULL;
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
                result += "[" + Utilities::itos(iterator->second->id) + "] " + iterator->second->name + "\n\r";
                ++results_found;
            }
        }
        else if(data_type == 2)
        {
            if(!Utilities::str_str(argument, *(iterator->second->stringTable[field_name])))
            {
                result += "[" + Utilities::itos(iterator->second->id) + "] " + iterator->second->name + "\n\r";
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
    DIDNT WORK!!!
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
			result += "No rooms in the room list.\n\r";
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
			result += "Invalid field_name.\n\r";
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
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\n\r";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "items"))
    {
		if (itemIndex.begin() == itemIndex.end())
		{
			result += "No items in the item list.\n\r";
			return 0;
		}
		if (data_type == 1 && itemIndex.begin()->second->intTable.find(field_name) != itemIndex.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && itemIndex.begin()->second->doubleTable.find(field_name) != itemIndex.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if(data_type == 2 && itemIndex.begin()->second->stringTable.find(field_name) != itemIndex.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\n\r";
			return 0;
		}

        std::map<int, Item *>::iterator iter;
        for(iter = itemIndex.begin(); iter != itemIndex.end(); iter++)
        {
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->doubleTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\n\r";
					++results_found;
				}
				break;
			}
        }
        return results_found;
    }
    else if(!Utilities::str_cmp(table_name, "characters"))
    {
		if (characterIndex.begin() == characterIndex.end())
		{
			result += "No characters in the character list.\n\r";
			return 0;
		}
		if (data_type == 1 && characterIndex.begin()->second->intTable.find(field_name) != characterIndex.begin()->second->intTable.end())
		{
			whichtable = 1;
		}
		else if (data_type == 1 && characterIndex.begin()->second->doubleTable.find(field_name) != characterIndex.begin()->second->doubleTable.end())
		{
			whichtable = 2;
		}
		else if (data_type == 2 && characterIndex.begin()->second->stringTable.find(field_name) != characterIndex.begin()->second->stringTable.end())
		{
			whichtable = 3;
		}
		else
		{
			result += "Invalid field_name.\n\r";
			return 0;
		}

		std::map<int, Character *>::iterator iter;
		for (iter = characterIndex.begin(); iter != characterIndex.end(); iter++)
		{
			switch (whichtable)
			{
			case 1:
				if (SearchComparisonInt(*(iter->second->intTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->doubleTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\n\r";
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
			result += "No quests in the quest list.\n\r";
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
			result += "Invalid field_name.\n\r";
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
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + Utilities::itos(*(iter->second->doubleTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->name + ":  " + *(iter->second->stringTable[field_name]) + "\n\r";
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
			result += "No skills in the skill list.\n\r";
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
			result += "Invalid field_name.\n\r";
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
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + Utilities::itos(*(iter->second->intTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 2:
				if (SearchComparisonDouble(*(iter->second->doubleTable[field_name]), value, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + Utilities::itos(*(iter->second->doubleTable[field_name])) + "\n\r";
					++results_found;
				}
				break;
			case 3:
				if (SearchComparisonString(*(iter->second->stringTable[field_name]), argument, conditional_type))
				{
					result += "[" + Utilities::itos(iter->second->id) + "] " + iter->second->long_name + ":  " + *(iter->second->stringTable[field_name]) + "\n\r";
					++results_found;
				}
				break;
			}
		}
		return results_found;
    }
    else
    {
        result += "Invalid table name.\n\r";
    }
    return 0;
}