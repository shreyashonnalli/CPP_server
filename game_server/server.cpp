#include "server.hpp"

GameServer::GameServer(int port, std::string writeLoc) : PORT(port), serverSocket(-1), writeLocation(writeLoc)
{
    currentSequenceNum = 1;
    setupServer();
    //    std::thread(&GameServer::asyncTest, this).detach();
    while (true)
    {
        std::cout << std::endl
                  << "Started new game" << std::endl;
        startThreadPool();
        startThreadPoolDestroyer();
    }
}

void GameServer::asyncTest()
{
    while (true)
    {
        std::cout << clientHandlerThreads.size() << " ";
        sleep(1);
    }
}

GameServer::~GameServer()
{
    startThreadPoolDestroyer();
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
    std::cout << "Writing to file" << std::endl
              << std::endl;

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

SparseMatrix GameServer::getGrid()
{
    return grid_;
}

void GameServer::setGrid(SparseMatrix grid)
{
    grid_ = grid;
}

uint64_t GameServer::getCurrentSequenceNumber()
{
    return currentSequenceNum;
}

void GameServer::updateCurrentSequenceNumber()
{
    currentSequenceNum++;
}

// blocking method.
// If there are active threads in the program, means the session isn't finished and waits indefinitely.
// If no active threads in the program, session might be finished, waits 20 secs before saying gaming session is finished.
int GameServer::getNextClientSocket()
{
    fd_set readSet;
    FD_ZERO(&readSet);
    FD_SET(serverSocket, &readSet);

    struct timeval timeout;
    timeout.tv_sec = 20;
    timeout.tv_usec = 0;

    if (clientHandlerThreads.size() == 0)
    {
        int result = select(serverSocket + 1, &readSet, nullptr, nullptr, &timeout);
        if (result == -1)
            std::exit(-1);
        else if (result == 0)
            return -1;
    }

    int clientSocket = accept(serverSocket, nullptr, nullptr);
    if (clientSocket == -1)
    {
        std::exit(-1);
    }
    return clientSocket;
}

// Join all active threads in case there are any
void GameServer::joinAllThreads()
{
    auto it = clientHandlerThreads.begin();
    while (it != clientHandlerThreads.end())
    {
        if (it->joinable())
        {
            it->join();
            it = clientHandlerThreads.erase(it);
            // Erase corresponding element in clientHandlers
            auto index = std::distance(clientHandlerThreads.begin(), it);
            clientHandlers.erase(clientHandlers.begin() + index);
        }
        else
        {
            ++it;
        }
    }
}

// Loops through all running threads and if its completed, then joins them.
void GameServer::joinOnlyFinishedThreads()
{
    for (int i = 0; i < clientHandlers.size(); i++)
    {
        if (!clientHandlers[i]->isCompleted())
        {
            std::cout << "A Thread is not completed yet. Keeping this in the pool." << std::endl;
        }
        else
        {
            std::cout << "A Thread is completed. Deleting this." << std::endl;
            if (clientHandlerThreads[i].joinable())
            {
                clientHandlerThreads[i].join();
                clientHandlerThreads.erase(clientHandlerThreads.begin() + i);
                clientHandlers.erase(clientHandlers.begin() + i);
                i--;
            }
        }
    }
    std::cout << "Number of threads in pool is now " << clientHandlers.size() << std::endl;
}

// Bit like a destructor but instead resets the entire server.
void GameServer::startThreadPoolDestroyer()
{
    if (clientHandlers.size() > 0)
        joinAllThreads();
    writeGridToFile();
    grid_.emptySparseMatrix();
    currentSequenceNum = 1;
    clientHandlerThreads.clear();
    clientHandlers.clear();
}

void GameServer::startThreadPool()
{
    int clientSocket;
    while ((clientSocket = getNextClientSocket()) != -1)
    {
        if (clientHandlerThreads.size() < MAX_CONNECTIONS)
        {
            auto clientHandler = std::make_unique<ClientHandler>(*this, clientSocket);
            clientHandlers.push_back(std::move(clientHandler));
            clientHandlerThreads.emplace_back(&ClientHandler::run, clientHandlers.back().get());
        }
        joinOnlyFinishedThreads();
    }

    std::cout << std::endl
              << "No players joined after 20 seconds... session has finished!" << std::endl;
}
