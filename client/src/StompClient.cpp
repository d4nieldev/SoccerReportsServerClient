#include <stdlib.h>
#include "../include/StompConnectionHandler.h"
#include <thread>
#include "../include/StompFrame.h"
#include "../include/InputManager.h"
#include <unordered_map>


using std::unordered_map;


int main(int argc, char *argv[]) {
	if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " host port" << std::endl << std::endl;
        return -1;
    }
    std::string host = argv[1];
    short port = atoi(argv[2]);
    bool shouldTerminate;
    
    while(1){
        StompConnectionHandler connectionHandler(host, port);
        if (!connectionHandler.connect()) {
            std::cerr << "Cannot connect to " << host << ":" << port << std::endl;
            return 1;
        }

        shouldTerminate = false;

        InputManager keyboard(connectionHandler, shouldTerminate);
        std::thread KeyBoardReader(&InputManager::run, &keyboard);

        int disconnectReceiptId;
        while (1) {
            std::string answer;
            if (!connectionHandler.getFrame(answer)) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }

            if (shouldTerminate)
                disconnectReceiptId = keyboard.getFinalReceiptId();

            StompFrame s(answer);
            if (s.getHeaders().find("receipt-id") != s.getHeaders().end())
                if (stoi(s.getHeaders().at("receipt-id")) == disconnectReceiptId)
                    break;
        }

        // wait for 
        std::string answer;
        if (!connectionHandler.getFrame(answer)) {
            std::cout << "Disconnected. Exiting...\n" << std::endl;
        }

    }
    return 0;
}
