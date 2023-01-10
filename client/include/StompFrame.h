#pragma once
#include <iostream>
#include <unordered_map>

using std::string;
using std::unordered_map;

class StompFrame{
    private:
        string command;
        unordered_map <string, string> headers;
        string body;
    public:
        StompFrame (string msg);
        StompFrame();
        StompFrame(string command,unordered_map <string, string> headers, string body);
        string getCommand();
        unordered_map <string, string> getHeaders();
        string getBody();
        string toString();
};