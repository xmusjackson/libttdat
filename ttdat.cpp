#include "ttdat.hpp"

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
    errorState = NO_ERROR;
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

    hdrFile.open(datFilePath + hdrFileName, std::ios::binary);

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
    fileList = new fileData[fileNameCount];
    
    if (newFormat) {
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

                std::getline(infoFile, (fileList[i].fileName), '\0');
                if (fileList[i].fileID == 0 && i != 0 && i != fileNameCount - 1)
                    fileList[i].fileName = fileList[fileList[i].pathID].fileName + "/" + fileList[i].fileName;

                infoFile.seekg(currOffset);
                infoFile.ignore(2);
            }

            fileList[fileNameCount - 1].fileID = fileNameCount - 1;

            infoType = getIntBE(infoFile, S_LONG);       // FIXME: The quickbms script has these checks, but in the files I've tested, these values
            fileCount = getIntBE(infoFile, S_LONG);      // appear to be the same as those at the beginning of the info offset +4 and +8, respectively.
                                                         // This needs More Testing; It's possible that some dat variants use a mix of this version number
    } else {
            currOffset = nameInfoOffset;
            for (int i= 0; i < fileNameCount; i++) {

            }
    }
}
