#ifndef CGAME_H
#define CGAME_H

#include <string>
#include <memory>
#include <map>
#include <list>
#include <vector>
#include <random>
#include <winsock2.h>
#include <WS2tcpip.h>
#include <windows.h>
#include "CHighResTimer.h"

class Server;
class User;
class Client;
class NPC;
class NPCIndex;
class Player;
class Character;
class Room;
class Skill;
class Class;
class Area;
class Quest;
class Item;
class Help;

class Game
{
public:

	Game();
    ~Game();

    static Game * GetGame();
    static void DeleteGame();
    void GameLoop(Server * server);
    void WorldUpdate(Server * server);
    void LoginHandler(Server * server, User * user, std::string argument);
	void NewUser(std::shared_ptr<Client> client);
	void RemoveUser(User * user);
	void RemoveUser(Client * client);
    NPC * NewNPC(NPCIndex * index);
    Player * NewPlayer(std::string name, User * user);
    void RemoveCharacter(Character * ch);
    void LoadGameStats(Server * server);
    void LoadRooms(Server * server);
    void LoadExits(Server * server);
    void LoadResets(Server * server);
    void LoadSkills(Server * server);
    void LoadNPCS(Server * server);
    void LoadQuests(Server * server);
    void LoadItems(Server * server);
    void LoadClasses(Server * server);
    void LoadTriggers(Server * server);
    void LoadAreas(Server * server);
	void LoadHelp(Server * server);

    Room * GetRoom(int id);
    Room * CreateRoomAnyID();
    Room * CreateRoom(int value);
    Skill * CreateSkillAnyID(std::string arg);
    Skill * GetSkill(int id);
    Quest * CreateQuestAnyID(std::string arg);
    Quest * GetQuest(int id);
	NPCIndex * CreateNPCAnyID(std::string arg);
    NPCIndex * GetNPCIndex(int id);
    Item * CreateItemAnyID(std::string arg);
    Item * GetItem(int id);
    Class * GetClass(int id);
    Class * GetClassByName(std::string name);
	Help * GetHelp(int id);
	Help * GetHelpByName(std::string name);
	Help * CreateHelpAnyID(std::string name);
    Area * CreateAreaAnyID(std::string name);
    Area * GetArea(int id);

	void SaveGameStats();
	void SaveRooms();
	void SaveSkills();
	void SaveNPCIndex();
	void SaveQuests();
	void SaveItems();
	void SaveClasses();
	void SaveAreas();
	void SaveHelp();

	User * DuplicatePlayerCheck(std::string name);
	User * GetUserByPCName(std::string name);
	Player * GetPlayerByName(std::string name);
	Player * GetPlayerWorld(Player * ch, std::string name);
    
	void GlobalMessage(std::string);

    static int ExperienceForLevel(int level);
    static int CalculateExperience(Character * ch, Character * victim);
    static int LevelDifficulty(int ch, int vict);
	static std::string LevelDifficultyColor(int leveldifficulty);
	static std::string LevelDifficultyLightColor(int leveldifficulty);
    static Character * LoadNPCRoom(int id, Room * toroom);
	
    struct SearchInfo
    {
        std::string name;
        void (*search_func)(Game * g, Character * ch, std::string argument);
    };
    
    bool SearchComparisonInt(int field_value, int search_value, int conditional_type);
    bool SearchComparisonString(std::string field_value, std::string search_value, int conditional_type);
	bool SearchComparisonDouble(double field_value, double search_value, int conditional_type);
	int Search(std::string table_name, std::string field_name, int conditional_type, std::string argument, int data_type, std::string & result);

    bool shutdown;
    std::list<User *> users; //user has a player, player has a user
    std::list<Character *> characters; //All NPCs and Players currently loaded in the world //TODO: boost.multiIndex ???
    std::map<int, NPCIndex *> npcIndex; //NPC prototypes
    std::map<int, Room *> rooms;
    std::map<int, Skill *> skills;
    std::map<int, Quest *> quests;
    std::map<int, Item *> items;
    std::map<int, Class *> classes;
    std::map<int, Area *> areas;
	std::map<int, Help *> helpIndex;

    static double currentTime;
    double worldupdateCount;
    int total_past_connections;
    int max_players_since_boot;
    int total_players_since_boot;
	__int64 totalBytesCompressed; //how many bytes actually sent
	__int64 totalBytesUncompressed; //how many bytes would have been sent
    int newplayerRoom;
    const static int MAX_LEVEL = 240;
	const static int IDLE_TIMEOUT = 900;

private:
	
    CRITICAL_SECTION userListCS; 
    HighResTimer timer;
};

#endif //CGAME_H