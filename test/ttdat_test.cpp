#include "../ttdat.hpp"

int main (int argc, char** argv) {
    pathInfo file;
    if (argc > 1) {
        file = getPathAndName(argv[1]);
    } else {
        return 1;
    }

    TTDat datFile(file.filePath, file.fileName);

    std::cout << "Info Offset: " << datFile.infoOffset << std::endl;
    std::cout << "Info Size: " << datFile.infoSize << std::endl;
    std::cout << "File Count: " << datFile.fileCount << std::endl;
    std::cout << "Info Type: " << datFile.infoType << std::endl;
    std::cout << "New Format?: " << (datFile.newFormat ? "True" : "False") << std::endl;
    std::cout << "Has HDR?: " << (datFile.hasHdr() ? "True" : "False") << std::endl;

    return 0;
}