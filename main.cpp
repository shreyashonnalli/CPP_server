#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "server.hpp"

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        std::cerr << "Usage: " << argv[0] << " <port>"
                  << " <fileLocation>" << std::endl;
        return -1;
    }

    int port = std::stoi(argv[1]);
    std::string fileLoc(argv[2]);

    // server runs forever
    GameServer server(port, fileLoc);
    return 0;
}
