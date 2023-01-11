#pragma once
#include "../include/StompConnectionHandler.h"
#include "../include/StompProtocol.h"
#include <unordered_map>
#include <vector>
#include <mutex>
#include <condition_variable>

using std::string;
using std::unordered_map;
using std::vector;

class User
{
private:
    bool loggedIn;
    StompConnectionHandler ch;
    unordered_map<string, unordered_map<string, vector<string>>> topicToUserToMessages;

public:
    std::mutex mtx;
    std::condition_variable cv;

    User(string host, int port);
    StompConnectionHandler& getConnectionHandler();
    bool isLoggedIn();
    void logout();
    void login();
    void addTopicUserMessage(string topic, string user, string message);
    vector<string> getTopicUserMessages(string topic, string user);
};