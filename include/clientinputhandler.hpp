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

class ClientInputHandler
{
public:
    ClientInputHandler();

    // Validate a client input string
    bool validateClientInput(const char *message);

    // checks if message is a read or a write.
    bool isRead(const char *message);

    // get various components of a valid input to the system.
    uint64_t getSeqNum();
    uint32_t getRowCoord();
    uint32_t getColCoord();
    std::string getDirection();
    std::string getStringToInsert();

private:
    // valid message divided into components.
    std::vector<std::string> cmpnts;
    std::vector<std::string> getStringComponents(const char *message);

    // validate if the inputs are in bounds even if they are syntactically valid
    bool validateReadInput(std::vector<std::string> components);
    bool validateWriteInput(std::vector<std::string> components);

    // checkers for types
    bool checkIsNumber(std::string str);
    bool checkIsDirection(std::string str);
    bool checkIsUINT32(std::string str);
    bool checkIsUINT64(std::string str);
};

#endif /* client_hpp */
