#include <iostream>
#include <thread>
#include <vector>
#include <unordered_map>
#include "../include/StompConnectionHandler.h"
#include "../include/User.h"
#include "../include/StompProtocol.h"

using std::vector;
using std::unordered_map;

class InputManager {
    private:
        User& user;
        StompProtocol& protocol;

    public:
        InputManager(User& _user, StompProtocol& _protocol);
        void run(string loginLine);
        unsigned int getFinalReceiptId();
};