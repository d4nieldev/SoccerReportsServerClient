#include "../include/StompProtocol.h"
#include "../include/User.h"
#include "../include/event.h"


using std::vector;
using std::pair;

StompProtocol::StompProtocol(User& _user) : user(_user), nextReceiptId(0), nextSubscriptionId(0) {}

unsigned int StompProtocol::getLastReceiptId()
{
    return nextReceiptId - 1;
}

string StompProtocol::process(vector<string> words)
{
    string command;
    unordered_map<string,string> headers;
    string body;
    string answer;

    if (words[0] == "login"){
        if (words.size() != 4)
            throw std::invalid_argument("invalid arguments for login command");

        std::cout << "got login command" << std::endl;

        command = "CONNECT";
        headers.insert(pair<string,string>("accept-version", "1.2"));
        headers.insert(pair<string,string>("host", words[1]));
        headers.insert(pair<string,string>("login", words[2]));
        headers.insert(pair<string,string>("passcode", words[3]));
        body = "";

        StompFrame s(command, headers, body);
        answer = s.toString();
    }
    else if (words[0] == "join"){
        if (words.size() != 2)
            throw std::invalid_argument("invalid arguments for join command");
            
        topicToSubId.insert(pair<string,int>(words[1], nextSubscriptionId));

        command = "SUBSCRIBE";
        headers.insert(pair<string,string>("destination", words[1]));
        headers.insert(pair<string,string>("id", std::to_string(nextSubscriptionId++)));
        headers.insert(pair<string,string>("receipt", std::to_string(nextReceiptId++)));
        body = "";

        StompFrame s(command, headers, body);
        answer = s.toString();
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

        StompFrame s(command, headers, body);
        answer = s.toString();
    }
    else if (words[0] == "report") {
        if (words.size() != 2)
            throw std::invalid_argument("invalid arguments for join command");
        
        names_and_events namesAndEvents = parseEventsFile(words[1]);
        for (Event e : namesAndEvents.events){
            command = "SEND";

            headers.insert(pair<string, string>("destination", e.get_team_a_name() + "_" + e.get_team_b_name()));

            body = "user:"+user.getUserName();
            body += "\nteam a:"+e.get_team_a_name();
            body += "\nteam b:"+e.get_team_b_name();
            body += "\nevent name:"+e.get_name();
            body += "\ntime:"+e.get_time();
            body += "\ngeneral game updates:";
            for (auto it = e.get_game_updates().begin(); it != e.get_game_updates().end(); it++)
                body += "\n\t" + it->first + ":" + it->second;
            body += "\nteam a updates:";
            for (auto it = e.get_team_a_updates().begin(); it != e.get_team_a_updates().end(); it++)
                body += "\n\t" + it->first + ":" + it->second;
            body += "\nteam b updates:";
            for (auto it = e.get_team_b_updates().begin(); it != e.get_team_b_updates().end(); it++)
                body += "\n\t" + it->first + ":" + it->second;
            body += "\ndescription:";
            body += "\n" + e.get_discription();
            
            StompFrame s(command, headers, body);
            answer += s.toString();
        }
    }
    else if (words[0] == "logout") {
        if (words.size() != 1)
            throw std::invalid_argument("invalid arguments for join command");

        user.logout();

        command = "DISCONNET";
        headers.insert(pair<string,string>("receipt", ""+nextReceiptId++));
        body = "";

        StompFrame s(command, headers, body);
        answer = s.toString();
    }

    return  answer;
}

bool StompProtocol::process(StompFrame frame)
{
    if (frame.getCommand() == "CONNECTED"){
        user.login();
        return true;
    }
    else if (frame.getCommand() == "MESSAGE"){
        user.parseMessage(frame.getHeaders()["destination"], frame.getBody());
        return true;
    }
    else if (frame.getCommand() == "ERROR")
        return false;
    else if (frame.getCommand() == "RECEIPT")
        return true;

    return false;
}
