#include <stdlib.h>
#include "../include/StompConnectionHandler.h"
#include <thread>
#include "../include/StompFrame.h"
#include "../include/InputManager.h"
#include <unordered_map>


using std::unordered_map;


int main(int argc, char *argv[]) {
    string host;
    int port;
    string line;
    
    while (1) {
        while (1){
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); //blocked 
            line = buf;

            if (line.substr(0, 5) == "login") {
                string host = line.substr(line.find(" ") + 1, line.find(":") - line.find(" ") - 1);
                int port = stoi(line.substr(line.find(":") + 1, 4));
                break;
            }
            else{
                std::cout << "You have to login before doing any other operations";
            }
        }

        User user(host, port);
        bool doneReceivingMessages = false;

        StompProtocol protocol(user);

        InputManager keyboard(user, protocol);
        std::thread KeyBoardReader(&InputManager::run, &keyboard, line);

        int disconnectReceiptId = -1;
        while (!doneReceivingMessages) {
            if (!user.isLoggedIn() && disconnectReceiptId == -1)
                disconnectReceiptId = keyboard.getFinalReceiptId();

            std::string answer;
            if (!user.getConnectionHandler().getFrame(answer)) {
                printDisconnectMsg();
                break;
            }

            StompFrame s(answer);
            std::cout << s.toString() << std::endl;

            // process the message
            if (!protocol.process(s))
                // error occured, disconnect client
                printDisconnectMsg();
                break;

            if (disconnectReceiptId != -1 && s.getHeaders().count("receipt-id") != 0)
                if (stoi(s.getHeaders().at("receipt-id")) == disconnectReceiptId)
                    doneReceivingMessages = true;  
        }     

    }
    return 0;
}

void printDisconnectMsg() {
    std::cout << "An error occured. Exiting...\n" << std::endl;
}
