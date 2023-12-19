#ifndef ClientInputHandler_hpp
#define ClientInputHandler_hpp

#include <stdio.h>
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string>
#include <sstream>
#include <future>

class ClientInputHandler {
public:
    ClientInputHandler();

    // Validate a client input string
    bool validateClientInput(const char* message);
    bool isRead(const char* message);
    uint64_t getSeqNum();
    uint32_t getRowCoord();
    uint32_t getColCoord();
    std::string getDirection();
    std::string getStringToInsert();
private:
    std::vector<std::string> cmpnts;
    
    std::vector<std::string> getStringComponents(const char* message);
    bool validateReadInput(std::vector<std::string> components);
    bool validateWriteInput(std::vector<std::string> components);
    
    bool checkIsNumber(std::string str);
    bool checkIsDirection(std::string str);
    bool checkIsUINT32(std::string str);
    bool checkIsUINT64(std::string str);
};

#endif /* client_hpp */
