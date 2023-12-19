#include "clienthandler.hpp"
#include "server.hpp"

ClientHandler::ClientHandler(GameServer& server, int clientSocket)
    : parentServer(server), clientSocket(clientSocket){
        memset(buffer, '\0', sizeof(buffer));
        
}

ClientHandler::~ClientHandler(){}

void ClientHandler::sendMessage(const char *message){
    // Send message to the client
    if (send(clientSocket, message, strlen(message), 0) == -1) perror("Error sending data");
}

int ClientHandler::readMessage() {
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
    if (sequenceNumber < parentServer.getCurrentSequenceNumber()){
        sendMessage("This sequence number has already passed for this game, need to wait for next game");
        return;
    }
    while(!sequenceNumberSynchronised()){}
    
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
    std::lock_guard<std::mutex> lock(writeMutex);
    SparseMatrix gridCopy = parentServer.getGrid();
    int matrixResponse = gridCopy.insertString(inputHandler.getRowCoord(),
                                               inputHandler.getColCoord(),
                                               inputHandler.getDirection(),
                                               inputHandler.getStringToInsert());
    parentServer.setGrid(gridCopy);
    if (matrixResponse == -1) sendMessage("Out of bounds for insert!");
    else sendMessage("Successful insert!");
}

void ClientHandler::closeSocket() {
    close(clientSocket);
}

void ClientHandler::run() {
    if (readMessage() == -1) return;
    proceedWithProtocol();
    std::cout << "closing connection having seqNum " << sequenceNumber <<", from thread id " << std::this_thread::get_id() << std::endl;
    closeSocket();
}
