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
    std::cout << "Disconnecting..." << std::endl;
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

        StompProtocol* protocol = new StompProtocol(*user);

        InputManager* keyboard = new InputManager(*user, *protocol);
        std::thread KeyBoardReader(&InputManager::run, *keyboard, line);
        while (1) {
            // wait for message for the server
            std::string answer;
            if (!user->getConnectionHandler().getFrame(answer)) {
                // client error
                break;
            }
            std::cout << "RECEIVED MESSAGE FROM THE SERVER" << std::endl;
            std::cout << answer << std::endl;
            StompFrame s(answer);

            // process the message
            if (!protocol->process(s)){
                // server error
                break;
            }

            // user is not logged in, and got a receipt
            if (!user->isLoggedIn() && s.getHeaders().count("receipt-id") != 0)
                if (stoi(s.getHeaders().at("receipt-id")) == keyboard->getFinalReceiptId()){
                    break;
                }
        }    
        disconnect(user, protocol, keyboard); 
        std::cout << "Done receiving messages." << std::endl;
        KeyBoardReader.join();

    }
    return 0;
}
