#include "Logger.h"
#include <iostream>
#include <ctime>
#include <fstream>
using namespace std;

vector<LogEntry> Logger::messages;

string timeStamp() {
    time_t time = std::time({});
    string timeString(30, '\0');
    strftime(&timeString[0], timeString.size(), "%b %d %Y, %T", localtime(&time));
    return timeString;
}

void Logger::Log(const std::string& message){
    LogEntry entry;
    entry.type = LOG_INFO;
    entry.message = "INFO | " + timeStamp() + " - " + message;
    cout << "\033[1;92m" << entry.message << "\033[0m\n"; 
    messages.push_back(entry);
    // Logger::Save(entry.message);
}

void Logger::Warn(const std::string& message){
    LogEntry entry;
    entry.type = LOG_WARNING;
    entry.message = "WARNING | " + timeStamp() + " - " + message;
    cout << "\033[1;93m" << entry.message << "\033[0m\n";
    messages.push_back(entry);
}

void Logger::Err(const std::string& message){
    LogEntry entry;
    entry.type = LOG_ERROR;
    entry.message = "ERROR | " + timeStamp() + " - " + message;
    cout << "\033[1;31m" << entry.message << "\033[0m\n";
    messages.push_back(entry);
}

void Logger::Save(const std::string& message){
    ofstream logFile;
    logFile.open("./src/Logger/logs/session.txt");
    if (logFile.is_open()){
        logFile << message + "\n";
        logFile.close();
    } 
    else cout << "Unable to open file"; 
}
