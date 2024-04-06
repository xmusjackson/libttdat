#include "libttdat/ttdat.hpp"

struct pathAndName {
    std::string filePath;
    std::string fileName;
};

pathAndName getPathAndName(std::string filename) {
    pathAndName file;
    size_t lastSlash = filename.find_last_of("/\\");
    size_t length = filename.length();
    
    file.filePath = filename.substr(0, lastSlash + 1);
    file.fileName = filename.substr(lastSlash + 1, length - lastSlash - 1);

    return file;
}

int main (int argc, char** argv) {
    pathAndName file;
    if (argc > 1) {
        file = getPathAndName(argv[1]);
    } else {
        std::cout << "Provide a DAT file as an argument." << std::endl;
        return 1;
    }

    TTDat datFile(file.filePath, file.fileName);

    std::cout << "Info Offset: " <<  datFile.getHdrOffset() <<  std::endl;
    std::cout << "File Names Offset: " << datFile.getFileNamesOffset() << std::endl;
    std::cout << "File Offsets Offset: " << datFile.getFileOffsOffset() << std::endl;
    std::cout << "Name Info Offset: " << datFile.getNameInfoOffset() << std::endl;
    std::cout << "CRCs Offset: " << datFile.getCrcsOffset() << std::endl;    
    std::cout << "Info Size: " << datFile.getInfoSize() << std::endl;
    std::cout << "File Names Size: " << datFile.getFileNamesSize() << std::endl;
    std::cout << "File Count: " << datFile.getFileCount() << std::endl;
    std::cout << "File Name Count: " << datFile.getFileNameCount() << std::endl;
    std::cout << "Info Type: " << datFile.getInfoType() << std::endl;
    std::cout << "New Format?: " << (datFile.getNewFormat() ? "True" : "False") << std::endl;
    if (datFile.getNewFormat()){ std::cout << "New Format Version: " << datFile.getNewFormatVersion() << std::endl; }
    std::cout << "Has HDR?: " << (datFile.hasHdr() ? "True" : "False") << std::endl;
    std::cout << "Is Compressed?: " << (datFile.isCompressed() ? "True" : "False") << std::endl;
    std::cout << "CRC Size: " << (datFile.isCrc64() ? "64" : "32") << std::endl;

    return 0;
}
