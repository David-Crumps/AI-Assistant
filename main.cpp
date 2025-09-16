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

#include <algorithm>
#include <cctype>
#include <regex>

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

namespace UrlRegex {
    const std::regex& getUrlRegex() {
        static const std::regex url_pattern(R"(^(https?://)?(www\.[a-zA-Z0-9.-]+\.[a-zA-Z]{2,})(:[0-9]+)?(/[\w\-./&&=]*)?$)");
        return url_pattern;
    }
}



void outputToVoice(const std::string& phrase) {
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

void closeAssistant() {
    outputToVoice("Closing program, goodbye");
    exit(0);
}

void help() {
    std::cout << "*************************" << std::endl;
    std::cout << "-> exit : Closes program" <<std::endl;
    std::cout << "*************************" <<std::endl;
    std::cout << "-> website <website_name> : Opens website either with predefined names or the full URL" << std::endl;
    std::cout << "Predefined website_names: (youtube, github, twitch.tv, reddit)\nEnter the full URL for a name not currently defined." << std::endl;
    std::cout << "****************************************************" << std::endl;
    std::cout << "->open <program> : opens program, with with a predefined name or the full path to said program" << std::endl;
    std::cout << "**********************************************************************************************" << std::endl;
}

void openWebsite(const std::string& keyword) {
    const auto& map = SiteRegistry::getSiteMap();
    const auto& regexUrl = UrlRegex::getUrlRegex();
    auto it = map.find(keyword);

    //Convert to OR statement
    if (it != map.end()) {
        std::string command = "start "+it->second;
        const char *charCommand = command.c_str();
        system(charCommand);
    } else if (std::regex_match(keyword, regexUrl)) {
        std::string command = "start "+keyword;
        const char *charCommand = command.c_str();
        system(charCommand);
    } else {
        std::cout << "Incorrect URL" << std::endl;
        std::cout << "*************" << std::endl;
        outputToVoice("Incorrect URL");
    }

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
    commandMap["exit"] = NoArgCommand(closeAssistant);

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

        std::transform(command.begin(), command.end(), command.begin(), [](unsigned char c){return std::tolower(c); });

        auto it = commandMap.find(command);
        if (it != commandMap.end()) {
            executeCommand(it->second, keyword);
        }


    } while (1);
    return 0;
}
