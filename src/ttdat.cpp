#include "libttdat/ttdat.hpp"

#ifdef _WIN32
#include "win32/pch.h"
#endif

TTDat::TTDat() {
    infoOffset = 0;
    newInfoOffset = 0;
    nameInfoOffset = 0;
    crcsOffset = 0;
    infoSize = 0;
    fileCount = 0;
    fileNameCount = 0;
    fileNamesSize = 0;
    fileNamesOffset = 0;
    infoType = 0;
    errorState = TTDAT_NO_ERROR;
    datFilePath = "";
    datFileName = "";
    datCompressed = false;
}

TTDat::TTDat(std::string filePath, std::string fileName) {
    infoOffset = 0;
    newInfoOffset = 0;
    nameInfoOffset = 0;
    crcsOffset = 0;
    infoSize = 0;
    fileCount = 0;
    fileNameCount = 0;
    fileNamesSize = 0;
    fileNamesOffset = 0;
    infoType = 0;
    errorState = TTDAT_NO_ERROR;
    datFilePath = filePath;
    datFileName = fileName;
    datCompressed = false;

    open_dat_file(datFilePath + datFileName);

    if (!(datFile.is_open())) {
        errorState = TTDAT_FILE_ERROR;
        return;
    }

    check_hdr_file();

    if (check_cmp2()) {
        decompress_dat();
    }

    get_dat_info();
    get_file_names();
    get_file_offsets();
    get_crc_size();
    get_crcs();
}

TTDat::~TTDat(){
    if (datFile.is_open())
        datFile.close();
    
    if (hdrFile.is_open())
        hdrFile.close();
}

void TTDat::open_dat_file (std::string fileName){
    datFile.open(fileName, std::ios::binary);
}

void TTDat::openDatFile (std::string pathName, std::string fileName){
    open_dat_file(pathName + fileName);
}

