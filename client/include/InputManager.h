#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include "../include/StompConnectionHandler.h"

using std::vector;
using std::unordered_map;

class InputManager {
    private:
        StompConnectionHandler& ch;
        unordered_map<string,int> topicToSubId;
        bool shouldTerminate;
        bool isLoggedIn;
        int nextReceiptId;
        int nextSubscriptionId;

    public:
        InputManager(StompConnectionHandler& _ch, bool _shouldTerminate);
        void run();
        int getFinalReceiptId();
        string getFrameFromLine(vector<string> words);
};