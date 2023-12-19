#include "server.hpp"

GameServer::GameServer(int port, std::string writeLoc) : PORT(port), serverSocket(-1), writeLocation(writeLoc)
{
    currentSequenceNum = 1;
    setupServer();
    startThreadPool();
}

GameServer::~GameServer()
{
    close(serverSocket);
    for(auto& thread: clientHandlerThreads) {
        thread.join();
    }
}

void GameServer::createSocket()
{
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Error creating socket");
        std::exit(EXIT_FAILURE);
    }
}

// Set up server address structure
void GameServer::setupServerAddress()
{
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
}

// Bind socket to address
void GameServer::bindSocketToServerAddress()
{
    int bindStatus = bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (bindStatus == -1)
    {
        perror("Error binding socket");
        close(serverSocket);
        std::exit(EXIT_FAILURE);
    }
}

void GameServer::listenToIncomingConnections()
{
    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1)
    {
        perror("Error listening for connections");
        close(serverSocket);
        std::exit(EXIT_FAILURE);
    }
}

void GameServer::setupServer()
{
    createSocket();
    setupServerAddress();
    bindSocketToServerAddress();
    listenToIncomingConnections();
    std::cout << "Server listening on port " << PORT << std::endl;
}

void GameServer::writeGridToFile()
{
    std::ofstream outputFile(writeLocation, std::ios::out | std::ios::app);

    if (outputFile.is_open())
    {
        outputFile << grid_;
        outputFile.close();
    }
    else
    {
        std::cerr << "Error: Unable to open the file." << std::endl;
    }
}

SparseMatrix GameServer::getGrid(){
    return grid_;
}

void GameServer::setGrid(SparseMatrix grid){
    grid_ = grid;
}


uint64_t GameServer::getCurrentSequenceNumber(){
    return currentSequenceNum;
}


void GameServer::updateCurrentSequenceNumber(){
    currentSequenceNum++;
}


int GameServer::getNextClientSocket(){
    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1) {
        std::exit(-1);
    }
    return clientSocket;
}


void GameServer::startThreadPool() {
    while(true){
        int clientSocket = getNextClientSocket();
        ClientHandler clientHandler(*this, clientSocket);
        if (clientHandlerThreads.size() < MAX_CONNECTIONS){
//            std::cout << clientHandlerThreads.size() << "\n";
            clientHandlerThreads.emplace_back(&ClientHandler::run, &clientHandler);
        }
        else{
            std::cerr << "Thread Pool is full. Connection Rejected." << std::endl;
            break;
        }
    }
}


