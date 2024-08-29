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
        std::string className;
        int time;
        std::string date;
        bool request = false;
    };
    struct inPTime{
        int pid;
        int aid;
        int utsStart;
        std::string date;
    };
    struct outPTime{
        std::string pName; //program name
        std::string aName; //activity name
        std::string tName; //type name
        std::string cName; //class name
        int timeUsed;
        std::string date;
    };
    class QueryClass
    {
    public:
            //getting started
        explicit QueryClass(unsigned char& ec); //construct
            //closing out
        ~QueryClass(); //destruct
        void endLoop(); //ends the loop and cleans it up
            //making requests
        void handleTraffic(data d); //may be useless but provides the opportunity for changing how stuff is handled in the future
        std::queue<outPTime> getData(); //gets the stored data and empties the queue
        bool activityChanged(); //checks to see if the activity has changed

    private:
        //functions/methods
            //getting started
        void buildTables(); //builds the database tables if they don't exist
        void requestLoop(); //always running only stops at due to mutex locks or the end of the program
            //error handling
        void handleError(int ecode, std::string, const std::string&); //handles all sqlite return error codes
            //callBacks for sqlite3_exec()
        static int idCallback(void* data, int argc, char** argv, char** azColName); //used for the ID callback for all the get_ID() functions
        int timeCallback(void* data, int argc, char** argv, char** azColName); //used for getTimeUsed()
            //handling different tables
        void handleActivity(const std::string&); //handles the process for inserting and changing activities
        void handlePTime(data&); //gets all the data together to insert and edit a pTime row
            //editing rows in a table
        bool changeActive(int act, int& id); //changes the active activity
        void editPtime(int& id, int& uts); //takes a unix time stamp as an int and changes the time used for the last active pTime
            //inserting rows
        bool insertActivity(const std::string&); //inserts activity if activity is new
        void insertProgram(const data&); //handles the process for inserting a program
        void insertType(const std::string&); //inserts the type if the type is new
        void insertClass(const std::string&, int); //inserts the class if the class is new
        void insertPtime(inPTime&); //inserts PTime
            //get information from database
        int getID(std::string query, char*&); //takes in the different id queries and runs them returning the corresponding id
        bool getActivityID(const std::string&); //gets the activity id of the currently active activity and sets currentActivity to that id
        int getProgramID(const std::string&); //gets the program id corresponding to the given program name
        int getTypeID(const std::string&); //gets the type id of the type with the corresponding name
        int getClassID(const std::string&); //gets the class id of the class with the corresponding name
        bool getCurrentPTimeID(inPTime&); //sets lastPTimeID to the current pTimes id so that timeused can be changed later
        int getTimeUsed(int); //gets the timeUsed from the ptime with the corresponding id
        void storeDataDate(std::string date); //stores the data in rData
            //thread safe queuing and dequeing
        void addQueue(data&); //thread safe adding to queue
        data popQueue(); //thread safe popping from queue
        //objects
        int currentActivity = 0;
        std::queue<data> requestQueue;
        unsigned char& exitCode;
        FILE* file;
        sqlite3* db;
        int lastPTimeID = 0;
        int utss = 0; //utsStart of the ptime whose timeUsed is to be edited, made out of convenience and brain numbness
        bool aChange = false;
        std::queue<outPTime> rData;
        //threading
        std::mutex rMutex; //used for the storeData function(s) and the getData function
        std::thread qLoop;
        std::mutex qMutex; //used for the requestQueue
        std::condition_variable cvq; //used for the qMutex
        std::condition_variable cvr; //used for the  rMutex
        std::atomic<bool> running;

    };
}

#endif //QUERYCLASS_H
