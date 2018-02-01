#ifndef CLISTENERMANAGER_H
#define CLISTENERMANAGER_H

class ListenerManager
{
public:
    virtual ~ListenerManager();
    void AddListener(Listener *);
    void RemoveListener(Listener *);
    bool HasListener(Listener *);
    //void DeferredRemoveListener(Listener *);
    void NotifyListeners();

private:
	struct ListenerCount
	{
		ListenerCount(Listener * l) : listener(l), refcount(1) {};
		bool operator==(const Listener * l) { return l == listener; };
		Listener * listener;
		int refcount;		//A listener might subscribe to a ListenerManager for various reasons:
	};						//   threat list, targeting, reset, spells... RemoveListener needs to know ALL reasons for listening are done
    std::list<ListenerCount> listeners_;
};

#endif