#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <algorithm>
#include <cstring>
#include <cstdint>
#include <unordered_map>

#include "win32.hpp"
#include "types.hpp"
#include "defs.hpp"

class LIBTTDAT_API TTDat {
    private:
        std::ifstream datFile;
        std::ifstream hdrFile;

        std::string datFilePath;
        std::string datFileName;

        std::unordered_map<uint64_t, std::string> crcNameList;
        
        fileData* fileList;
        fileData* modFileList; /* Unused for now */

        fileNameData* nameList;

        loc infoLoc;

        int fileCount;
        int fileNameCount;
        int fileNamesSize;
        int nameFieldSize;
        int infoType;
        int hdrSize; /* May not be needed, should be ~ 4 larger than infoSize */
        int infoSize;
        int newInfoOffset;
        int newFormatVersion;

        unsigned int crcsOffset;
        unsigned int infoOffset;
        unsigned int fileNamesOffset;
        unsigned int fileOffsOffset;
        unsigned int nameInfoOffset;
        unsigned int newFormatCheck;

        bool newFormat;
        bool datCompressed;
        bool crc64;

        TTDatError errorState;

        int get_info_offset();

        void decompress_dat();
        
        void open_dat_file(std::string fileName);

        void get_dat_info();

        void get_file_offsets();

        void get_file_names();
        
        void get_crcs();

        void get_crc_size();

        void match_crcs(); /* Match crcs from fileList to those from nameList */

        void alloc_lists();

        bool check_cmp2();
        
        bool check_hdr_file();

        bool is_new_format();
    
    public: 
        TTDat();

        TTDat(std::string filePath, std::string fileName);

        ~TTDat();

        void openDatFile(std::string filePath, std::string fileName);

        const bool hasHdr() { return (this->infoLoc != 0);};

        const int getFileCount() {return fileCount;};

        const int getFileNameCount() {return fileNameCount;};

        const int getFileNamesSize() {return fileNamesSize;};

        const unsigned int getFileNamesOffset(){return fileNamesOffset;};
        
        const unsigned int getFileOffsOffset(){return fileOffsOffset;};

        const int getNameFieldSize() {return nameFieldSize;};

        const unsigned int getNameInfoOffset() {return nameInfoOffset;};

        const int getInfoType() {return infoType;};

        const int getHdrSize() {return hdrSize;};

        const unsigned int getCrcsOffset() {return crcsOffset;};
        
        const unsigned int getHdrOffset() {return infoOffset;};

        const int getNewInfoOffset() {return newInfoOffset;};

        const int getInfoSize() {return infoSize;};

        const int getNewFormatVersion() {return newFormatVersion;};

        const bool getNewFormat() {return newFormat;};

        const unsigned int getNewFormatCheck() {return newFormatCheck;};

        const bool isCompressed() {return datCompressed;};

        const bool isCrc64() {return crc64;};

        const TTDatError error() {return errorState;};
};
