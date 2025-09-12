#include <iostream>

#include <cstdlib>
#include <windows.h>

#include <stdio.h>
#include <string>
#include <lmcons.h>
#include <iomanip>
#include <sstream>

#include <ctime>
#include <chrono>


void outputToVoice(std::string phrase) {
    std::string command = "espeak \"" + phrase + "\"";
    system(command.c_str());
}
std::tm currentLocalTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);

    std::tm result;
    localtime_s(&result, &now_c);
    return result;
}

void greeting() {
    std::string output;
    std::tm local_tm = currentLocalTime();

    output = (local_tm.tm_hour < 12) ? "Good Morning" :
        (local_tm.tm_hour >= 12 && local_tm.tm_hour < 17) ? "Good Afternoon" : "Good Evening";

    //Fetching windows username
    TCHAR username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    if (GetUserName(username, &username_len)) {
            outputToVoice(output+" "+username);
    }
    else {
        std::cerr << "Invalid User" << std::endl;
        exit(0);
    }
}

void returnCurrentTime() {
    std::tm currentTime = currentLocalTime();
    std::ostringstream oss;

    std::string period = (currentTime.tm_hour>=12) ? "PM" : "AM";
    int hour = (currentTime.tm_hour)%12;
    if (hour == 0) hour = 12;

    oss << hour << ":" <<currentTime.tm_min << period;
    std::string output = oss.str();

    std::cout << output << std::endl;
    outputToVoice("The current time is "+ output);
}



int main()
{
    system("cls");
    std::cout << "*******************" << std::endl;
    std::cout << "PERSONAL ASSISSTANT" << std::endl;
    std::cout << "*******************" << std::endl;
    greeting();
    do{
        std::cout << "How may I assist you today?" << std::endl;
        outputToVoice("How may I assist you today?");
        std::string command;
        getline(std::cin, command);

        if (command == "What is the time?") {
            returnCurrentTime();
        }
        else if (command == "Exit") {
            exit(0);
        }
    } while (1);
    return 0;
}
