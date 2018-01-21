#ifndef CQUEST_H
#define CQUEST_H

class Character;

class Quest
{
public:
    Quest();
    Quest(std::string name, int id);
    ~Quest();

    //Quest "index" data
    int id;
    std::string name;
    std::string shortDescription;
    std::string longDescription;
    std::string progressMessage;
    std::string completionMessage;
    int level;
    int questRequirement; //id of a quest that must be completed to offer this quest
    int questRestriction; //TODO if questRestriction is already complete OR active, don't offer this quest
    int levelRequirement;
    Character * start;
    Character * end;
    //std::vector<Item *> itemRewards;
    int experienceReward;
    int moneyReward;
    //int reputationReward; ??
    bool shareable;
    //TODO adds flags to quests? flag: mage rogue warrior cleric? male female, class combos?

    bool changed;

    enum ObjectiveType { OBJECTIVE_ROOM, OBJECTIVE_VISITNPC, OBJECTIVE_KILLNPC, OBJECTIVE_ITEM };
    struct QuestObjective
    {
        ObjectiveType type;
        int count;
        void * objective; //pointer to room, item, npc todo: why not just store the id?
        std::string description; //TODO
    };
    std::vector<QuestObjective> objectives;

    //For search  Store a reference to all searchable class data by type
    std::map<std::string, std::string*> stringTable;
    std::map<std::string, int*> intTable;
    std::map<std::string, double*> doubleTable;

    void AddObjective(int type, int count, int id, std::string desc);
    void RemoveObjective(int index);
    void Save();
    static int GetDifficulty(int playerlevel, int questlevel);
    static std::string GetDifficultyColor(int playerlevel, int questlevel);

private:

};

#endif