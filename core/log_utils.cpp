#include "ServerInstance.hpp"


const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

    return buf;
}


void Http_application::serverLog (int serverIndx)
{
    logFile << "connection count : "<< connectionCount  << " ";
    logFile <<  "\e[0;32mserver name: \e[0m: " <<     serverList[serverIndx]->getServerName () << " ";
    logFile << "\e[0;32mserver port: \e[0m:" << serverList[serverIndx]->getServerPort () << " ";
    logFile << " " << currentDateTime ()  <<  std::endl;
}