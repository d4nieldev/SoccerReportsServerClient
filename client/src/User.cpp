#include "../include/User.h"
#include "User.h"

using std::pair;
using std::stringstream;

User::User(string host, int port, string _userName) : loggedIn(false), ch(host, port), userName(_userName)
{
    if (!ch.connect()) 
        throw std::runtime_error("Cannot connect to " + host + ":" + std::to_string(port));
}

void User::addStat(string topic, string user, int idx, string statName, string statVal)
{
    if (topicToUserStats.count(topic) == 0)
        topicToUserStats.insert(std::make_pair(topic, unordered_map<string, vector<map<string, string>>>()));
    
    if (topicToUserStats[topic].count(user) == 0) {
        topicToUserStats[topic].insert(std::make_pair(user, vector<map<string, string>>()));
        topicToUserStats[topic][user].push_back(map<string, string>());
        topicToUserStats[topic][user].push_back(map<string, string>());
        topicToUserStats[topic][user].push_back(map<string, string>());
    }
    
    // if stat does not exist, create it
    // if stat exists - replace it
    topicToUserStats[topic][user][idx].insert(std::make_pair(statName, statVal));

}

void User::addEventDetails(string topic, string user, string eventTime, string eventName, string eventDesc)
{
    if (topicToUserEventDetails.count(topic) == 0)
        topicToUserEventDetails.insert(std::make_pair(topic, unordered_map<string, vector<vector<string>>>()));
    
    if (topicToUserEventDetails[topic].count(user) == 0)
        topicToUserEventDetails[topic].insert(std::make_pair(user, vector<vector<string>>()));
    
    vector<string> eventDetails;
    eventDetails.push_back(eventTime);
    eventDetails.push_back(eventName);
    eventDetails.push_back(eventDesc);

    topicToUserEventDetails[topic][user].push_back(eventDetails);
}

string User::getStats(string topic, string user, int idx)
{
    string output = "";
    
    if (topicToUserStats.count(topic) > 0 && topicToUserStats[topic].count(user) > 0)
        for (auto it = topicToUserStats[topic][user][idx].begin(); it != topicToUserStats[topic][user][idx].end(); it++) {
            output += "\n" + it->first + ":" + it->second;
    }

    output += "\n";

    return output;
}


StompConnectionHandler& User::getConnectionHandler(){
    return ch;
}
bool User::isLoggedIn()
{
    return loggedIn;
}

void User::logout()
{
    loggedIn = false;
}

void User::login()
{
    {
        std::lock_guard<std::mutex> lck(mtx);
        loggedIn = true;
    }
    cv.notify_one();
}

void User::parseMessage(string topic, string message)
{
    vector<string> result;
    stringstream ss(message);
    string token;
    string user;

    string eventName;
    string eventTime;
    string eventDesc;

    while (std::getline(ss, token, '\n')) {
        result.push_back(token);
    }

    for (int i = 0; i < result.size(); i++){
        string key = result[i].substr(0, result[i].find(":"));
        string val = result[i].substr(result[i].find(":") + 1, result[i].length() - result[i].find(":") - 1);

        if (key == "user")
            user = val;
        else if (key == "event name")
            eventName = val;
        else if (key == "time")
            eventTime = val;
        else if (key == "description")
            eventDesc = val;
        else if (key == "general game updates")
            while (i < result.size() && result[i][0] == '\t'){
                string statName = result[i].substr(1, result[i].find(":"));
                string statVal = result[i].substr(result[i].find(":") + 1, result[i].length() - result[i].find(":") - 1);
                addStat(topic, user, 0, statName, statVal);
                i++;
            }
        else if (key == "team a game updates")
            while (i < result.size() && result[i][0] == '\t'){
                string statName = result[i].substr(1, result[i].find(":"));
                string statVal = result[i].substr(result[i].find(":") + 1, result[i].length() - result[i].find(":") - 1);
                addStat(topic, user, 1, statName, statVal);
                i++;
            }
        else if (key == "team b game updates")
            while (i < result.size() && result[i][0] == '\t'){
                string statName = result[i].substr(1, result[i].find(":"));
                string statVal = result[i].substr(result[i].find(":") + 1, result[i].length() - result[i].find(":") - 1);
                addStat(topic, user, 2, statName, statVal);
                i++;
            }
    }

    addEventDetails(topic, user, eventTime, eventName, eventDesc);
}

string User::getUserName()
{
    return userName;
}

string User::getGeneralStats(string topic, string user)
{
    return getStats(topic, user, 0);
}

string User::getTeamAStats(string topic, string user)
{
    return getStats(topic, user, 1);
}

string User::getTeabBStats(string topic, string user)
{
    return getStats(topic, user, 2);
}

string User::getGameEventReports(string topic, string user)
{
    string output = "";
    if (topicToUserEventDetails.count(topic) > 0 && topicToUserEventDetails[topic].count(user) > 0){
        for (int i = 0; i < topicToUserEventDetails[topic][user].size(); i++){
            vector<string> eventDetails = topicToUserEventDetails[topic][user][i];
            output += "\n" + eventDetails[0] + " - " + eventDetails[1] + ":";
            output += "\n\n" + eventDetails[2] + "\n\n";
        }
    }
}