/* Reads an int of size 'size' from 'file' at 'offset' as a little endian value */
int TTDat::get_int(std::ifstream& file, unsigned short size, unsigned int offset) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 4);

    for (int i = size - 1; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

/* Reads an int of size 'size' from 'file' at 'offset' as a big endian value */
int TTDat::get_int_be(std::ifstream& file, unsigned short size, unsigned int offset) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

/* Reads an int of size 'size' from 'file' at 'offset' as a little endian value */
int TTDat::get_int(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = size - 1; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

/* Reads an int of size 'size' from 'file' at 'offset' as a big endian value */
int TTDat::get_int_be(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

int TTDat::get_info_offset () {
    int tmp = get_int(datFile, S_LONG, 0);

        if (tmp & INFO_AND) {
            tmp ^= INFO_XOR;
            tmp <<= INFO_SHIFT;
            tmp += INFO_ADD;
        }

    return tmp;
}

bool TTDat::is_new_format() {
    char *infoTypeStr, *fileCountStr;
    infoTypeStr = long_to_str(infoType);
    fileCountStr = long_to_str(fileCount);

    if ((!strncmp(infoTypeStr, "\x34\x43\x43\x2e", 4) || !strncmp(infoTypeStr, "\x2e\x43\x43\x34", 4)) ||
       (!strncmp(fileCountStr, "\x34\x43\x43\x2e", 4) || !strncmp(fileCountStr, "\x2e\x43\x43\x34", 4))) {
        free(infoTypeStr);
        free(fileCountStr);
        return true;
    }

    free(infoTypeStr);
    free(fileCountStr);
    return false;
}

void TTDat::decompress_dat () {
    /* Decompress dat to new file */
    /* Close datFile and reopen with new decompressed dat */
}

bool TTDat::check_cmp2 () {
    char* cmpStr = (char*)malloc(17);
    datFile.seekg(0);
    datFile.read(cmpStr, 16);

    if (strncmp(cmpStr, "CMP2CMP2CMP2CMP2", 16) == 0) {
        datCompressed = true;
        free(cmpStr);
        return true;
    }
    
    free(cmpStr);
    return false;
}

bool TTDat::check_hdr_file () {
    size_t dotPos = datFileName.find_last_of('.');
    std::string hdrFileName = datFileName.substr(0, dotPos) + ".HDR";

    hdrFile.open(datFilePath + hdrFileName, std::ios::binary);

    if (hdrFile.is_open()) {
        infoLoc = HDR;
        return true;
    }
    
    hdrFile.clear();
    infoLoc = DAT;
    return false;
}

char* TTDat::long_to_str(int integer) {
    char* intStr = (char*)malloc(5);
    memcpy(intStr, &integer, 4);
    intStr[4] = '\0';
    
    return intStr;
}

std::string TTDat::to_upper(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);

    return string;
}

void TTDat::get_dat_info() {
        std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

        if (infoLoc) {
            infoOffset = 12;
            infoSize = get_int_be(hdrFile, S_LONG, 0); // Size of .HDR - 4
            infoType = get_int(hdrFile, S_LONG);
            fileCount = get_int(hdrFile, S_LONG);
        } else {
            infoOffset = get_info_offset();
            infoSize = get_int(datFile, S_LONG, 4);
            infoType = get_int(datFile, S_LONG, infoOffset);
            fileCount = get_int(datFile, S_LONG);
        }
        
        if ((newFormat = is_new_format())) {
            infoType = get_int_be(infoFile, S_LONG, infoOffset + 12);
            newFormatVersion = get_int_be(infoFile, S_LONG);
            fileCount = get_int_be(infoFile, S_LONG);
            fileNameCount = get_int_be(infoFile, S_LONG);
            fileNamesSize = get_int_be(infoFile, S_LONG);
            fileNamesOffset = infoOffset + 32;
            nameInfoOffset = fileNamesSize + fileNamesOffset;
            fileOffsOffset = nameInfoOffset + (fileNameCount * 12) + 4;
            crcsOffset = fileOffsOffset + (fileCount * 16) + 8;
        } else {
            newInfoOffset = infoFile.tellg();
            nameInfoOffset = newInfoOffset + (fileCount * 16) + 4;
            fileNameCount = get_int(infoFile, S_LONG, nameInfoOffset - 4);
            nameFieldSize = (infoType <= -5) ? 12 : 8;
            fileNamesOffset = (fileNameCount * nameFieldSize) + nameInfoOffset + 4;
            crcsOffset = get_int(infoFile, S_LONG, fileNamesOffset - 4) + fileNamesOffset - 4;
            fileNamesSize = crcsOffset - fileNamesOffset - 4;
        }
}

void TTDat::get_file_names() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    unsigned int currOffset;

    if (fileNameCount * 64 > 1073741824) { // If fileNameCount would result in massive amounts of memory being allocated (something is wrong)
        errorState = TTDAT_OFFSET_ERROR;
        return;
    }

    fileNames = new fileName[fileNameCount];
    
    if (newFormat) {
        /* New Format */
        int nameOffset;

        currOffset = nameInfoOffset + 4;
        infoFile.seekg(currOffset);

        for (int i = 0; i < (fileNameCount); i++) {
            nameOffset = get_int_be(infoFile, S_LONG);

            fileNames[i].previous = get_int_be(infoFile, S_SHORT);
            if (newFormatVersion >= 2)
                infoFile.ignore(S_SHORT);

            fileNames[i].next.u = get_int_be(infoFile, S_SHORT);
            currOffset = infoFile.tellg();
            infoFile.seekg(nameOffset + fileNamesOffset);
            std::getline(infoFile, fileNames[i].fileName, '\0');
            if ((fileNames[i].next.u == 0 && i != 0) || (fileNames[i].next.u != 0 && fileNames[i - 1].next.u == 0))
                fileNames[i].fileName = fileNames[fileNames[i].previous].fileName + "/" + fileNames[i].fileName;

            infoFile.seekg(currOffset + 2);
        }
    } else {
        /* Old format */
        infoFile.seekg(nameInfoOffset);
        int currOffset;
        int nameOffset;
        std::string tmpPath = "";
        for (int i = 0; i < fileNameCount; i++) {
            fileNames[i].next.s = get_int(infoFile, S_SHORT);
            fileNames[i].previous = get_int(infoFile, S_SHORT);
            nameOffset = get_int(infoFile, S_LONG);
            if (infoType <= -5) infoFile.ignore(S_LONG);
            currOffset = infoFile.tellg();
            infoFile.seekg(fileNamesOffset + nameOffset);
            std::getline(infoFile, fileNames[i].fileName, '\0');
            infoFile.seekg(currOffset);
            if (fileNames[i].previous != 0) tmpPath = fileNames[(fileNames[i].previous)].pathName;
            fileNames[i].pathName = tmpPath;
            if (fileNames[i].next.s > 0) {
                if (fileNames[i].fileName != "") {
                    tmpPath += fileNames[i].fileName + "/";
                }
            }
        }
    }
}

void TTDat::get_file_offsets() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    
    infoFile.seekg(fileOffsOffset);

    if (newFormat) {
        infoType = get_int_be(infoFile, S_LONG);       // FIXME: The quickbms script has these checks, but in the files I've tested, these values
        fileCount = get_int_be(infoFile, S_LONG);      // appear to be the same as those at the beginning of the info offset +4 and +8, respectively.
                                                            // This needs More Testing; It's possible that some dat variants use a mix of this version number

        struct {unsigned int offset; unsigned int size; int packed; unsigned int zsize;} fileList[fileCount];
        unsigned int offsetOr;
        for (unsigned int i = 0; i < fileCount; i++) {
            if (infoType <= -13) {
                fileList[i].packed = get_int_be(infoFile, S_SHORT);
                infoFile.ignore(S_SHORT);
                fileList[i].offset = get_int_be(infoFile, S_LONG);
            } else if (infoType <= -11) {
                fileList[i].offset = get_int_be(infoFile, S_LONGLONG);
            } else {
                fileList[i].offset = get_int_be(infoFile, S_LONG);
            }

            fileList[i].zsize = get_int_be(infoFile, S_LONG);
            fileList[i].size = get_int_be(infoFile, S_LONG);

            if (infoType <= -13) {
                fileList[i].packed = fileList[i].packed ? 2 : 0;
            } else if (infoType <= -10) {
                fileList[i].packed = (fileList[i].size >> 31) ? 2 : 0;
                fileList[i].size &= 0x7FFFFFFF;
            } else {
                fileList[i].packed = get_int_be(infoFile, S_BYTE);
                infoFile.ignore(S_SHORT);
                offsetOr = get_int_be(infoFile, S_BYTE);
                fileList[i].offset |= offsetOr;
            }
        }
    } else {

    }
}

void TTDat::get_crc_size() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    
}

void TTDat::get_crcs() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

}