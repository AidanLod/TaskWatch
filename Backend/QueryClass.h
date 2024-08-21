//This class is responsible for handling all queries for the database
#ifndef QUERYCLASS_H
#define QUERYCLASS_H
#include <sqlite3.h>
#include <iostream>
#include <cstdio>
#include <mutex>
#include <thread>
#include <atomic>
#include <unistd.h>
#include <queue>
#include <string>
#include <chrono>
#include <tuple>
#include <condition_variable>
#include <utility>

namespace Q
{
    enum Table {ACTIVITY, PTIME, PROGRAM, TYPE, END};
    struct data{
        Table table;
        int id = 0;
        std::string name;
        std::string typeName;
        std::string programName;
        std::string activityName;
        int time;
        std::string date;
        bool request = false;
    };
    struct pTime{
        int pid;
        int aid;
        int utsStart;
        std::string date;
    };
    class QueryClass
    {
    public:
        explicit QueryClass(unsigned char& ec); //construct
        ~QueryClass(); //destruct
        void handleTraffic(data d); //may be useless but provides the opportunity for changing how stuff is handled in the future
        void endLoop(); //ends the loop and cleans it up
        bool activityChanged(); //checks to see if the activity has changed

    private:
        //functions/methods
        void requestLoop(); //always running only stops at due to mutex locks or the end of the program
        void handleError(int ecode, int bNum, const std::string&); //handles all sqlite return error codes
        void handleActivity(const std::string&); //handles the process for inserting and changing activities
        bool changeActive(int act, int& id); //changes the active activity
        bool insertActivity(const std::string&); //inserts activity if activity is new
        bool getActivityID(const std::string&); //gets the activity id of the currently active activity and sets currentActivity to that id
        void insertProgram(const data&); //handles the process for inserting a program
        int getProgramID(const std::string&); //gets the program id corresponding to the given program name
        void insertType(const std::string&); //inserts the type if the type is new
        int getTypeID(const std::string&); //gets the type id
        void handlePTime(data&); //gets all the data together to insert and edit a pTime row
        void insertPtime(pTime&); //inserts PTime
        void editPtime(int& id, int& uts); //takes a unix time stamp as an int and changes the time used for the last active pTime
        bool getCurrentPTime(pTime&); //sets lastPTimeID to the current pTimes id so that timeused can be changed later
        int getTimeUsed(int); //gets the timeUsed from the ptime with the corresponding id
        void addQueue(data&); //thread safe adding to queue
        data popQueue(); //thread safe popping from queue
        std::queue<pTime> returnData(); //returns all pTime data
        std::queue<pTime> returnDataRestricted(); //
        //objects
        int currentActivity = 0;
        std::queue<data> requestQueue;
        unsigned char& exitCode;
        FILE* file;
        int rc; //stores return code
        sqlite3* db;
        int lastPTimeID = 0;
        int utss = 0; //utsStart of the ptime whose timeUsed is to be edited, made out of convenience and brain numbness
        bool aChange = false;
        //threading
        std::mutex dbMutex; //to be used in handleTraffic
        std::thread qLoop;
        std::mutex qMutex; //used for the requestQueue
        std::condition_variable cvq; //used for the qMutex
        std::condition_variable cvdb; //used for the  dbMutex
        std::atomic<bool> running;

    };
}

#endif //QUERYCLASS_H
