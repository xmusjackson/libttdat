#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <map>

#include "types.hpp"


#define INFO_AND 0x80000000
#define INFO_XOR 0xFFFFFFFF
#define INFO_SHIFT 8
#define INFO_ADD 0x100

#define ID_DIR 0

#define NO_NAME 0xFFFFFFFF

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

        int getShortInt(std::ifstream& /*file*/, int /*offset*/);

        int getShortIntBE(std::ifstream& /*file*/, int /*offset*/);

        int getInfoOffset();
        
        int getInfoSize();

        void decompressDat();
        
        void _openDatFile(std::string /*datFileName*/);

        void getFileInfo();

        bool checkCMP2();
        
        bool checkHdrFile();

        bool isNewFormat();

        /* Get a string from the contents of a u_int32_t location. This gets the
        little endian representation.
        Returns: A cstring containing the values from the int's memory location
        Be sure to free the string!
        */
        char* longToStr(unsigned int);

        /*
        Unused for now, but I had a reason for this in mind at one point 
        Speaks for itself, converts a string to an upper. There's probably 
        a faster way to do this, but until this function gets used a lot, 
        this will work fine.

        Returns: Uppercase String
        */
        std::string toUpper(std::string&);
    
    public: 
        TTDat();

        TTDat(std::string, std::string);

        ~TTDat();

        void openDatFile(std::string, std::string);

        bool hasHdr() { return this->infoLoc;};

        /*
         These are public for testing purposes, they will be made private and 
         necessary getters will be created once initial functionality is 
         implemented
         */

        unsigned int fileCount;

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