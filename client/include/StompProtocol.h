#pragma once

#include "../include/User.h"
#include "../include/StompFrame.h"
#include <vector>

class StompProtocol
{
    private:
        User& user;
        unordered_map<string, unsigned int> topicToSubId;
        unsigned int nextReceiptId;
        unsigned int nextSubscriptionId;
    public:
        StompProtocol(User& user);
        unsigned int getLastReceiptId();
        /**
         * process a command from the user and return the stomp frame as string
         * @param words tokens of the line the user wrote
         * @return list of messages to be sent
        */
        vector<string> process(vector<string> words);

        /**
         * process a frame from the server and do something
         * @param frame the frame received from the server
         * @return true iff the frame is not an error frame
        */
        bool process(StompFrame frame);
};
