#include "../include/InputManager.h"
#include "../include/StompFrame.h"
#include <vector>
#include <stdexcept>
#include <unordered_map>

using std::string;
using std::vector;
using std::unordered_map;
using std::pair;

InputManager::InputManager(StompConnectionHandler& _ch, bool _shouldTerminate) : 
    ch(_ch), shouldTerminate(_shouldTerminate), isLoggedIn(false), nextReceiptId(0), nextSubscriptionId(0) {}

void InputManager::run(){
    while(!shouldTerminate){
        const short bufsize = 1024;
        char buf[bufsize];
        std::cin.getline(buf, bufsize); //blocked 
        string line(buf);

        std::string delimiter = " ";
        size_t pos = 0;
        std::string token;

        vector<string> words;

        while ((pos = line.find(delimiter)) != std::string::npos) {
            token = line.substr(0, pos);
            words.push_back(token);
            line.erase(0, pos + delimiter.length());
        }

        string frame;
        if (words[0] == "login" || words[0] == "join" || words[0] == "exit" || words[0] == "report" || words[0] == "logout"){
            frame = getFrameFromLine(words);
            if (!ch.sendFrame(frame)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
        }
        else if (words[0] == "summary")
            // TODO handle summary
            continue;
        else
            std::cout << "The command given is invalid. Please try again";
    }
}

int InputManager::getFinalReceiptId(){
    return nextReceiptId - 1;
}

string InputManager::getFrameFromLine(vector<string> words){
    
    
    string command;
    unordered_map<string,string> headers;
    string body;

    if (words[0] == "login"){
        if (words.size() != 4)
            throw std::invalid_argument("invalid arguments for login command");

        command = "CONNECT";
        headers.insert(pair<string,string>("accept-version", "1.2"));
        headers.insert(pair<string,string>("host", words[1]));
        headers.insert(pair<string,string>("login", words[2]));
        headers.insert(pair<string,string>("passcode", words[3]));
        body = "";
    }
    else if (words[0] == "join"){
        if (words.size() != 2)
            throw std::invalid_argument("invalid arguments for join command");
            
        topicToSubId.insert(pair<string,int>(words[1], nextSubscriptionId));

        command = "SUBSCRIBE";
        headers.insert(pair<string,string>("destination", words[1]));
        headers.insert(pair<string,string>("id", ""+nextSubscriptionId++));
        headers.insert(pair<string,string>("receipt", ""+nextReceiptId++));
        body = "";
    }
    else if (words[0] == "exit"){
        if (words.size() != 2)
            throw std::invalid_argument("invalid arguments for join command");

        int subId = topicToSubId.at(words[1]);
        topicToSubId.erase(words[1]);

        command = "UNSUBSCRIBE";
        headers.insert(pair<string,string>("id", ""+subId));
        headers.insert(pair<string,string>("receipt", ""+nextReceiptId++));
        body = "";
    }
    else if (words[0] == "report") {
        if (words.size() != 2)
            throw std::invalid_argument("invalid arguments for join command");

        // TODO handle report
        command = "SEND";
    }
    else if (words[0] == "logout") {
        if (words.size() != 1)
            throw std::invalid_argument("invalid arguments for join command");

        shouldTerminate = true;

        command = "DISCONNET";
        headers.insert(pair<string,string>("receipt", ""+nextReceiptId++));
        body = "";
    }

    
    StompFrame s(command, headers, body);
    return s.toString();
}