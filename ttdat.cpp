#include "ttdat.hpp"

TTDat::TTDat() {
}

TTDat::TTDat(std::string filePath, std::string fileName) {
    this->infoOffset = 0;
    this->infoSize = 0;
    this->fileCount = 0;
    this->infoType = 0;
    this->errorState = NO_ERROR;
    this->datFilePath = filePath;
    this->datFileName = fileName;
    this->_openDatFile(this->datFilePath + this->datFileName);

    if (!(this->datFile.is_open())) {
        this->errorState = FILE_ERROR;
        return;
    }

    this->checkHdrFile();

    if (this->checkCMP2()) {
        this->decompressDat();
    }

    switch (this->infoLocType) {
        case DAT:
            this->getDatInfo();
            break;

        case HDR:
            this->getHdrInfo();
            break;

        default:
            this->errorState = GENERAL_ERROR; // Until I implement better error handling
            return;
    }
}

TTDat::~TTDat(){
    if (this->datFile.is_open())
        this->datFile.close();
    
    if (this->hdrFile.is_open())
        this->hdrFile.close();
}

void TTDat::_openDatFile (std::string fileName){
    this->datFile.open(fileName);
    if (!this->datFile.is_open())
        this->errorState = FILE_ERROR;
}

void TTDat::openDatFile (std::string pathName, std::string fileName){
    this->_openDatFile(pathName + fileName);
}

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

/* Reads a long int as big endian */
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

int TTDat::getInfoOffset () {
    int tmp = getLongInt(this->datFile, 0);

        if (tmp & INFO_AND) {
            tmp ^= INFO_XOR;
            tmp <<= INFO_SHIFT;
            tmp += INFO_ADD;
        }

    return tmp;
}

bool TTDat::isNewFormat() {
    char *infoTypeStr, *fileCountStr;
        infoTypeStr = this->longToStr(this->infoType);

        fileCountStr = this->longToStr(this->fileCount);

    if ((!strcmp(infoTypeStr, "\0x34\0x43\0x43\0x2e") || !strcmp(infoTypeStr, "\0x2e\0x43\0x43\0x34")) ||
       (!strcmp(fileCountStr, "\0x34\0x43\0x43\0x2e") || !strcmp(fileCountStr, "\0x2e\0x43\0x43\0x34"))) {
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
    char* cmpStr = (char*)malloc(16);
    this->datFile.seekg(0);
    this->datFile.read(cmpStr, 16);

    if (!strncmp(cmpStr, "CMP2CMP2CMP2CMP2", 16)) {
        this->isCompressed = true;
        return true;
    }

    return false;
}

bool TTDat::checkHdrFile () {
    size_t dotPos = this->datFileName.find_last_of('.');
    std::string hdrFileName = this->datFileName.substr(0, dotPos) + ".HDR";

    this->hdrFile.open(this->datFilePath + hdrFileName);

    if (this->hdrFile.is_open()) {
        this->infoLocType = HDR;
        return true;
    }
    
    this->hdrFile.clear();
    this->infoLocType = DAT;
    return false;
}

void TTDat::getDatInfo() {
        this->infoOffset = this->getInfoOffset();
        this->infoSize = this->getLongInt(this->datFile, 4);
        this->infoType = this->getLongInt(this->datFile, this->infoOffset);
        this->fileCount = this->getLongInt(this->datFile, this->infoOffset + 4);
        if ((this->newFormat = this->isNewFormat())) {

        }
}

void TTDat::getHdrInfo() {
        this->infoOffset = 4;
        this->infoSize = this->getLongIntBE(this->hdrFile, 0);
        this->infoType = this->getLongInt(this->hdrFile, this->infoOffset);
        this->fileCount = this->getLongInt(this->hdrFile, this->infoOffset + 4);
        if ((this->newFormat = this->isNewFormat())) {

        }
}

char* TTDat::longToStr(u_int32_t integer) {
    char* intStr = (char*)malloc(4);
    strlcpy(intStr, (char*)&integer, 4);
    
    return intStr;
}

std::string TTDat::toUpper(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);

    return string;
}