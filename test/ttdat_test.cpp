#include "../ttdat.hpp"

int main (int argc, char** argv) {
    pathInfo file;
    if (argc > 1) {
        file = getPathAndName(argv[1]);
    } else {
        return 1;
    }

    TTDat datFile(file.filePath, file.fileName);

    std::cout << "Info Offset: " <<  datFile.infoOffset <<  std::endl;
    std::cout << "New Info Offset: " <<  datFile.newInfoOffset <<  std::endl;
    std::cout << "Info Size: " << datFile.infoSize << std::endl;
    std::cout << "File Count: " << datFile.fileCount << std::endl;
    std::cout << "Info Type: " << datFile.infoType << std::endl;
    std::cout << "New Format?: " << (datFile.newFormat ? "True" : "False") << std::endl;
    if (datFile.newFormat){ std::cout << "New Format Version: " << datFile.newFormatVersion << std::endl; }
    std::cout << "Has HDR?: " << (datFile.hasHdr() ? "True" : "False") << std::endl;
    std::cout << "Is Compressed?: " << (datFile.isCompressed ? "True" : "False") << std::endl;
    std::cout << "File Name Count: " << datFile.fileNameCount << std::endl;
    std::cout << "File Names Offset: " << datFile.fileNamesOffset << std::endl;
    std::cout << "File Names Size: " << datFile.fileNamesSize << std::endl;
    std::cout << "CRCs Offset: " << datFile.crcsOffset << std::endl;
    std::cout << "Name Info Offset: " << datFile.nameInfoOffset << std::endl;

    return 0;
}