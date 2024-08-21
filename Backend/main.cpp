#include <iostream>
#include "QueryClass.h"
#include "WWindow.h"

int main() {
    unsigned char exitCode = 0; //everywhere where exit code is present that will turn into an error popup in the frontend this is just a placeholder
    Q::QueryClass qC(exitCode);
    W::WWindow changeCheck(qC, exitCode);
    //in place of front end for now
    std::string done;
    int counter = 0;
    std::chrono::seconds wait(5);
    Q::data d;
    while (done != "t")
    {
        std::cout << "Enter 'a' to insert and/or start an activity.\n";
        std::cout << "Enter 't' to terminate program:\n";
        std::cin >> done;
        if (done == "a"){
            if (counter == 0 || counter == 3)
                done = "test with spaces";
            else if (counter == 4)
                done = "Free Time";
            else {
                std::cout << "Enter activity to start: ";
                std::cin >> done;
            }
            Q::data d;
            d.table = Q::ACTIVITY;
            d.activityName = done;
            qC.handleTraffic(d);
            done = "";
            counter++;
        }

    }

    changeCheck.stopMonitoring();
    qC.endLoop();
    return exitCode;
}