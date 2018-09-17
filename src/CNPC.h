#ifndef CNPC_H
#define CNPC_H

#include "CNPCIndex.h"
#include "CCharacter.h"
#include "CServer.h"

class Player;
class Exit;

class NPC : public Character
{
public:
	NPC() = delete;
	NPC(NPCIndex * index_);
	NPC(const NPC & copy) = delete;
	~NPC();

	struct Looter //A single eligible looter for a piece of loot dropped in a corpse
	{
		enum RollType
		{
			ROLL_UNDECIDED, ROLL_NEED, ROLL_GREED, ROLL_PASS
		};
		Player * ch;
		RollType roll_type;
		int final_roll;

		Looter(Player * ch_) : ch(ch_), roll_type(ROLL_UNDECIDED), final_roll(0) {};
		bool operator == (const Player * c) const
		{
			return ch == c;
		}
	};
	struct OneLoot //One item dropped in a corpse
	{
		int id;
		Item * item;
		std::list<Looter> looters;
		double roll_timer;
	};
	struct LeashData //Keep track of wander distance and leash distance
	{
		int type;
		Room * room;
		Exit * next;
	};

	std::list<OneLoot> loot; //Loot dropped in a corpse
	Reset * reset; //reset that spawned this npc, if any

	std::list<LeashData> leashData;
	std::list<std::pair<Room *, int>> leashPath;
	Room * leashOrigin;
	//std::map<int, Exit*> wanderPath;

	inline bool IsNPC() override { return true; };
	inline bool IsPlayer() override { return false; };
	sol::object AsNPC() override { return sol::make_object(Server::lua, this); };

	void Notify(SubscriberManager * lm);

	inline int GetMaxHealth() override { return npcindex->maxHealth; };
	inline int GetMaxMana() override { return npcindex->maxMana; };
	inline int GetMaxEnergy() override { return npcindex->maxEnergy; };
	inline int GetMaxRage() override { return npcindex->maxRage; };

	inline NPCIndex * GetNPCIndex() override { return npcindex; };
	int GetIndexID();
	std::string GetName() override;
	int GetLevel() override;
	int GetGender() override { return npcindex->gender; };
	bool FlagIsSet(const int flag) override;
	bool IsAlive() override;
	inline virtual std::string GetTitle() override { return npcindex->title; };

	void DoLootRoll(OneLoot * oneloot);
	OneLoot * GetCorpseLoot(int corpse_id);
	void RemoveLoot(OneLoot *);
	void RemoveLooter(Character * ch);
	void RemoveAllLooters() override;
	void SetRollType(Player * who, int corpse_id, Looter::RollType type);

	double GetMainhandWeaponSpeed() override;
	double GetOffhandWeaponSpeed() override;
	double GetMainhandDamagePerSecond() override;
	int GetOffhandDamageRandomHit() override;
	double GetOffhandDamagePerSecond() override;
	int GetMainhandDamageRandomHit() override;
	double GetDodge() override { return 5;  }; //todo: npcindex needs these fields, no stats!
	double GetCrit() override { return 5; };
	double GetParry() override { return 5; };
	int GetArmor() override { return GetNPCIndex()->armor; };
	void Cast(std::string argument) override;

private:
	NPCIndex * npcindex;
};

#endif