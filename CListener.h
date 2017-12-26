#ifndef CLISTENER_H
#define CLISTENER_H

class Listener
{
public:
    virtual ~Listener() {}
    virtual void Notify(class ListenerManager *) = 0;
};

#endif