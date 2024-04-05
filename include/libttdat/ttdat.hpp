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
        
        fileData* modFileList; /* Unused for now */

        fileName* fileNames;

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

        TTDatError errorState;

        int get_info_offset();

        void decompress_dat();
        
        void open_dat_file(std::string /*datFileName*/);

        void get_dat_info();

        void get_file_offsets();

        void get_file_names();
        
        void get_crcs();

        void get_crc_size();

        bool check_cmp2();
        
        bool check_hdr_file();

        bool is_new_format();
    
    public: 
        TTDat();

        TTDat(std::string, std::string);

        ~TTDat();

        void openDatFile(std::string, std::string);

        bool hasHdr() { return (this->infoLoc != 0);};

        int getFileCount() {return fileCount;};

        int getFileNameCount() {return fileNameCount;};

        int getFileNamesSize() {return fileNamesSize;};

        unsigned int getFileNamesOffset(){return fileNamesOffset;};
        
        unsigned int getFileOffsOffset(){return fileOffsOffset;};

        int getNameFieldSize() {return nameFieldSize;};

        unsigned int getNameInfoOffset() {return nameInfoOffset;};

        int getInfoType() {return infoType;};

        int getHdrSize() {return hdrSize;};

        unsigned int getCrcsOffset() {return crcsOffset;};
        
        unsigned int getHdrOffset() {return infoOffset;};

        int getNewInfoOffset() {return newInfoOffset;};

        int getInfoSize() {return infoSize;};

        int getNewFormatVersion() {return newFormatVersion;};

        bool getNewFormat() {return newFormat;};

        unsigned int getNewFormatCheck() {return newFormatCheck;};

        bool isCompressed() {return datCompressed;};

        TTDatError error() {return errorState;};
};
