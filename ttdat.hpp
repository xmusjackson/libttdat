#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <filesystem>
#include <algorithm>

#include "util/types.hpp"
#include "util/file.hpp"

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
        
        fileData* datFileList;
        
        fileData* modFileList;

        infoLoc infoLocType;

        int getLongInt(std::ifstream& /*file*/, int /*offset*/);

        int getLongIntBE(std::ifstream& /*file*/, int /*offset*/);

        int getInfoOffset();
        
        int getInfoSize();

        void decompressDat();
        
        void _openDatFile(std::string /*datFileName*/);

        void getDatInfo();
        
        void getHdrInfo();

        bool checkCMP2();
        
        bool checkHdrFile();

        bool isNewFormat();

        std::string toUpper(std::string&);
    
    public: 
        TTDat();

        TTDat(std::string, std::string);

        ~TTDat();

        void openDatFile(std::string, std::string);

        bool hasHdr() { return this->infoLocType;};

        static char* longToStr(u_int32_t);

        int fileNamesOffset;

        u_int32_t fileCount;

        int infoType;

        int hdrSize;

        int crcsOffset;
        
        int infoOffset;

        int infoSize;

        bool newFormat;

        bool isCompressed;

        TTDatError errorState;

};