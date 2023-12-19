#ifndef ClientHandler_hpp
#define ClientHandler_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include "clientinputhandler.hpp"
#include <thread>

class GameServer;

class ClientHandler {
public:
    ClientHandler(GameServer& server, int clientSocket);
    ~ClientHandler();
    void run();
    
private:
    ClientInputHandler inputHandler;
    GameServer& parentServer;
    int clientSocket;
    uint64_t sequenceNumber;
    char buffer[1024 + 1];
    std::mutex writeMutex;
    
    void closeSocket();
    
    void sendMessage(const char* message);
    int readMessage();
    
    bool sequenceNumberSynchronised();
    
    void proceedWithProtocol();
    void readProtocol();
    void writeProtocol();
    
};

#endif
