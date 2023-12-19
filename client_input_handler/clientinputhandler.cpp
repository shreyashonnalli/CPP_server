#include "clientinputhandler.hpp"

ClientInputHandler::ClientInputHandler() {}

bool ClientInputHandler::validateClientInput(const char* message){
    std::vector<std::string> components = getStringComponents(message);
    cmpnts = components;
    if (components.size() != 6 && components.size() != 7) return false;
    if (components.size() == 6) return validateReadInput(components);
    else return validateWriteInput(components);
}

std::vector<std::string> ClientInputHandler::getStringComponents(const char* message){
    std::string inputString = message;
    std::stringstream ss(inputString);
    std::vector<std::string> components;
    std::string token;
    while(std::getline(ss, token, ',')){
        components.push_back(token);
    }
    return components;
}

bool ClientInputHandler::validateReadInput(std::vector<std::string> components){
    for(auto component: components) if (component.length() == 0) return false;
    if (components[0] != "READ") return false;
    if (!checkIsUINT64(components[1])) return false;
    if (!checkIsUINT32(components[2])) return false;
    if (!checkIsUINT32(components[3])) return false;
    if (!checkIsDirection(components[4])) return false;
    if (components[5] != "END") return false;
    return true;
}

bool ClientInputHandler::validateWriteInput(std::vector<std::string> components){
    for(auto component: components) if (component.length() == 0) return false;
    if (components[0] != "WRITE") return false;
    if (!checkIsUINT64(components[1])) return false;
    if (!checkIsUINT32(components[2])) return false;
    if (!checkIsUINT32(components[3])) return false;
    if (!checkIsDirection(components[4])) return false;
    if (components[5].size() == 0) return false;
    if (components[6] != "END") return false;
    return true;
}

bool ClientInputHandler::checkIsUINT32(std::string str){
    if (!checkIsNumber(str)) return false;
    try {
        size_t pos;
        u_long value = std::stoul(str, &pos);
        return pos == str.length() && value < std::numeric_limits<uint32_t>::max();
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
    return false;
}

bool ClientInputHandler::checkIsUINT64(std::string str){
    if (!checkIsNumber(str)) return false;
    try {
        size_t pos;
        u_long value = std::stoul(str, &pos);
        return pos == str.length() && value < std::numeric_limits<uint64_t>::max() && value > 0;
    } catch (const std::invalid_argument& e) {
        return false;
    } catch (const std::out_of_range& e) {
        return false;
    }
    return false;
}

bool ClientInputHandler::checkIsNumber(std::string str){
    for(int i=0;i<str.length();i++){
        if (!isdigit(str[i])) return false;
    }
    return true;
}

bool ClientInputHandler::checkIsDirection(std::string str){
    if (str != "+X" && str != "-X" && str != "+Y" && str != "-Y") return false;
    else return true;
}


//assumes valid input
bool ClientInputHandler::isRead(const char *message){
    std::vector<std::string> components = getStringComponents(message);
    if (components[0] == "READ") return true;
    else return false;
}

uint64_t ClientInputHandler::getSeqNum(){
    return std::stoull(cmpnts[1]);
}

uint32_t ClientInputHandler::getRowCoord(){
    return std::stoul(cmpnts[2]);
}

uint32_t ClientInputHandler::getColCoord(){
    return std::stoul(cmpnts[3]);
}

std::string ClientInputHandler::getDirection(){
    return cmpnts[4];
}

//used only after valid and in write function
std::string ClientInputHandler::getStringToInsert(){
    if (!validateWriteInput(cmpnts)) return "";
    return cmpnts[5];
}
