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
        {"youtube", "https://www.youtube.com"},
        {"twitch" , "https://www.twitch.tv"},
        {"reddit", "https://www.reddit.com"},
        {"maxroll", "https://maxroll.gg/"},
        {"poe", "https://www.poe2wiki.net/wiki/Path_of_Exile_2_Wiki"},
        {"github", "https://github.com/David-Crumps"}
    };
    return siteMap;
  }
}

namespace ProgramRegistry {
    const std::unordered_map<std::string, std::string>& getProgramMap() {
        static const std::unordered_map<std::string, std::string> programMap = {
            {"notepad", "C:\\Windows\\notepad.exe"},
            {"word", "C:\\Program Files\\Microsoft Office\\root\\Office16\\WINWORD.exe"},
            {"excel", "C:\\Program Files\\Microsoft Office\\root\\Office16\\EXCEL.exe"},
            {"powerpoint", "C:\\Program Files\\Microsoft Office\\root\\Office16\\POWERPNT.exe"},
            {"guild_wars", "C:\\Guild Wars 2\\Guild Wars 2\\Gw2-64.exe"}
        };
        return programMap;
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
    std::string timeOfDay;
    std::tm local_tm = currentLocalTime();
    std::string output;

    timeOfDay = (local_tm.tm_hour < 12) ? "morning" :
        (local_tm.tm_hour >= 12 && local_tm.tm_hour < 17) ? "afternoon" : "evening";

    //Fetching windows username
    TCHAR username[UNLEN+1];
    DWORD username_len = UNLEN+1;
    if (GetUserName(username, &username_len)) {
            output = "How may I assist you this "+timeOfDay+"?";
            std::cout << output << std::endl;
            outputToVoice("Hello "+std::string(username)+", "+output);
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
    std::cout << "************************" << std::endl;
    std::cout << "-> exit : Closes program" << std::endl;
    std::cout << "************************\n" <<std::endl;

    std::cout << "******************************" << std::endl;
    std::cout << "-> time : Returns current time" << std::endl;
    std::cout << "******************************\n" << std::endl;

    std::cout << "**************************************************************************************" << std::endl;
    std::cout << "-> website <website_name> : Opens website either with predefined names or the full URL" << std::endl;
    std::cout << "<website_name> can either be: (youtube, github, twitch, reddit, poe, maxroll) OR a valid URL" << std::endl;
    std::cout << "********************************************************************************************\n" << std::endl;

    std::cout << "**********************************************************************************************" << std::endl;
    std::cout << "->open <program> : choose a program to open from the provided list" << std::endl;
    std::cout << "<program> -> (notepad, word, excel, powerpoint, guild_wars)" << std::endl;
    std::cout << "**********************************************************************************************" << std::endl;
}

void openWebsite(const std::string& keyword) {
    const auto& map = SiteRegistry::getSiteMap();
    const auto& regexUrl = UrlRegex::getUrlRegex();
    auto it = map.find(keyword);

    if (it != map.end()) {
        std::string command = "start "+it->second;
        const char *charCommand = command.c_str();
        system(charCommand);
    } else if (std::regex_match(keyword, regexUrl)) {
        std::string command = "start "+keyword;
        const char *charCommand = command.c_str();
        system(charCommand);
    } else {
        std::cout << "\nIncorrect URL" << std::endl;
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
    struct ProcessContext {
        STARTUPINFOW si = {};
        PROCESS_INFORMATION pi = {};
    };
    ProcessContext ctx;
    ctx.si.cb = sizeof(ctx.si);
    std::unordered_map<std::string, CommandVariant> commandMap;

    commandMap["help"] = NoArgCommand(help);
    commandMap["website"] = ArgCommand(openWebsite);
    commandMap["exit"] = NoArgCommand(closeAssistant);
    commandMap["time"] = NoArgCommand(returnCurrentTime);

    commandMap["program"] = ArgCommand([&ctx](const std::string& keyword) {
        const auto& map = ProgramRegistry::getProgramMap();
        auto it = map.find(keyword);
        if (it != map.end()){
            std::wstring wideInput = std::wstring(it->second.begin(), it->second.end());
            std::wstring quoted = L"\""+wideInput+L"\"";
            CreateProcessW(NULL, &quoted[0], NULL, NULL, FALSE, 0, NULL, NULL, &ctx.si, &ctx.pi);
        }
    });

    system("cls");
    std::cout << "*******************" << std::endl;
    std::cout << "PERSONAL ASSISSTANT" << std::endl;
    std::cout << "*******************\n" << std::endl;
    greeting();

    std::cout << "Type \"help\" to view commands\n" << std:: endl;
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
        else {
            std::cout<< "Incorrect command" << std::endl;
            outputToVoice("Incorrect command");
        }
        std::cout << "\n";


    } while (true);
    return 0;
}
