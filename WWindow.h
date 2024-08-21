#ifndef UNTITLED_WWINDOW_H
#define UNTITLED_WWINDOW_H
#include <iostream>
#include <fstream>
#include <X11/Xlib.h>
#include <X11/Xatom.h> // Include Xatom for predefined atoms
#include <X11/Xutil.h>
#include <unistd.h>
#include <atomic>
#include <thread>
#include <mutex>
#include <regex>
#include <queue>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <ctime>
#include <cstring>
#include "QueryClass.h"
#include <sstream>
#include <iomanip>
#include <climits>

namespace W
{
    class WWindow
    {
    public:
        WWindow(Q::QueryClass& db, unsigned char& ec); //construct
        ~WWindow(); //destruct

        void startMonitoring(); //starts a loop that calls getWindowName() every num seconds
        void stopMonitoring(); //stops the loop that calls getWindowName()

    private:
        //functions/methods
        void monitoringLoop();
        void getWindowName(const std::string&); //gets current window name
        void getClassName(); //gets the current windows class name
        std::string findFile(std::string&); //takes the executable path and returns the .desktop path
        std::string getWinType(std::string&); //takes the .desktop path and returns the name of the category that the program falls under
        void getDate(); //gets the current date and returns it as a string mm/dd/yy
        //objects
        FILE* eFile; //holds pointer to newly opened stream
        unsigned char& exitCode;
        int time;
        std::string date;
        Display *display; //holds connection to x server
        std::string currentWindowName; //holds the most up-to-date window name, updates before windowCheck
        std::string previousWindowName; //holds the name of the previous window
        std::string currentClassName; //holds the most up-to-date class name
        Q::QueryClass& database; //holds reference to QueryClass so that mutex locks can be handled properly
        Q::data d;
        Window activeWindow; //current active window
        Window previousWindow; //previous window recorded before verifying there has been a window change
        //threading
        std::thread mLoop; //monitoring loop thread
        std::atomic<bool> monitoring; // handles whether the user desires for the program to continue monitoring
    };

}
#endif //UNTITLED_WWINDOW_H
