#include "ttdat.hpp"

TTDat::TTDat() {
}

TTDat::TTDat(std::string filePath, std::string fileName) {
    infoOffset = 0;
    newInfoOffset = 0;
    nameInfoOffset = 0;
    infoSize = 0;
    fileCount = 0;
    fileNameCount = 0;
    fileNamesSize = 0;
    fileNamesOffset = 0;
    infoType = 0;
    errorState = NO_ERROR;
    datFilePath = filePath;
    datFileName = fileName;
    isCompressed = false;
    _openDatFile(datFilePath + datFileName);

    if (!(datFile.is_open())) {
        errorState = FILE_ERROR;
        return;
    }

    checkHdrFile();

    if (checkCMP2()) {
        decompressDat();
    }

    getFileInfo();
}

TTDat::~TTDat(){
    if (datFile.is_open())
        datFile.close();
    
    if (hdrFile.is_open())
        hdrFile.close();
}

void TTDat::_openDatFile (std::string fileName){
    datFile.open(fileName);
}

void TTDat::openDatFile (std::string pathName, std::string fileName){
    _openDatFile(pathName + fileName);
}

/* Reads a long int from file at offset as a little endian value */
int TTDat::getLongInt(std::ifstream& file, int offset) {
    unsigned char bytes[4];
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 4);

    for (int i = 3; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    return int32;
}

/* Reads a long int from file at offset as a big endian value */
int TTDat::getLongIntBE(std::ifstream& file, int offset) {
    unsigned char bytes[4];
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 4);

    for (int i = 0; i <= 3; i++)
        int32 = (int32 << 8) + bytes[i];

    return int32;
}

 /* Reads a short int from file at offset as a little endian value */
int TTDat::getShortInt(std::ifstream& file, int offset) {
    unsigned char bytes[2];
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 2);

    for (int i = 1; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    return int32;
}

/* Reads a short int from file at offset as a big endian value */
int TTDat::getShortIntBE(std::ifstream& file, int offset) {
    unsigned char bytes[2];
    int int32 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 2);

    for (int i = 0; i <= 1; i++)
        int32 = (int32 << 8) + bytes[i];

    return int32;
}

int TTDat::getInfoOffset () {
    int tmp = getLongInt(datFile, 0);

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
    
    if ((!strcmp(infoTypeStr, "\x34\x43\x43\x2e") || !strcmp(infoTypeStr, "\x2e\x43\x43\x34")) ||
       (!strcmp(fileCountStr, "\x34\x43\x43\x2e") || !strcmp(fileCountStr, "\x2e\x43\x43\x34"))) {
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

    hdrFile.open(datFilePath + hdrFileName);

    if (hdrFile.is_open()) {
        infoLoc = HDR;
        return true;
    }
    
    hdrFile.clear();
    infoLoc = DAT;
    return false;
}

char* TTDat::longToStr(unsigned int integer) {
    char* intStr = (char*)malloc(5);
    strlcpy(intStr, (char*)&integer, 5);
    
    return intStr;
}

std::string TTDat::toUpper(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);

    return string;
}

void TTDat::getFileInfo() {
        //int tmpOffset;
        std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

        if (infoLoc) {
            infoOffset = 12;
            infoSize = getLongIntBE(hdrFile, 0);
            infoType = getLongInt(hdrFile, 4);
            fileCount = getLongInt(hdrFile, 8);
        } else {
            infoOffset = getInfoOffset();
            infoSize = getLongInt(datFile, 4);
            infoType = getLongInt(datFile, infoOffset);
            fileCount = getLongInt(datFile, infoOffset + 4);
        }
        
        if ((newFormat = isNewFormat())) {
            infoType = getLongIntBE(infoFile, infoOffset + 12);
            newFormatVersion = getLongIntBE(infoFile, infoOffset + 16);
            
            fileCount = getLongIntBE(infoFile, infoOffset + 20);
            fileNameCount = getLongIntBE(infoFile, infoOffset + 24);
            fileNamesSize = getLongIntBE(infoFile, infoOffset + 28);
            fileNamesOffset = infoOffset + 32;
            
            nameInfoOffset = fileNamesSize + fileNamesOffset;
            fileList = new fileData[fileNameCount];
            
            int currOffset;
            currOffset = nameInfoOffset + 4;
            for (int i = 0; i < (fileNameCount); i++) {
                fileList[i].nameOffset = getLongIntBE(infoFile, currOffset);
                if (i == 0)
                    fileList[i].nameOffset += 1;

                fileList[i].pathID = getShortIntBE(infoFile, currOffset+=4);
                fileList[i].fileID = getShortIntBE(infoFile, (newFormatVersion >= 2) ? currOffset+=6 : currOffset+=4);
                infoFile.seekg(fileList[i].nameOffset + fileNamesOffset);
                std::getline(infoFile, (fileList[i].fileName), '\0');
                if (fileList[i].fileID == 0 && i != 0 && i != fileNameCount - 1)
                    fileList[i].fileName = fileList[fileList[i].pathID].fileName + "/" + fileList[i].fileName;

                currOffset += 2;
            }
            currOffset = infoFile.tellg();
            fileList[fileNameCount - 1].fileID = fileNameCount - 1;
            infoType = getLongIntBE(infoFile, currOffset);
            fileCount = getLongIntBE(infoFile, currOffset);

        } else {
            if (!(infoLoc)) {newInfoOffset = 8;}
            nameInfoOffset = infoOffset + newInfoOffset + (fileCount * 16) + 8;
            fileNameCount = getLongInt(infoFile, nameInfoOffset - 8);
            nameFieldSize = (infoType <= -5) ? 12 : 8;
            fileNamesOffset = (fileNameCount * nameFieldSize) + nameInfoOffset - 4;
            fileNamesSize = getLongInt(infoFile, fileNamesOffset);
            crcsOffset = getLongInt(infoFile, fileNamesOffset) + fileNamesOffset + 4;
            nameInfoOffset += 4;
            fileList = new fileData[fileNameCount];
        }
}