//
//  server.hpp
//  game-server
//
//  Created by Shreyas Honnalli on 18/12/2023.
//

#ifndef server_hpp
#define server_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <vector>
#include <matrix.hpp>
#include <fstream>
#include <filesystem>
#include "clientinputhandler.hpp"
#include "clienthandler.hpp"
#include <thread>

constexpr uint64_t MAX_CONNECTIONS = std::numeric_limits<uint64_t>::max();

class GameServer
{
public:
    // needed so multiple clientHandlers cant manipulate the grid at the same time.
    std::mutex writeMutex;

    GameServer(int port, std::string writeLoc);
    ~GameServer();

    // public method used by other classes(Mainly clientHandler children)
    void writeGridToFile();
    SparseMatrix getGrid();
    void setGrid(SparseMatrix grid);
    void updateCurrentSequenceNumber();
    uint64_t getCurrentSequenceNumber();

private:
    // server related info
    const int PORT;
    int serverSocket;
    struct sockaddr_in serverAddr;

    // thread pool of clientHandlers
    std::vector<std::thread> clientHandlerThreads;
    std::vector<std::unique_ptr<ClientHandler>> clientHandlers;

    // Location to write file, the data structure and the currentSequenceNumber
    std::string writeLocation;
    SparseMatrix grid_;
    uint64_t currentSequenceNum;

    // Needed while testing
    void asyncTest();

    // server initialization methods
    void createSocket();
    void setupServerAddress();
    void bindSocketToServerAddress();
    void listenToIncomingConnections();
    void setupServer();

    // blocking method
    int getNextClientSocket();

    // functions related to the thread pool
    void startThreadPool();
    void joinAllThreads();
    void joinOnlyFinishedThreads();
    void startThreadPoolDestroyer();
};

#endif /* server_hpp */
