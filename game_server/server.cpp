#include "server.hpp"

GameServer::GameServer(int port, std::string writeLoc) : PORT(port), serverSocket(-1), writeLocation(writeLoc)
{
    currentSequenceNum = 1;
    setupServer();
    std::thread(&GameServer::asyncTest, this).detach();
    startThreadPool();
    startThreadPoolDestroyer();
}

void GameServer::asyncTest(){
    while(true){
        std::cout << clientHandlerThreads.size() << " ";
        sleep(1);
    }
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

void GameServer::joinAllThreads(){
    for (auto& thread : clientHandlerThreads) {
        if (thread.joinable()) thread.join();
    }
}

void GameServer::joinOnlyFinishedThreads(){
    for (int i = 0; i < clientHandlers.size(); i++) {
        if (!clientHandlers[i]->isCompleted()) {
            std::cout << "Thread " << i << " is not completed yet. Continue with execution." << std::endl;
        }
        else {
            std::cout << "Thread " << i << " is completed. Deleting this..." << std::endl;
            if (clientHandlerThreads[i].joinable()){
                clientHandlerThreads[i].join();
                clientHandlerThreads.erase(clientHandlerThreads.begin() + i);
                clientHandlers.erase(clientHandlers.begin() + i);
                i--;
            }
        }
    }
}

void GameServer::startThreadPoolDestroyer() {
    joinAllThreads();
    writeGridToFile();
    grid_.emptySparseMatrix();
    currentSequenceNum = 1;
    clientHandlerThreads.clear();
}

void GameServer::startThreadPool() {
    while(true){
        int clientSocket = getNextClientSocket();
        if (clientHandlerThreads.size() < MAX_CONNECTIONS){
            auto clientHandler = std::make_unique<ClientHandler>(*this, clientSocket);
            
            clientHandlers.push_back(std::move(clientHandler));
            clientHandlerThreads.emplace_back(&ClientHandler::run, clientHandlers.back().get());
        }
        joinOnlyFinishedThreads();
        grid_.printMatrix();
    }
}

bool GameServer::allThreadsJoinable() {
    for (auto& thread : clientHandlerThreads) {
        if (!thread.joinable()) return false;
    }
    if (clientHandlerThreads.size() < 1) return false;
    else return true;
}


