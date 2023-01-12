#include <stdlib.h>
#include "../include/StompConnectionHandler.h"
#include <thread>
#include "../include/StompFrame.h"
#include "../include/InputManager.h"
#include <unordered_map>
#include <sstream>


using std::unordered_map;
using std::stringstream;
using std::string;


void disconnect(User* user, StompProtocol* protocol, InputManager* keyboard) {
    user->logout();
    std::cout << "An error occured. Exiting...\n" << std::endl;
    delete user;
    delete protocol;
    delete keyboard;
}

int main(int argc, char *argv[]) {
    string host;
    int port;
    string line;
    string name;
    
    while (1) {
        while (1){
            const short bufsize = 1024;
            char buf[bufsize];
            std::cin.getline(buf, bufsize); //blocked 
            line = buf;            

            if (line.substr(0, 5) == "login") {
                //split the command
                std::vector<std::string> result;
                std::stringstream ss(line);
                std::string token;

                while (std::getline(ss, token, ' ')) {
                    result.push_back(token);
                }
                
                host = result[1].substr(0,result[1].find(":"));

                port = stoi(result[1].substr(result[1].find(":") + 1, 4));
                name=result[2];

                break;
            }
            else{
                std::cout << "You have to login before doing any other operations" << std::endl;
            }
        }

        User* user = new User(host, port, name);
        bool doneReceivingMessages = false;

        StompProtocol* protocol = new StompProtocol(*user);

        InputManager* keyboard = new InputManager(*user, *protocol);
        std::thread KeyBoardReader(&InputManager::run, *keyboard, line);

        int disconnectReceiptId = -1;
        while (!doneReceivingMessages) {
            if (!user->isLoggedIn() && disconnectReceiptId == -1)
                disconnectReceiptId = keyboard->getFinalReceiptId();

            std::string answer;
            if (!user->getConnectionHandler().getFrame(answer)) {
                // client error
                disconnect(user, protocol, keyboard);
                break;
            }

            StompFrame s(answer);
            std::cout << s.toString() << std::endl;

            // process the message
            if (!protocol->process(s))
                // server error
                disconnect(user, protocol, keyboard);
                break;

            if (disconnectReceiptId != -1 && s.getHeaders().count("receipt-id") != 0)
                if (stoi(s.getHeaders().at("receipt-id")) == disconnectReceiptId)
                    doneReceivingMessages = true;  
        }     

        KeyBoardReader.join();

    }
    return 0;
}
