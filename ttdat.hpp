#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

#include "util/types.hpp"
#include "util/file.hpp"
#include "util/util.hpp"


#define INFO_AND 0x80000000
#define INFO_XOR 0xFFFFFFFF
#define INFO_SHIFT 8
#define INFO_ADD 0x100

struct fileData {
    char* crc;
    char* fileName;
    char* filePath;
    int offset;
    int fileSize;
};

class TTDat {
    private:
        std::ifstream datFile;

        std::ifstream hdrFile;

        std::string datFilePath;

        std::string datFileName;
        
        fileData* fileList;
        
        fileData* modFileList;

        loc infoLoc;

        int getLongInt(std::ifstream& /*file*/, int /*offset*/);

        int getLongIntBE(std::ifstream& /*file*/, int /*offset*/);

        int getInfoOffset();
        
        int getInfoSize();

        void decompressDat();
        
        void _openDatFile(std::string /*datFileName*/);

        void getFileInfo();

        bool checkCMP2();
        
        bool checkHdrFile();

        bool isNewFormat();
    
    public: 
        TTDat();

        TTDat(std::string, std::string);

        ~TTDat();

        void openDatFile(std::string, std::string);

        bool hasHdr() { return this->infoLoc;};

        u_int32_t fileCount;

        int fileNameCount;

        int fileNamesSize;

        int fileNamesOffset;

        int nameFieldSize;

        int nameInfoOffset;

        int infoType;

        int hdrSize;

        int crcsOffset;
        
        int infoOffset;

        int newInfoOffset;

        int infoSize;

        int newFormatVersion;

        bool newFormat;

        bool isCompressed;

        TTDatError errorState;

};