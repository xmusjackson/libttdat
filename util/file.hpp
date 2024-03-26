#include <iostream>
#include <string>

struct pathInfo {
    std::string filePath;
    std::string fileName;
};

pathInfo getPathAndName(std::string);
