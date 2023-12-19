#include <iostream>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "server.hpp"

int main(int argc, char *argv[])
{
    GameServer server(5001, "/Users/shreyashonnalli/Desktop/flowtraders/output.txt");
    return 0;
}
