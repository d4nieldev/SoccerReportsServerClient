#include "../include/StompProtocol.h"
#include "../include/User.h"
#include "../include/event.h"


using std::vector;
using std::pair;

StompProtocol::StompProtocol(User& _user) : nextReceiptId(0), nextSubscriptionId(0), user(_user) {}

unsigned int StompProtocol::getLastReceiptId()
{
    return nextReceiptId - 1;
}

string StompProtocol::process(vector<string> words)
{
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
        
        names_and_events namesAndEvents = parseEventsFile(words[1]);
        
        command = "SEND";
        headers.insert(pair<string, string>("destination", namesAndEvents.team_a_name + "_" + namesAndEvents.team_b_name));
        body
    }
    else if (words[0] == "logout") {
        if (words.size() != 1)
            throw std::invalid_argument("invalid arguments for join command");

        user.logout();

        command = "DISCONNET";
        headers.insert(pair<string,string>("receipt", ""+nextReceiptId++));
        body = "";
    }

    
    StompFrame s(command, headers, body);
    return s.toString();
}

bool StompProtocol::process(StompFrame frame)
{
    if (frame.getCommand() == "CONNECTED")
        user.login();

    return false;
}
