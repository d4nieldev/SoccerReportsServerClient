#include "../include/User.h"
#include "User.h"

using std::pair;

User::User(string host, int port) : loggedIn(false), ch(host, port) {
    if (!ch.connect()) 
        throw std::runtime_error("Cannot connect to " + host + ":" + std::to_string(port));
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

void User::addTopicUserMessage(string topic, string user, string message)
{
    // check if the topic is not present and add it if so
    if (topicToUserToMessages.count(topic) == 0)
        topicToUserToMessages.insert(pair<string, unordered_map<string, vector<string>>>(topic, unordered_map<string, vector<string>>()));

    // check if the user is not present and add it if so
    if (topicToUserToMessages.at(topic).count(user) == 0)
        topicToUserToMessages.at(topic).insert(pair<string, vector<string>>(user, vector<string>()));
    
    // add the message to the topic->user
    topicToUserToMessages.at(topic).at(user).push_back(message);
}

vector<string> User::getTopicUserMessages(string topic, string user)
{
    if (topicToUserToMessages.count(topic) == 0)
        throw std::invalid_argument("map does not contain the topic: " + topic + ".");

    if (topicToUserToMessages.at(topic).count(user) == 0)
        throw std::invalid_argument("map at topic: " + topic + " does not contain the user: " + user + ".");

    return topicToUserToMessages.at(topic).at(user);
}
