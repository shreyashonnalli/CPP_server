//
//  client.cpp
//  game-server
//
//  Created by Shreyas Honnalli on 18/12/2023.
//

#include "client.hpp"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <seqNum>"
                  << " <seqNum>" << std::endl;
        return 1;
    }
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[1024];

    int seqNum = std::stoi(argv[1]);
    int seqNum2 = std::stoi(argv[2]);

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Error creating socket");
        return 1;
    }

    // Set up server address structure
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(5001);
    if (inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr) <= 0)
    {
        perror("Error converting IP address");
        close(clientSocket);
        return 1;
    }

    // Connect to the server
    int status = connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (status == -1)
    {
        perror("Error connecting to server");
        close(clientSocket);
        return 1;
    }

    std::cout << "Connected to the server." << std::endl;

    std::string message = "WRITE," + std::to_string(seqNum) + ",0,0,+Y,annnnnnnnnnn,END";
    send(clientSocket, message.c_str(), message.length(), 0);

    memset(buffer, '\0', sizeof(buffer));
    read(clientSocket, buffer, sizeof(buffer) - 1);
    std::cout << "From Server: " << buffer << std::endl;

    std::string message2 = "WRITE," + std::to_string(seqNum2) + ",0,0,+Y,annnnnnnnnnn,END";
    send(clientSocket, message2.c_str(), message2.length(), 0);

    memset(buffer, '\0', sizeof(buffer));
    read(clientSocket, buffer, sizeof(buffer) - 1);
    std::cout << "From Server: " << buffer << std::endl;
    // Close the socket
    close(clientSocket);

    return 0;
}
