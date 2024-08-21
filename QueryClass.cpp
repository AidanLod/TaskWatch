//
// Created by main on 6/2/24.
//
#include "QueryClass.h"
namespace Q {
    QueryClass::QueryClass(unsigned char &ec) : exitCode(ec) {
        file = freopen("ErrorOutput.txt", "w", stderr);
        if (file == nullptr) {
            std::cerr << "Error redirecting stderr to file." << std::endl;
            exitCode = 1;
        }
        rc = sqlite3_open("timeData.db", &db);
        try { //block 1
            if (rc != SQLITE_OK) {
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 1, "QueryClass()");
        }
        running = true;
        qLoop = std::thread(&QueryClass::requestLoop, this);
        data d;
        d.typeName = "Misc";
        d.table = TYPE;
        handleTraffic(d);
    }

    QueryClass::~QueryClass() {
        sqlite3_close(db);
    }

    void QueryClass::handleError(int ecode, int bNum, const std::string &fName) {
        std::cerr << "Additional information for provided SQLite error codes may be found at\n"
                     "https://sqlite.org/rescode.html#ok\n";
        switch (ecode) {
            case 1:
                std::cerr << "ERR Code (Q1): SQLITE_ERROR\n"
                             "Something went wrong. Code returned in exception block " << bNum << " in " << fName
                          << std::endl;
                exitCode = 1;
                break;
            case 2:
                std::cerr << "ERR Code (Q2): SQLITE_INTERNAL\n"
                             "Bug in database engine. Code returned in exception block " << bNum << " in " << fName
                          << std::endl;
                exitCode = 1;
                break;
            case 3:
                std::cerr << "ERR Code (Q3): SQLITE_PERM\n"
                             "The access mode for a newly created database could not be provided. Code returned in exception block "
                          << bNum << " in " << fName << std::endl;
                exitCode = 1;
                break;
            case 4:
                std::cerr << "ERR Code (Q4): SQLITE_ABORT\n"
                             "An operation was aborted prior to completion. Code returned in exception block " << bNum
                          << " in " << fName << std::endl;
                exitCode = 1;
                break;
            case 5:
                std::cerr << "ERR Code (Q5): SQLITE_BUSY\n"
                             "The database file could not be written or read from because of concurrent activity. Code returned in exception block "
                          << bNum << " in " << fName << std::endl;
                break;
            case 6:
                std::cerr << "ERR Code (Q6): SQLITE_LOCKED\n"
                             "A write operation could not continue because of a conflict within the same database connection or a conflict "
                             "with a different database connection that uses a shared cache. Code returned in exception block "
                          << bNum << " in " << fName << std::endl;
                break;
            case 19:
                std::cerr << "ERR Code (Q19): SQLITE_CONSTRAINT\n"
                             "Some sort of constraint was hit, this will most likely be a UNIQUE constraint which we expect to hit, there should be no worries with this error."
                             "Code returned in exception block" << bNum << " in " << fName << std::endl;
                break;
            case 2067:
                std::cerr << "ERR Code (Q2067): SQLITE_CONSTRAINT_UNIQUE\n"
                             "An attempt to create a row using a duplicate unique value was made. This is nothing to worry about usually."
                             "Code returned in exception block" << bNum << " in " << fName << std::endl;
                break;
            default:
                std::cerr << "unfortunately haven't catalogued this error from " << bNum << "in" << fName << std::endl;
                exitCode = 1;
                break;

        }

    }

    void QueryClass::handleTraffic(data d) {
        addQueue(d);
    }

    void QueryClass::requestLoop() {
        while (running.load()) {
            data d = popQueue(); //stops here while waiting for the queue to not be empty
            Table t = d.table;
            if (t != END) { //end is only added to the queue when the program is shutting down
                if (d.request) {
                    //gets the data depending on what is requested
                }
                else if (t == ACTIVITY)
                    handleActivity(d.activityName);
                else if (t == PROGRAM)
                    insertProgram(d);
                else if (t == TYPE)
                    insertType(d.typeName);
                else if (t == PTIME)
                    handlePTime(d);
            }
            else {
                editPtime(lastPTimeID, d.time);
            }
        }
    }

    void QueryClass::addQueue(data& d) {
        std::lock_guard<std::mutex> lock(qMutex);
        requestQueue.push(std::move(d));
        cvq.notify_one();
    }

    data QueryClass::popQueue() {
        std::unique_lock<std::mutex> lock(qMutex);
        cvq.wait(lock, [this] {
            return !requestQueue.empty();
        });
        data d = requestQueue.front();
        requestQueue.pop();
        return d;
    }

