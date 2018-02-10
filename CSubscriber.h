#ifndef CLISTENER_H
#define CLISTENER_H

class Subscriber
{
public:
    virtual ~Subscriber() {}
    virtual void Notify(class SubscriberManager *) = 0;
};

#endif