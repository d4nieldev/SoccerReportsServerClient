#pragma once
#include "../include/StompConnectionHandler.h"
#include <map>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <unordered_map>

using std::map;
using std::pair;
using std::string;
using std::unordered_map;
using std::vector;

class User
{
private:
    bool loggedIn;
    StompConnectionHandler* ch;
    string userName;
    // maps topic to user to general, team a, and team b stats, saves stats names in lexicographic order
    unordered_map<string, unordered_map<string, vector<map<string, string>>>> topicToUserStats;
    // maps topic to user to event details (time, name, and description)
    unordered_map<string, unordered_map<string, vector<vector<string>>>> topicToUserEventDetails;

    void addStat(string topic, string user, int idx, string statName, string statVal);

    void addEventDetails(string topic, string user, string eventTime, string eventName, string eventDesc);

    string getStats(string topic, string user, int idx);

public:
    std::mutex mtx;
    std::condition_variable cv;

    User(string host, int port, string userName);
    ~User();
    StompConnectionHandler &getConnectionHandler();
    bool isLoggedIn();
    void logout();
    void login();
    /**
     * parsing the stats and event details from the message
     * @param topic   the topic name
     * @param message body of STOMP MESSAGE from server to parse
     */
    void parseMessage(string topic, string message);
    string getUserName();

    string getGeneralStats(string topic, string user);
    string getTeamAStats(string topic, string user);
    string getTeabBStats(string topic, string user);
    string getGameEventReports(string topic, string user);
};