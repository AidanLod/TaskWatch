
//
// Created by main on 6/2/24.
//
#include "WWindow.h"

W::WWindow::WWindow(Q::QueryClass& db, unsigned char& ec): exitCode(ec), database(db) {
    eFile = freopen("ErrorOutput.txt", "w", stderr);
    if (eFile == nullptr) {
        std::cerr << "Error redirecting stderr to file." << std::endl;
        exitCode = 1;
    }
    monitoring.store(false);
    display = XOpenDisplay(nullptr);
    if (!display) {
        std::cerr << "Failed to open X display." << std::endl;
        exitCode = 1;
    }
    startMonitoring();
    getDate();
}

W::WWindow::~WWindow() {
    XCloseDisplay(display);
    if(monitoring.load())
    {
        stopMonitoring();
    }
}

void W::WWindow::startMonitoring() {
    d.activityName = "Free Time";
    d.table = Q::ACTIVITY;
    d.time = static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now())); //this line gets the current unix timestamp which is of type time_point, it is then converted to time_t which is typically a long, and then it is static cast as an int
    database.handleTraffic(d);
    monitoring.store(true);
    mLoop = std::thread(&WWindow::monitoringLoop, this);
    mLoop.detach();
}

void W::WWindow::stopMonitoring() {
    monitoring.store(false);
    mLoop.join();
}

void W::WWindow::monitoringLoop() {
    while (monitoring.load()){
        std::this_thread::sleep_for(std::chrono::milliseconds (1250));
        time = static_cast<int>(std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()));
        getWindowName("_NET_WM_NAME");
        if (activeWindow != previousWindow){
//            std::cerr << currentWindowName << std::endl;
            previousWindow = activeWindow;
            getClassName();
            previousWindowName = currentWindowName;
//            std::cerr << time << " monitoringLoop\n";
            getDate();
            std::string p = findFile(currentClassName); //finds a .desktop file with the same Icon name
            p.pop_back();
            d.table = Q::PTIME;
            d.time = time;
            d.date = date;
            d.programName = currentWindowName;
            d.className = currentClassName;
            p = getWinCategory(p);
            d.typeName = refineWinCategory(p);
            database.handleTraffic(d);

        }
        else if(database.activityChanged() || currentWindowName != previousWindowName){
//            std::cerr << currentWindowName << std::endl;
            previousWindowName = currentWindowName;
            d.programName = currentWindowName;
            d.time = time;
            database.handleTraffic(d);
        }
    }
}


void W::WWindow::getWindowName(const std::string& winEl) {

    Atom actualType;
    int actualFormat;
    unsigned long nItems, bytesAfter;
    unsigned char* prop = nullptr;

    // Get the atom for _NET_ACTIVE_WINDOW
    Atom netActiveWindow = XInternAtom(display, "_NET_ACTIVE_WINDOW", True);
    if (netActiveWindow == None) {
        std::cerr << "Failed to get _NET_ACTIVE_WINDOW atom." << std::endl;
        return;
    }

    // Use the atom to get the property of the root window
    if (XGetWindowProperty(display, DefaultRootWindow(display), netActiveWindow, 0, ~0L, False,
                           AnyPropertyType, &actualType, &actualFormat, &nItems,
                           &bytesAfter, &prop) != Success) {
        std::cerr << "Failed to get _NET_ACTIVE_WINDOW property." << std::endl;
        return;
    }

    if (nItems == 0) {
        std::cerr << "No active window found." << std::endl;
        return;
    }

    activeWindow = *(Window*)prop;
    XFree(prop);

    // Get the atom for _NET_WM_NAME
    Atom netWmName = XInternAtom(display, winEl.c_str(), True);
    if (netWmName == None) {
        std::cerr << "Failed to get "<< winEl << " atom." << std::endl;
        return;
    }

    // Use the atom to get the window name property
    if (XGetWindowProperty(display, activeWindow, netWmName, 0, ~0L, False,
                           AnyPropertyType, &actualType, &actualFormat, &nItems,
                           &bytesAfter, &prop) != Success) {
        std::cerr << "Failed to get "<< winEl << " property." << std::endl;
        return;
    }

    currentWindowName = "";
    if (prop) {
        currentWindowName = std::string(reinterpret_cast<char*>(prop));
        XFree(prop);
    }
    if (currentWindowName.empty()) {
        std::cerr << "Failed to get the active window name." << std::endl;
        exitCode = 1;
    }
}

void W::WWindow::getClassName() {
    XClassHint classHint;
    if (XGetClassHint(display, activeWindow, &classHint)){
        currentClassName = classHint.res_class;
//        std::cerr << currentClassName << std::endl;
    }
    else{
        std::cerr << "Failed to get WM_CLASS for window\n";
    }

}

