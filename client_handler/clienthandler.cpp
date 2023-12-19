#include "clienthandler.hpp"
#include "server.hpp"

ClientHandler::ClientHandler(GameServer& server, int clientSocket)
    : parentServer(server), clientSocket(clientSocket){
//        struct timeval tv;
//        tv.tv_sec = 1;
//        tv.tv_usec = 0;
//        setsockopt(clientSocket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);
        memset(buffer, '\0', sizeof(buffer));
}

ClientHandler::~ClientHandler(){}

void ClientHandler::sendMessage(const char *message){
    // Send message to the client
    if (send(clientSocket, message, strlen(message), 0) == -1) perror("Error sending data");
}

int ClientHandler::readMessage() {
    
    struct pollfd fd;
    int ret;
    fd.fd = clientSocket;
    fd.events = POLLIN;
    if (poll(&fd, 1, 1000) <= 0) return -1;
    
    memset(buffer, '\0', sizeof(buffer));
    read(clientSocket, buffer, sizeof(buffer) - 1);
    if (inputHandler.validateClientInput(buffer) == false)
    {
        sendMessage("Invalid Input");
        return -1;
    }
    return 1;
}

bool ClientHandler::sequenceNumberSynchronised(){
    return sequenceNumber == parentServer.getCurrentSequenceNumber();
}

void ClientHandler::proceedWithProtocol() {
    sequenceNumber = inputHandler.getSeqNum();
    //wait for previous sequences to finish incase this has arrived earlier
    while(!sequenceNumberSynchronised()){}
    std::cout << parentServer.getCurrentSequenceNumber() << "\n";
    if (inputHandler.isRead(buffer)) readProtocol();
    else writeProtocol();
    parentServer.updateCurrentSequenceNumber();
}

void ClientHandler::readProtocol() {
    std::string matrixResponse = parentServer.getGrid().readFromGrid(inputHandler.getRowCoord(),
                                            inputHandler.getColCoord(),
                                            inputHandler.getDirection());
    sendMessage(matrixResponse.c_str());
}

void ClientHandler::writeProtocol() {
    parentServer.writeMutex.lock();
    SparseMatrix gridCopy = parentServer.getGrid();
    int matrixResponse = gridCopy.insertString(inputHandler.getRowCoord(),
                                               inputHandler.getColCoord(),
                                               inputHandler.getDirection(),
                                               inputHandler.getStringToInsert());
    parentServer.setGrid(gridCopy);
    parentServer.writeMutex.unlock();
    if (matrixResponse == -1) sendMessage("Out of bounds for insert!");
    else sendMessage("Successful insert!");
}

void ClientHandler::closeSocket() {
    close(clientSocket);
}

void ClientHandler::run() {
    while(readMessage() != -1) proceedWithProtocol();
//    if (readMessage() == -1) return;
//    proceedWithProtocol();
    std::cout << "closing connection having seqNum " << sequenceNumber <<", from thread id " << std::this_thread::get_id() << std::endl;
    closeSocket();
}
