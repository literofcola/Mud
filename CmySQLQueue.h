#ifndef MYSQLQUEUE_H
#define MYSQLQUEUE_H

#include <mysql++.h>

using namespace mysqlpp;

class mySQLQueue
{
public:
    mySQLQueue();
    ~mySQLQueue();

    bool Connect(std::string db, std::string server, std::string user, std::string password="", int port=0);
    std::string PeekWrite();
    std::string PeekRead();
    void Write(std::string sql);    //queued write
    void Read(std::string sql, void (*callback)(StoreQueryResult)); //queued read
    StoreQueryResult Read(std::string sql);   //non-queued read
    void Write(std::string sql, std::string & status);  //non-queued write
    void Close();
    void Disconnect();

private:
    Connection conn;
    static unsigned __stdcall Start(void * arg);
    void Run();

    struct queueData
    {
        queueData * next;
        std::string query;
        StoreQueryResult result;
        void (*callback)(StoreQueryResult); //Result must be by value
    };

    Query * query;
    queueData * writeFirst;
    queueData * readFirst;
    queueData * writeLast;
    queueData * readLast;
    HANDLE myThreadHandle;
    bool running;
};

#endif //MYSQLQUEUE_H
