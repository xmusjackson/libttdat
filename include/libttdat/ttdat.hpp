#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <string.h>

#include "win32.hpp"
#include "types.hpp"
#include "defs.hpp"

class LIBTTDAT_API TTDat {
    private:
        std::ifstream datFile;

        std::ifstream hdrFile;

        std::string datFilePath;

        std::string datFileName;
        
        fileData* fileList;
        
        fileData* modFileList;

        loc infoLoc;

        int getInt(std::ifstream& /*file*/, unsigned short /*size*/, unsigned int /*offset*/);

        int getIntBE(std::ifstream& /*file*/, unsigned short /*size*/, unsigned int /*offset*/);

        int getInt(std::ifstream& /*file*/, unsigned short /*size*/);

        int getIntBE(std::ifstream& /*file*/, unsigned short /*size*/);

        int getInfoOffset();

        void decompressDat();
        
        void _openDatFile(std::string /*datFileName*/);

        void getFileInfo();

        void getFileList();

        bool checkCMP2();
        
        bool checkHdrFile();

        bool isNewFormat();

        /* Get a string from the contents of a 32 bit integer location. This gets the
        little endian representation.
        Returns: A cstring containing the values from the int's memory location
        Be sure to free the string!
        */
        char* longToStr(int);

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

        int fileCount;

        int fileNameCount;

        int fileNamesSize;

        unsigned int fileNamesOffset;

        int nameFieldSize;

        unsigned int nameInfoOffset;

        int infoType;

        int hdrSize;

        unsigned int crcsOffset;
        
        unsigned int infoOffset;

        int newInfoOffset;

        int infoSize;

        int newFormatVersion;

        bool newFormat;

        unsigned int newFormatCheck;

        bool isCompressed;

        TTDatError errorState;

};
