#include "stdafx.h"
#include "CmySQLQueue.h"
#include "CLogFile.h"

using std::string;
using namespace mysqlpp;

mySQLQueue::mySQLQueue()
{
    writeFirst = readFirst = writeLast = readLast = 0;
    running = true;
    query = new Query(&conn);
    myThreadHandle = (HANDLE)_beginthreadex(nullptr, 0, mySQLQueue::Start, this, 0, nullptr);
}

mySQLQueue::~mySQLQueue()
{
    queueData * wf = writeFirst;
    queueData * rf = readFirst;
    queueData * next = wf;
    while(wf)
    {
        next = wf->next;
        wf->next = nullptr;
        delete wf;
        wf = next;
    }
    while(rf)
    {
        next = rf->next;
        rf->next = nullptr;
        delete rf;
        rf = next;
    }
    delete query;
}

bool mySQLQueue::Connect(string db, string server, string user, string password, int port)
{
    try
    {
        conn.connect(db.c_str(), server.c_str(), user.c_str(), password.c_str(), port);
        LogFile::Log("status", "Connected to SQL server " + server + ", db = " + db);
    }
    catch(const Exception & er)
    //catch(const ConnectionFailed & er) 
    { 
        LogFile::Log("error", "mySQLQueue: Could not connect to mySQL server " + db + " on " + server + ": " + er.what());
        return false;
    }
    return true;
}

void mySQLQueue::Disconnect()
{
    conn.disconnect();
}

void mySQLQueue::Close()
{
    running = false;
    ResumeThread(myThreadHandle);
    WaitForSingleObject(myThreadHandle, INFINITE);
}

//Grab the sql string for the next write
string mySQLQueue::PeekWrite()
{
    return writeFirst->query;
}

//Grab the sql string for the next read
string mySQLQueue::PeekRead()
{
    return readFirst->query;
}

void mySQLQueue::WriteQueued(string sql)
{
    //set up the queue with data to write
    queueData * qd = new queueData();
    qd->next = nullptr;
    qd->query = sql;
    qd->callback = nullptr;

    if(!writeLast)
    {
        writeFirst = writeLast = qd;
    }
    else
    {
        writeLast->next = qd;
        writeLast = qd;
    }

    ResumeThread(myThreadHandle); //resume the thread (if it's not already)
}

void mySQLQueue::Write(string sql)
{
	std::string status;
    Query q = conn.query();
    try
    {
        q.exec(sql);
        status = "Done.";
    }
    catch (const BadQuery& er)
    {   // handle any connection
        // or query errors that may come up
        LogFile::Log("error", string("mySQLQueue::Error: ") + er.what());
        status = string("mySQLQueue::Error: ") + er.what();
    } 
    catch (const BadConversion & er) 
    {
        // we still need to catch bad conversions incase something goes
        // wrong when the data is converted into stock
        LogFile::Log("error", "mySQLQueue::Error: Tried to convert \"" + er.data + "\" to a \""
            + er.type_name + "\".");
        status = "mySQLQueue::Error: Tried to convert \"" + er.data + "\" to a \"" + er.type_name + "\".";
    } 
	catch (const Exception & er)
	{
		LogFile::Log("error", string("mySQLQueue::Error: ") + er.what());
        status = string("mySQLQueue::Error: ") + er.what();
	}
}

void mySQLQueue::ReadQueued(string sql, void (*callback)(StoreQueryResult))
{
    //set up the queue with data to read
    queueData * qd = new queueData();
    qd->next = nullptr;
    qd->query = sql;
    qd->callback = callback;

    if(!readLast)
    {
        readFirst = readLast = qd;
    }
    else
    {
        readLast->next = qd;
        readLast = qd;
    }

    ResumeThread(myThreadHandle); //resume the thread (if it's not already)
}

StoreQueryResult mySQLQueue::Read(string sql)
{
    Query q = conn.query();

    q << sql;
    //logfile.Log("mySQLQueue: Executing query: " + sql);
	return q.store();
}

/* static unsigned __stdcall Thread(void * arg); <-- Class definition */
unsigned __stdcall mySQLQueue::Start(void * arg)
{
	mySQLQueue *sql = (mySQLQueue *)arg;
	LogFile::Log("status", "SQL Thread Start");
	sql->Run();
	//logfile.Log("SQL Thread End");

	_endthreadex(0);
	return 0;
}

void mySQLQueue::Run()
{
    queueData * temp;
    while(running)
    {
        //cerr << "ran once" << endl;
        //while(writeFirst != nullptr || readFirst != nullptr)
        //{
            //handle reads...
            while(readFirst)
            {
                *query << readFirst->query;
                //logfile.Log("mySQLQueue: Executing query: " + readFirst->query);
                try
                {
                    readFirst->result = query->store();
                }
                catch (const BadQuery & er)
                { // handle any connection
                    // or query errors that may come up
                    std::cerr << "Error: " << er.what() << std::endl;
                } 
                catch (const BadConversion & er) 
                {
                    // we still need to cache bad conversions incase something goes
                    // wrong when the data is converted into stock
					std::cerr << "Error: Tried to convert \"" << er.data << "\" to a \""
                        << er.type_name << "\"." << std::endl;
                } 
				catch (const Exception & er)
				{
					std::cerr << "Error: " << er.what() << std::endl;
				}

                readFirst->callback(readFirst->result);

                if(readFirst == readLast)
                {
                    delete readFirst;
                    readFirst = readLast = nullptr;
                }
                else
                {
                    temp = readFirst;
                    readFirst = readFirst->next;
                    delete temp;
                }
            }
            //then handle writes...
            while(writeFirst)
            {
                //logfile.Log("mySQLQueue: Executing query: " + writeFirst->query);
                try
                {
                    query->exec(writeFirst->query);
                }
                catch (const BadQuery & er)
	            {   // handle any connection
		            // or query errors that may come up
                    LogFile::Log("error", string("mySQLQueue::Error: ") + er.what());
	            } 
	            catch (const BadConversion & er) 
	            {
		            // we still need to cache bad conversions incase something goes
		            // wrong when the data is converted into stock
                    LogFile::Log("error", "mySQLQueue::Error: Tried to convert \"" + er.data + "\" to a \""
			             + er.type_name + "\".");
	            } 
				catch (const std::exception & er)
				{
					LogFile::Log("error", string("mySQLQueue::Error: ") + er.what());
				}

                if(writeFirst == writeLast)
                {
                    delete writeFirst;
                    writeFirst = writeLast = nullptr;
                }
                else
                {
                    temp = writeFirst;
                    writeFirst = writeFirst->next;
                    delete temp;
                }
            }
        //}
        if(running)
            SuspendThread(myThreadHandle);
    }
}