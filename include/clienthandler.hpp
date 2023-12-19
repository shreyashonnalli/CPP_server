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
#include <poll.h>

class GameServer;

class ClientHandler
{
public:
    ClientHandler(GameServer &server, int clientSocket);
    ~ClientHandler();

    // run for async thread to run, isCompleted is for server to deduce if joinable.
    void run();
    bool isCompleted();

private:
    // input handler to handle and error check inputs
    ClientInputHandler inputHandler;

    // parent server
    GameServer &parentServer;

    // current sequence number of client
    // buffer to take input from client
    // sequence number of client input
    int clientSocket;
    char buffer[1024 + 1];
    uint64_t sequenceNumber;

    // needed to deduce if joinable
    std::promise<void> promise;
    std::future<void> future{promise.get_future()};

    // methods to help with sockets
    bool pollSocket();
    void resetBuffer();
    void closeSocket();

    // sending and recieving messages to and from socket and ensuring in sync
    void sendMessage(const char *message);
    int readMessage();
    bool sequenceNumberSynchronised();

    // protocol related communication
    void proceedWithProtocol();
    void readProtocol();
    void writeProtocol();
};

#endif
