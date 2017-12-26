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
    std::list<Listener*> listeners_;
};

#endif