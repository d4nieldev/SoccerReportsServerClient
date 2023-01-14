#include "../include/InputManager.h"
#include "../include/StompFrame.h"
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <sstream>

using std::string;
using std::vector;
using std::unordered_map;
using std::pair;
using std::stringstream;

InputManager::InputManager(User& _user, StompProtocol& _protocol) : user(_user), protocol(_protocol) {}

void InputManager::run(string loginLine){
    string line = loginLine;

    while(1){
        // split line by spaces
        vector<string> words;
        string token;
        std::istringstream tokenStream(line);
        while (std::getline(tokenStream, token, ' '))
            words.push_back(token);

        // process the line
        vector<string> frames;
        if (words[0] == "login" || words[0] == "join" || words[0] == "exit" || words[0] == "report" || words[0] == "logout"){
            frames = protocol.process(words);
            std::cout << "SENDING " << frames.size() << " FRAMES" << std::endl;
            // split the message to frames and send one by one
            for (string frame : frames){
                if (!user.getConnectionHandler().sendFrame(frame)) {
                    std::cout << "Disconnected. Exiting...\n" << std::endl;
                    break;
                }
            }
            if (words[0] == "login"){
                std::cout << "waiting for server to confirm connect..." << std::endl;
                std::unique_lock<std::mutex> lck(user.mtx);
                user.cv.wait(lck, [this](){ return user.isLoggedIn(); });
                std::cout << "connected" << std::endl;
            }
        }
        else if (words[0] == "summary"){
            string topic = words[1];
            string userToGetData = words[2];
            string file = words[3];

            string team_a = topic.substr(0, topic.find("_"));
            string team_b = topic.substr(topic.find("_") + 1, topic.length() - topic.find("_") - 1);

            string output = team_a + " vs " + team_b;
            output += "\nGame stats:";
            output += "\nGeneral stats:";
            output += user.getGeneralStats(topic, userToGetData) + "\n";
            output += "\n" + team_a + " stats:";
            output += user.getTeamAStats(topic, userToGetData) + "\n";
            output += "\n" + team_b + " stats:";
            output += user.getTeabBStats(topic, userToGetData) + "\n";
            output += "\nGame event reports:";
            output += user.getGameEventReports(topic, userToGetData);

            // write to file
            std::ofstream myFile(file, std::ios::trunc);
            if (myFile.is_open()) {
                myFile << output << std::endl;
                myFile.close();
            } else 
                std::cout << "Unable to open file." << std::endl;
        }
        else
            std::cout << "The command given is invalid. Please try again" << std::endl;

        if (user.isLoggedIn()){
            // get a new line
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); //blocked 
            line = buf;
        }
        else
            break;
    }
}

unsigned int InputManager::getFinalReceiptId()
{
    return protocol.getLastReceiptId();
}