    void QueryClass::endLoop() {
        if (running.load()) {
            running.store(false);
        }
        if (qLoop.joinable()) {
            data d;
            d.table = END;
            d.time = static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); //this line gets the current unix timestamp which is of type time_point, it is then converted to time_t which is typically a long, and then it is static cast as an int
            addQueue(d);
        }
        qLoop.join();
    }

    void QueryClass::handleActivity(const std::string &name) {
        //add in error handling if a new activity fails to be inserted and or started
        //should just change the last activity to the active one
        if (currentActivity != 0) //enters if activity is already started
            changeActive(0, currentActivity);
        //goes here once there is no currently active activity
        //enters if activity insertion failed in an unexpected way
        if (!insertActivity(name)) {
            std::cerr << "Failed to insert and/or start activity\n";
            exitCode = 1;
            return;
        }
        //goes here to get the newly activated activites id
        if (!getActivityID(name)) {
            std::cerr << "Failed to get activity ID\n";
            exitCode = 1;
            return;
        }
        if (!changeActive(1, currentActivity)){
            std::cerr << "Failed to change active\n";
            exitCode = 1;
        }
   }

    bool QueryClass::changeActive(int act, int &id) {
        const char *sql;
        sqlite3_stmt *stmt;
        sql = "UPDATE activity SET active = ? WHERE id = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "handleActivity()");
            exitCode = 1;
            return false;
        }
        rc = sqlite3_bind_int(stmt, 1, act);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "handleActivity()");
            exitCode = 1;
            return false;
        }
        rc = sqlite3_bind_int(stmt, 2, id);
        try { //block 3
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 3, "handleActivity()");
            exitCode = 1;
            return false;
        }

        rc = sqlite3_step(stmt);
        try { //block 4
            if (rc != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 4, "handleActivity()");
            exitCode = 1;
            return false;
        }
        sqlite3_finalize(stmt);
        return true;
    }


    bool QueryClass::insertActivity(const std::string &name) {
        sqlite3_stmt *stmt;
        const char *sql = "INSERT OR IGNORE INTO activity (name, active) VALUES (?, 1);";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 4
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 4, "handleActivity()");
            return false;
        }
        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        try { //block 5
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 5, "handleActivity()");
            return false;
        }
        rc = sqlite3_step(stmt);
        try { //block 6
            if (rc != SQLITE_DONE)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 6, "handleActivity()");
            if (ecode != SQLITE_CONSTRAINT) {
                return false;
            }
        }
        aChange = true;
        sqlite3_finalize(stmt);
        return true;
    }

    bool QueryClass::getActivityID(const std::string &name) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id FROM activity WHERE name = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 7
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 7, "handleActivity()");
            return false;
        }
        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        try { //block 8
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 8, "handleActivity()");
            return false;
        }
        rc = sqlite3_step(stmt);
        try { //block 9
            if (rc != SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW) {
                std::cerr << "No activity found with name " << name << std::endl;
                return false;
            }
            currentActivity = sqlite3_column_int(stmt, 0);
        } catch (int ecode) {
            handleError(ecode, 9, "handleActivity()");
            return false;
        }
        sqlite3_finalize(stmt);
        return true;
    }

    bool QueryClass::activityChanged() {
        if (aChange){
            aChange = false;
            return true;
        }
        return false;
    }

    void QueryClass::insertProgram(const data& d) {
        sqlite3_stmt *stmt;
        const char *sql = "INSERT OR IGNORE INTO program (name, type) VALUES (?, ?);";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK) {
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 1, "insertProgram()");
            return;
        }
        rc = sqlite3_bind_text(stmt, 1, d.programName.c_str(), -1, SQLITE_STATIC);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "insertProgram()");
            return;
        }
        insertType(d.typeName);
        int typeID = getTypeID(d.typeName);
        if (typeID == 0)
            typeID = 1;
        rc = sqlite3_bind_int(stmt, 2, typeID);
        try { //block 3
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 3, "insertProgram()");
            return;
        }
        rc = sqlite3_step(stmt);
        try { //block 4
            if (rc != SQLITE_DONE)
                throw (rc);

        } catch (int ecode) {
            handleError(ecode, 4, "insertProgram()");
            if (ecode != SQLITE_CONSTRAINT)
                return;
        }
        sqlite3_finalize(stmt);

    }

    int QueryClass::getProgramID(const std::string & name) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id FROM program WHERE name = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "getProgramID()");
            return 0;
        }
        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "getProgramID()");
            return 0;
        }
        int programID;
        rc = sqlite3_step(stmt);
        //following block should get the
        try { //block 3
            if (rc != SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW) {
                std::cerr << "No program found with name " << name << std::endl;
                return 0;
            }
            programID = sqlite3_column_int(stmt, 0);
        } catch (int ecode) {
            handleError(ecode, 3, "getProgramID()");
            return 0;
        }
        sqlite3_finalize(stmt);
        return programID;
    }

    void QueryClass::insertType(const std::string & type) {
        sqlite3_stmt  *stmt;
        const char *sql = "INSERT OR IGNORE INTO type (name) VALUES (?);";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK) {
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 1, "insertType()");
            return;
        }
        rc = sqlite3_bind_text(stmt, 1, type.c_str(), -1, SQLITE_STATIC);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "insertType()");
            return;
        }
        rc = sqlite3_step(stmt);
        try { //block 3
            if (rc != SQLITE_DONE)
                throw (rc);

        } catch (int ecode) {
            handleError(ecode, 3, "insertType()");
            if (ecode != SQLITE_CONSTRAINT)
                return;
        }
        sqlite3_finalize(stmt);
    }

    int QueryClass::getTypeID(const std::string & name) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id FROM type WHERE name = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "getTypeID()");
            return 0;
        }
        rc = sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "getTypeID()");
            return 0;
        }
        int typeID;
        rc = sqlite3_step(stmt);
        try { //block 3
            if (rc != SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW) {
                std::cerr << "No type found with name " << name << std::endl;
                return 0;
            }
            typeID = sqlite3_column_int(stmt, 0);
        } catch (int ecode) {
            handleError(ecode, 3, "getTypeID()");
            return 0;
        }
        sqlite3_finalize(stmt);
        return typeID;
    }

    void QueryClass::handlePTime(data& d) {
        pTime ptimeData;
        ptimeData.date = d.date; //sets date
        ptimeData.utsStart = d.time; //sets time
        insertProgram(d); //inserts the program if it doesn't exist
        ptimeData.pid = getProgramID(d.programName); //gets the program id using the name of the program
        ptimeData.aid = currentActivity; //sets the current activity
        if (lastPTimeID != 0) {
            editPtime(lastPTimeID, d.time); //edits the currently running ptime by changing total time
        }
        if (getCurrentPTime(ptimeData)){ //if ptime exists edit utsStart
            sqlite3_stmt *stmt;
            const char *sql = "UPDATE pTime SET utsStart = ? WHERE id = ?;";
            rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
            try { //block 1
                if (rc != SQLITE_OK)
                    throw (rc);
            } catch (int ecode) {
                handleError(ecode, 1, "handlePTime()");
                exitCode = 1;
                return;
            }
            rc = sqlite3_bind_int(stmt, 1, lastPTimeID);
            try { //block 2
                if (rc != SQLITE_OK)
                    throw (rc);
            } catch (int ecode) {
                handleError(ecode, 2, "handlePTime()");
                exitCode = 1;
                return;
            }
            rc = sqlite3_step(stmt);
            try { //block 3
                if (rc != SQLITE_DONE) {
                    sqlite3_finalize(stmt);
                    throw (rc);
                }
            } catch (int ecode) {
                handleError(ecode, 3, "handlePTime()");
                exitCode = 1;
                return;
            }
            sqlite3_finalize(stmt);
            getCurrentPTime(ptimeData);
            return;
        }
        insertPtime(ptimeData); //if ptime doesn't exist
        getCurrentPTime(ptimeData);

    }

    void QueryClass::insertPtime(pTime & pt) {
        sqlite3_stmt *stmt;
        const char *sql = "INSERT OR IGNORE INTO pTime (pid, utsStart, timeUsed, activity, date) VALUES (?, ?, 0, ?, ?);";
        rc = sqlite3_prepare_v2(db, sql,  -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK) {
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 1, "insertPtime()");
            return;
        }
        rc = sqlite3_bind_int(stmt, 1, pt.pid);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "insertPtime()");
            return;
        }
        rc = sqlite3_bind_int(stmt, 2, pt.utsStart);
        try { //block 3
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 3, "insertPtime()");
            return;
        }
        rc = sqlite3_bind_int(stmt, 3, pt.aid);
        try { //block 4
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 4, "insertPtime()");
            return;
        }
        rc = sqlite3_bind_text(stmt, 4, pt.date.c_str(), -1, SQLITE_STATIC);
        try { //block 5
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 5, "insertPtime()");
            return;
        }
        rc = sqlite3_step(stmt);
        try { //block 6
            if (rc != SQLITE_DONE)
                throw (rc);

        } catch (int ecode) {
            handleError(ecode, 6, "insertPtime()");
            if (ecode != SQLITE_CONSTRAINT)
                return;
        }
        sqlite3_finalize(stmt);


    }

    bool QueryClass::getCurrentPTime(pTime & pt) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT id FROM pTime WHERE pid = ? AND date = ? AND activity = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "getCurrentPTime()");
            return false;
        }
        rc = sqlite3_bind_int(stmt, 1, pt.pid); //binds pid
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "getCurrentPTime()");
            return false;
        }
        rc = sqlite3_bind_text(stmt, 2, pt.date.c_str(), -1, SQLITE_STATIC); //binds date
        try { //block 3
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 3, "getCurrentPTime()");
            return false;
        }
        rc = sqlite3_bind_int(stmt, 3, pt.aid); //binds activity
        try { //block 4
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 4, "getCurrentPTime()");
            return false;
        }
        rc = sqlite3_step(stmt);
        try { //block 5
            if (rc != SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW) {
                //std::cerr << "Current pTime not found with given parameters: pid: " << pt.pid << " date: " << pt.date << " activity: " << pt.aid << std::endl;
                return false;
            }
            lastPTimeID = sqlite3_column_int(stmt, 0);
        } catch (int ecode) {
            handleError(ecode, 5, "getCurrentPTime()");
            return false;
        }
        sqlite3_finalize(stmt);
        return true;
    }

    void QueryClass::editPtime(int &id, int &uts) {
        int tu = getTimeUsed(id) + (uts - utss);
        sqlite3_stmt *stmt;
        const char *sql = "UPDATE pTime SET timeUsed = ? WHERE id = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            sqlite3_finalize(stmt);
            handleError(ecode, 1, "editPTime()");
            exitCode = 1;
            return;
        }
        rc = sqlite3_bind_int(stmt, 1, tu); //binds the new time used to timeUsed
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            sqlite3_finalize(stmt);
            handleError(ecode, 2, "editPTime()");
            exitCode = 1;
            return;
        }
        rc = sqlite3_bind_int(stmt, 2, id); //binds id to id
        try { //block 3
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            sqlite3_finalize(stmt);
            handleError(ecode, 3, "editPTime()");
            exitCode = 1;
            return;
        }
        rc = sqlite3_step(stmt);
        try { //block 4
            if (rc != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                throw (rc);
            }
        } catch (int ecode) {
            handleError(ecode, 4, "editPTime()");
            exitCode = 1;
            return;
        }
        sqlite3_finalize(stmt);

    }

    int QueryClass::getTimeUsed(int id) {
        sqlite3_stmt *stmt;
        const char *sql = "SELECT utsStart, timeUsed FROM pTime WHERE id = ?;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try { //block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "getTimeUsed()");
            return false;
        }
        rc = sqlite3_bind_int(stmt, 1, id);
        try { //block 2
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 2, "getTimeUsed()");
            return false;
        }
        int timeUsed;
        rc = sqlite3_step(stmt);
        try { //block 3
            if (rc != SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW) {
                std::cerr << "No pTime found with id: " << id << std::endl;
                return 0;
            }
            utss = sqlite3_column_int(stmt, 0);
            timeUsed = sqlite3_column_int(stmt, 1);
        } catch (int ecode) {
            handleError(ecode, 3, "getTimeUsed()");
            return 0;
        }
        sqlite3_finalize(stmt);
        //std::cerr << timeUsed << " " << id << std::endl;
        return timeUsed;
    }

    std::queue<pTime> QueryClass::returnData() {
        std::queue<pTime> rData;
        sqlite3_stmt *stmt;
        const char *sql = "SELECT * FROM pTime;";
        rc = sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr);
        try{//block 1
            if (rc != SQLITE_OK)
                throw (rc);
        } catch (int ecode) {
            handleError(ecode, 1, "returnData()");
            return rData;
        }
        rc = sqlite3_step(stmt);
        try { //block 2
            if (rc!= SQLITE_DONE && rc != SQLITE_ROW)
                throw (rc);
            if (rc != SQLITE_ROW){
                std::cerr << "No pTime data found \n";
                return rData;
            }
        } catch (int ecode){
            handleError(ecode, 2, "returnData()");
            return rData;
        }
        //insert the retrieval of the row data to create a ptime struct object to push onto rData
        //insert while loop where while rc == SQLITE_ROW the loop continues or something, gotta look into how retrieving multiple rows works
//        rData.push()
        return rData;
    }

    std::queue<pTime> QueryClass::returnDataRestricted() {
        return std::queue<pTime>();
    }


}