/*pid_t W::WWindow::getWindowPID() {
    Atom actualType;
    int actualFormat;
    unsigned long numItems, bytesAfter;
    unsigned char* prop = nullptr;
    Atom pidAtom = XInternAtom(display, "_NET_WM_PID", True);
    if (pidAtom == None) {
        std::cerr << "No _NET_WM_PID property found\n";
        return -1;
    }
    if (XGetWindowProperty(display, activeWindow, pidAtom, 0, 1, False, XA_CARDINAL, &actualType, &actualFormat, &numItems,
                           &bytesAfter, &prop) != Success) {
        std::cerr << "Cannot get _NET_WM_PID property\n";
        return -1;
    }
    pid_t pid = *(pid_t*)prop;
    XFree(prop);
    return pid;
}

std::string W::WWindow::getPath(pid_t &pid) {
    char retPath [PATH_MAX];
    std::string path = "/proc/" + std::to_string(pid) + "/exe";
    ssize_t len = readlink(path.c_str(), retPath, sizeof(retPath) - 1);
    if (len == -1) {
        std::cerr << "Cannot read executable path for PID " << pid << "\n";
        return "";
    }
    retPath[len] = '\0';
    return std::string(retPath);

}*/

std::string W::WWindow::findFile(std::string &className) { //fix this to make it more stable
    std::vector<std::string> searchPaths = {"/usr/share/applications/", "/var/lib/flatpak/exports/share/applications/", "/usr/local/share/applications/"};

    for (const std::string& dir : searchPaths) {
        std::string command = "grep -li \"Name=" + className + "\" " + dir + "*.desktop";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Cannot run grep command\n";
            continue;
        }
        char buffer[PATH_MAX];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            return std::string(buffer);
        }
        pclose(pipe);
    }
    //second try with search for Exec with case-insensitive exact match
//    std::cerr << "second search\n";
    for (const std::string& dir : searchPaths) {
        std::string command = "grep -lix \"Exec=" + className + "\" " + dir + "*.desktop";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Cannot run grep command\n";
            continue;
        }
        char buffer[PATH_MAX];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            return std::string(buffer);
        }
        pclose(pipe);
    }
    //third try with search for Exec without exact match
//    std::cerr << "third search\n";
    for (const std::string& dir : searchPaths) {
        std::string command = "grep -li \"Exec=" + className + "\" " + dir + "*.desktop";
        FILE* pipe = popen(command.c_str(), "r");
        if (!pipe) {
            std::cerr << "Cannot run grep command\n";
            continue;
        }
        char buffer[PATH_MAX];
        if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
            pclose(pipe);
            return std::string(buffer);
        }
        pclose(pipe);
    }
    //fourth try where goes through and checks each part of the string
//    std::cerr << "fourth search\n";
    std::regex specialCharRegex("[^a-zA-Z0-9]+"); //excludes all alphanumeric characters
    std::sregex_token_iterator end;
    std::sregex_token_iterator iter(className.begin(), className.end(), specialCharRegex, -1);
    while (iter != end) {
        std::string temp = iter->str();
        for (int i = 0; i < 2; i++) {
            for (const std::string &dir: searchPaths) {
                std::string command = "grep -li \"" + temp + "\" " + dir + "*.desktop";
                FILE *pipe = popen(command.c_str(), "r");
                if (!pipe) {
                    std::cerr << "Cannot run grep command\n";
                    continue;
                }
                char buffer[PATH_MAX];
                if (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
                    pclose(pipe);
//                    std::cerr << "found\n";
                    return std::string(buffer);
                }

                pclose(pipe);
            }
            for (char& c : temp)
            {
                c = std::tolower(static_cast<unsigned char>(c));
            }
        }
        iter++;
    }
    std::cerr << "No .desktop file found for an application with the name " << className << "\n";
    return "Misc";
}

std::string W::WWindow::getWinCategory(std::string &filePath) {
    std::ifstream desFile(filePath);
    if (!desFile.is_open()) {
        std::cerr << "Cannot open .desktop file: " << filePath<< std::endl;
        std::cerr << "Error code: " << strerror(errno) << std::endl;
        return "Misc";
    }

    std::string line;
    while (std::getline(desFile, line)) {
        if (line.find("Categories=") == 0) {
//            std::cerr << line << std::endl;
            return line.substr(10);
        }
    }
    std::cerr << "No Categories found in .desktop file " << filePath << "\n";
    return "Misc";

}

std::string W::WWindow::refineWinCategory(const std::string& inCat) {
    if (inCat.find("Game") != std::string::npos){
        return "Games";
    }
    if (inCat.find("Settings") != std::string::npos){
        return "Settings";
    }
    if (inCat.find("Development") != std::string::npos){
        return "Development";
    }
    if (inCat.find("Education") != std::string::npos){
        return "Education";
    }
    if (inCat.find("Graphics") != std::string::npos){
        return "Graphics";
    }
    if (inCat.find("Network") != std::string::npos){
        return "Internet";
    }
    if (inCat.find("Science") != std::string::npos){
        return "Science";
    }
    if (inCat.find("System") != std::string::npos || inCat.find("Emulator") != std::string::npos){
        return "System";
    }
    if (inCat.find("Accessibility") != std::string::npos || inCat.find("Core") != std::string::npos ||
            inCat.find("Legacy") != std::string::npos || inCat.find("Utility") != std::string::npos){
        return "Accessories";
    }
    return "Other";
}

void W::WWindow::getDate() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowc = std::chrono::system_clock::to_time_t(now);
    std::tm *local_time = std::localtime(&nowc);
    std::ostringstream oss;
    oss << std::put_time(local_time, "%Y-%m-%d");
    date = oss.str();
}







