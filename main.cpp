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

#include <unordered_map>
#include <functional>
#include <variant>

using NoArgCommand = std::function<void()>;
using ArgCommand = std::function<void(const std::string&)>;
using CommandVariant = std::variant<NoArgCommand, ArgCommand>;

namespace SiteRegistry {
    const std::unordered_map<std::string, std::string>& getSiteMap() {
    static const std::unordered_map<std::string, std::string> siteMap = {
        {"youtube", "https://www.youtube.com"}
    };
    return siteMap;
  }
}



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
            outputToVoice(output+" "+username+ ". How may I assist you today?");
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

void help() {
    std::cout << "Help" << std::endl;
}

void openWebsite(const std::string& url) {
    std::cout << "Successfully opened URL" << std::endl;

}

void executeCommand(const CommandVariant& cmd, const std::string& arg = "") {
    if (std::holds_alternative<NoArgCommand>(cmd)) {
        std::get<NoArgCommand>(cmd)();
    }
    else if (std::holds_alternative<ArgCommand>(cmd)) {
        std::get<ArgCommand>(cmd)(arg);
    }
}


int main()
{
    std::unordered_map<std::string, CommandVariant> commandMap;
    commandMap["help"] = NoArgCommand(help);
    commandMap["website"] = ArgCommand(openWebsite);

    system("cls");
    std::cout << "*******************" << std::endl;
    std::cout << "PERSONAL ASSISSTANT" << std::endl;
    std::cout << "*******************" << std::endl;
    std::cout << "How may I assist you today?" << std::endl;
    greeting();
    std::cout << "***************************" << std::endl;
    std::cout << "Type \"help\" to view commands" << std:: endl;
    outputToVoice("Type help to view commands.");

    do{
        std::string input;
        getline(std::cin, input);
        std::istringstream iss(input);
        std::string command, keyword;
        iss >> command >> keyword;

        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            executeCommand(it->second, keyword);
        }


    } while (1);
    return 0;
}
