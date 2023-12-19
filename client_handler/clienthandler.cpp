#include "clienthandler.hpp"
#include "server.hpp"

ClientHandler::ClientHandler(GameServer &server, int clientSocket)
    : parentServer(server), clientSocket(clientSocket)
{
}

ClientHandler::~ClientHandler() {}

void ClientHandler::resetBuffer()
{
    memset(buffer, '\0', sizeof(buffer));
}

// wrapper to simplify sending a message to socket.
void ClientHandler::sendMessage(const char *message)
{
    if (send(clientSocket, message, strlen(message), 0) == -1)
        perror("Error sending data");
}

// polling socket if it has more requests.
bool ClientHandler::pollSocket()
{
    struct pollfd fd;
    int ret;
    fd.fd = clientSocket;
    fd.events = POLLIN;
    if (poll(&fd, 1, 1000) <= 0)
        return false;
    else
        return true;
}

// wrapper for reading a message from socket.
int ClientHandler::readMessage()
{
    if (!pollSocket())
        return -1;
    resetBuffer();
    read(clientSocket, buffer, sizeof(buffer) - 1);
    if (inputHandler.validateClientInput(buffer) == false)
    {
        sendMessage("Invalid Input");
        return -1;
    }
    return 1;
}

// finds out if the current thread is next in line for execution.
bool ClientHandler::sequenceNumberSynchronised()
{
    return sequenceNumber == parentServer.getCurrentSequenceNumber();
}

// protocol of communication if inputs are valid.
void ClientHandler::proceedWithProtocol()
{
    sequenceNumber = inputHandler.getSeqNum();

    // wait for previous sequences (other threads) to finish as this thread has arrived earlier
    while (!sequenceNumberSynchronised())
    {
    }

    // proceed to read or write and then update sequenceNumber.
    if (inputHandler.isRead(buffer))
        readProtocol();
    else
        writeProtocol();
    parentServer.writeMutex.lock();
    parentServer.updateCurrentSequenceNumber();
    parentServer.writeMutex.unlock();
}

// protocol to read.
void ClientHandler::readProtocol()
{
    std::string matrixResponse = parentServer.getGrid().readFromGrid(inputHandler.getRowCoord(),
                                                                     inputHandler.getColCoord(),
                                                                     inputHandler.getDirection());
    sendMessage(matrixResponse.c_str());
}

// protocol to write.
void ClientHandler::writeProtocol()
{
    parentServer.writeMutex.lock();
    SparseMatrix gridCopy = parentServer.getGrid();
    int matrixResponse = gridCopy.insertString(inputHandler.getRowCoord(),
                                               inputHandler.getColCoord(),
                                               inputHandler.getDirection(),
                                               inputHandler.getStringToInsert());
    parentServer.setGrid(gridCopy);
    parentServer.writeMutex.unlock();
    if (matrixResponse == -1)
        sendMessage("Out of bounds for insert!");
    else
        sendMessage("Successful insert!");
}

void ClientHandler::closeSocket()
{
    close(clientSocket);
}

// called in an async thread.
void ClientHandler::run()
{
    while (readMessage() != -1)
        proceedWithProtocol();
    closeSocket();

    promise.set_value();
}

// Checks if the thread is completed and ready to be destroyed.
bool ClientHandler::isCompleted()
{
    return future.wait_for(std::chrono::milliseconds(100)) == std::future_status::ready;
}
