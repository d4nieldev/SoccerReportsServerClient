CFLAGS:=-c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS:=-lboost_system -lpthread

all: StompWCIClient

EchoClient: bin/ConnectionHandler.o bin/echoClient.o
	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

StompWCIClient: bin/StompConnectionHandler.o bin/StompClient.o bin/event.o bin/StompFrame.o bin/InputManager.o
	g++ -o bin/StompWCIClient bin/StompConnectionHandler.o bin/StompClient.o bin/event.o bin/StompFrame.o bin/InputManager.o $(LDFLAGS)

bin/ConnectionHandler.o: src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

bin/echoClient.o: src/echoClient.cpp
	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

bin/event.o: src/event.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp

bin/StompClient.o: src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

bin/StompConnectionHandler.o: src/StompConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/StompConnectionHandler.o src/StompConnectionHandler.cpp

bin/StompFrame.o: src/StompFrame.cpp
	g++ $(CFLAGS) -o bin/StompFrame.o src/StompFrame.cpp
	
bin/InputManager.o: src/InputManager.cpp
	g++ $(CFLAGS) -o bin/InputManager.o src/InputManager.cpp


.PHONY: clean
clean:
	rm -f bin/*
	