#ifndef CLISTENERMANAGER_H
#define CLISTENERMANAGER_H

class SubscriberManager
{
public:
    virtual ~SubscriberManager();
    void AddSubscriber(Subscriber *);
    void RemoveSubscriber(Subscriber *);
    bool HasSubscriber(Subscriber *);
	std::string DebugPrintSubscribers();
    void NotifySubscribers();

protected:
	struct SubscriberCount
	{
		SubscriberCount(Subscriber * l) : subscriber(l), refcount(1) {};
		bool operator==(const Subscriber * l) { return l == subscriber; };
		Subscriber * subscriber;
		int refcount;		//A subscriber might subscribe to a SubscriberManager for various reasons:
	};						//   threat list, targeting, reset, spells... RemoveSubscriber needs to know ALL reasons are done
    std::list<SubscriberCount> subscribers_;
};

#endif