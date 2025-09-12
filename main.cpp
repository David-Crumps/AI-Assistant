#include <iostream>

#include <cstdlib>
#include <windows.h>

#include <stdio.h>
#include <string>
#include <lmcons.h>

#include <ctime>
#include <chrono>


void outputToVoice(std::string phrase) {
    std::string command = "espeak \"" + phrase + "\"";
    system(command.c_str());
}

void greeting() {
    std::string output;

    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm* local_tm = std::localtime(&now_c);

    output = (local_tm->tm_hour < 12) ? "Good Morning" :
        (local_tm->tm_hour >= 12 && local_tm->tm_hour < 17) ? "Good Afternoon" : "Good Evening";

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

int main()
{
    system("cls");
    std::cout << "*******************************" << std::endl;
    std::cout << "PERSONAL ASSISSTANT INTIALISING" << std::endl;
    std::cout << "*******************************" << std::endl;
    greeting();

    return 0;
}
