#include "file.hpp"

pathInfo getPathAndName(std::string fileAndPath) {
    pathInfo file;
    size_t lastSlash = fileAndPath.find_last_of("/\\");
    size_t length = fileAndPath.length();
    
    file.filePath = fileAndPath.substr(0, lastSlash + 1);
    file.fileName = fileAndPath.substr(lastSlash + 1, length - lastSlash - 1);

    return file;
}