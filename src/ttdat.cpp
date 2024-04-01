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
    isCompressed = false;
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
    isCompressed = false;

    _openDatFile(datFilePath + datFileName);

    if (!(datFile.is_open())) {
        errorState = TTDAT_FILE_ERROR;
        return;
    }

    checkHdrFile();

    if (checkCMP2()) {
        decompressDat();
    }

    getFileInfo();
    getFileList();
}

TTDat::~TTDat(){
    if (datFile.is_open())
        datFile.close();
    
    if (hdrFile.is_open())
        hdrFile.close();
}

void TTDat::_openDatFile (std::string fileName){
    datFile.open(fileName, std::ios::binary);
}

void TTDat::openDatFile (std::string pathName, std::string fileName){
    _openDatFile(pathName + fileName);
}

/* Reads an int of size 'size' from 'file' at 'offset' as a little endian value */
int TTDat::getInt(std::ifstream& file, unsigned short size, unsigned int offset) {
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
int TTDat::getIntBE(std::ifstream& file, unsigned short size, unsigned int offset) {
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
int TTDat::getInt(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = size - 1; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

/* Reads an int of size 'size' from 'file' at 'offset' as a big endian value */
int TTDat::getIntBE(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

int TTDat::getInfoOffset () {
    int tmp = getInt(datFile, S_LONG, 0);

        if (tmp & INFO_AND) {
            tmp ^= INFO_XOR;
            tmp <<= INFO_SHIFT;
            tmp += INFO_ADD;
        }

    return tmp;
}

bool TTDat::isNewFormat() {
    char *infoTypeStr, *fileCountStr;
    infoTypeStr = longToStr(infoType);
    fileCountStr = longToStr(fileCount);

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

void TTDat::decompressDat () {
    /* Decompress dat to new file */
    /* Close datFile and reopen with new decompressed dat */
}

bool TTDat::checkCMP2 () {
    char* cmpStr = (char*)malloc(17);
    datFile.seekg(0);
    datFile.read(cmpStr, 16);

    if (strncmp(cmpStr, "CMP2CMP2CMP2CMP2", 16) == 0) {
        isCompressed = true;
        free(cmpStr);
        return true;
    }
    
    free(cmpStr);
    return false;
}

bool TTDat::checkHdrFile () {
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

char* TTDat::longToStr(int integer) {
    char* intStr = (char*)malloc(5);
    memcpy(intStr, &integer, 4);
    intStr[4] = '\0';
    
    return intStr;
}

std::string TTDat::toUpper(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);

    return string;
}

void TTDat::getFileInfo() {
        std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

        if (infoLoc) {
            infoOffset = 12;
            infoSize = getIntBE(hdrFile, S_LONG, 0);
            infoType = getInt(hdrFile, S_LONG);
            fileCount = getInt(hdrFile, S_LONG);
        } else {
            infoOffset = getInfoOffset();
            infoSize = getInt(datFile, S_LONG, 4);
            infoType = getInt(datFile, S_LONG, infoOffset);
            fileCount = getInt(datFile, S_LONG);
        }
        
        if ((newFormat = isNewFormat())) {
            infoType = getIntBE(infoFile, S_LONG, infoOffset + 12);
            newFormatVersion = getIntBE(infoFile, S_LONG);

            fileCount = getIntBE(infoFile, S_LONG);
            fileNameCount = getIntBE(infoFile, S_LONG);
            fileNamesSize = getIntBE(infoFile, S_LONG);
            fileNamesOffset = infoOffset + 32;
            
            nameInfoOffset = fileNamesSize + fileNamesOffset;
        } else {
            if (!(infoLoc)) newInfoOffset = 8;
            nameInfoOffset = infoOffset + newInfoOffset + (fileCount * 16) + 12;
            fileNameCount = getInt(infoFile, S_LONG, nameInfoOffset - 8);
            nameFieldSize = (infoType <= -5) ? 12 : 8;
            fileNamesOffset = (fileNameCount * nameFieldSize) + nameInfoOffset - 8;
            fileNamesSize = getInt(infoFile, S_LONG, fileNamesOffset);
            crcsOffset = fileNamesSize + fileNamesOffset + 4;
        }
}

void TTDat::getFileList() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    unsigned int currOffset;

    if (fileNameCount * 64 > 1073741824) { // If fileNameCount would result in massive amounts of memory being allocated (something is wrong)
        errorState = TTDAT_OFFSET_ERROR;
        return;
    }

    fileList = new fileData[fileNameCount];
    
    if (newFormat) {

            /* Get file ids, path ids, and name offsets */
            currOffset = nameInfoOffset + 4;
            infoFile.seekg(currOffset);
            for (int i = 0; i < (fileNameCount); i++) {
                fileList[i].nameOffset = getIntBE(infoFile, S_LONG);
                if (i == 0)
                    fileList[i].nameOffset += 1;

                fileList[i].pathID = getIntBE(infoFile, S_SHORT);
                if (newFormatVersion >= 2)
                    infoFile.ignore(2);

                fileList[i].fileID = getIntBE(infoFile, S_SHORT);
                currOffset = infoFile.tellg();
                infoFile.seekg(fileList[i].nameOffset + fileNamesOffset);
                std::getline(infoFile, fileList[i].fileName, '\0');
                if (fileList[i].fileID == 0 && i != 0 && i != fileNameCount - 1)
                    fileList[i].fileName = fileList[fileList[i].pathID].fileName + "/" + fileList[i].fileName;

                infoFile.seekg(currOffset + 2);
            }

            fileList[fileNameCount - 1].fileID = fileNameCount - 1;

            /* Get file offsets, 'packed', size, and zsize; 
               Note: These offets do not align with the above file list, they align with
               the below crc list which must be used to get the file name for each 
             */

            infoType = getIntBE(infoFile, S_LONG);       // FIXME: The quickbms script has these checks, but in the files I've tested, these values
            fileCount = getIntBE(infoFile, S_LONG);      // appear to be the same as those at the beginning of the info offset +4 and +8, respectively.
                                                              // This needs More Testing; It's possible that some dat variants use a mix of this version number


            for (unsigned int i = 0, j = 0; i < fileCount; (i++, j++)) {
                if (infoType <= -13) {

                }
                else if (infoType <= -11) {

                }
                else {

                }
            }
    } else {
            //currOffset = nameInfoOffset;
            for (unsigned int i= 0; i < fileCount; i++) {

            }
    }
}
