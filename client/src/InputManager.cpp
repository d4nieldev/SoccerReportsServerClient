#include "../include/InputManager.h"
#include "../include/StompFrame.h"
#include <vector>
#include <stdexcept>
#include <unordered_map>
#include "InputManager.h"

using std::string;
using std::vector;
using std::unordered_map;
using std::pair;

InputManager::InputManager(User& _user, StompProtocol& _protocol) : user(_user), protocol(_protocol) {}

void InputManager::run(string loginLine){
    string line = loginLine;

    while(1){
        // split line by spaces
        std::string delimiter = " ";
        size_t pos = 0;
        std::string token;

        vector<string> words;

        while ((pos = line.find(delimiter)) != std::string::npos) {
            token = line.substr(0, pos);
            words.push_back(token);
            line.erase(0, pos + delimiter.length());
        }

        // process the line
        string frame;
        if (words[0] == "login" || words[0] == "join" || words[0] == "exit" || words[0] == "report" || words[0] == "logout"){
            frame = protocol.process(words);
            if (!user.getConnectionHandler().sendFrame(frame)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            if (!user.isLoggedIn()){
                std::cout << "waiting for server to confirm connect..." << std::endl;
                std::unique_lock<std::mutex> lck(user.mtx);
                user.cv.wait(lck, [&]{ return user.isLoggedIn(); });
            }
        }
        else if (words[0] == "summary")
            // TODO handle summary
            continue;
        else
            std::cout << "The command given is invalid. Please try again";

        if (user.isLoggedIn()){
            // get a new line
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); //blocked 
            line = buf;
        }
    }
}

unsigned int InputManager::getFinalReceiptId()
{
    return protocol.getLastReceiptId();
}
