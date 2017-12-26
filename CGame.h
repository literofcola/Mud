#ifndef CGAME_H
#define CGAME_H

class Server;
typedef boost::shared_ptr<Server> Server_ptr;

using boost::multi_index_container;
using namespace boost::multi_index;

class Game
{
public:
    static Game * GetGame();
    static void DeleteGame();
    void GameLoop(Server_ptr server);
    void WorldUpdate(Server_ptr server);
    void LoginHandler(Server_ptr server, User * user, std::string argument);
	void NewUser(Client_ptr client);
	void RemoveUser(User * user);
    Character * NewCharacter();
    Character * NewCharacter(std::string name, User * user);
    Character * NewCharacter(Character * copy);
    void RemoveCharacter(Character * ch);
    void LoadGameStats(Server_ptr server);
    void LoadRooms(Server_ptr server);
    void LoadExits(Server_ptr server);
    void LoadResets(Server_ptr server);
    void LoadSkills(Server_ptr server);
    void LoadNPCS(Server_ptr server);
    void LoadQuests(Server_ptr server);
    void LoadItems(Server_ptr server);
    void LoadClasses(Server_ptr server);
    void LoadTriggers(Server_ptr server);
    void LoadAreas(Server_ptr server);
	void LoadHelp(Server_ptr server);

    User * DuplicatePlayerCheck(std::string name);
    User * GetUserByPCName(std::string name);
    Character * GetCharacterByPCName(std::string name);
    Character * GetPlayerWorld(Character * ch, std::string name);
    void GlobalMessage(std::string);

    Room * GetRoom(int id);
    Room * CreateRoomAnyID();
    Room * CreateRoom(int value);
    Skill * CreateSkillAnyID(std::string arg);
    Skill * GetSkill(int id);
    Quest * CreateQuestAnyID(std::string arg);
    Quest * GetQuest(int id);
    Character * CreateNPCAnyID(std::string arg);
    Character * GetCharacterIndex(int id);
    Item * CreateItemAnyID(std::string arg);
    Item * GetItemIndex(int id);
    Class * GetClass(int id);
    Class * GetClassByName(std::string name);
	Help * GetHelp(int id);
	Help * GetHelpByName(std::string name);
	Help * CreateHelpAnyID(std::string name);
    Area * CreateAreaAnyID(std::string name);
    Area * GetArea(int id);
    
    static int ExperienceForLevel(int level);
    static int CalculateExperience(Character * ch, Character * victim);
    static int LevelDifficulty(int level1, int level2);
    static Character * LoadNPCRoom(int id, Room * toroom);
    void SaveGameStats();
    void SaveRooms();
    void SaveSkills();
    void SaveCharacterIndex();
    void SaveQuests();
    void SaveItemIndex();
    void SaveClasses();
    void SaveAreas();
	void SaveHelp();

    struct SearchInfo
    {
        std::string name;
        void (*search_func)(Game * g, Character * ch, std::string argument);
    };
    
    //static void SearchRooms(Game * g, Character * ch, std::string argument);
    //static void SearchItems(Game * g, Character * ch, std::string argument);
    //static void SearchCharacters(Game * g, Character * ch, std::string argument);
    bool Game::SearchComparisonInt(int field_value, int search_value, int conditional_type);
    bool Game::SearchComparisonString(std::string field_value, std::string search_value, int conditional_type);
    int Search(std::string table_name, std::string field_name, int conditional_type, std::string argument, int data_type, std::string & result);
    /*template <typename T1>
    int DoSearch(T1 table_name, std::string field_name, std::string argument, int data_type, std::string & result);*/


    bool shutdown;

    std::list<User *> users;
    std::list<Character *> characters; //Characters loaded into the world //TODO: boost.multiIndex ???
    std::map<int, Character *> characterIndex; //Character prototypes
    std::map<int, Room *> rooms;
    std::map<int, Skill *> skills;
    std::map<int, Quest *> quests;
    std::map<int, Item *> itemIndex;
    std::map<int, Class *> classes;
    std::map<int, Area *> areas;
	std::map<int, Help *> helpIndex;

    /*typedef boost::multi_index_container<Room, indexed_by<
		ordered_unique<member<Room,int,&Room::id> >,    
        ordered_non_unique<member<Room,std::string,&Room::name> > > > room_set;
	room_set m_rooms;*/

    static double currentTime;
    double worldupdateCount;
    int total_past_connections;
    int max_players_since_boot;
    int total_players_since_boot;
	__int64 totalBytesCompressed; //how many bytes actually sent
	__int64 totalBytesUncompressed; //how many bytes would have been sent
    int newplayerRoom;
    const static int MAX_LEVEL = 240;

	static const char TELOPT_MXP = '\x5B';  // (91)
	static const char TELOPT_MCCP = '\x56'; //MCCP 2 (86)
	static const char TELOPT_GMCP = '\xC9'; //201

private:
    Game();
    ~Game();
	
    //std::map<Player *> players;
    HighResTimer timer;
};

#endif //CGAME_H