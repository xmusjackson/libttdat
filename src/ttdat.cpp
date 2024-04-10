#include "libttdat/ttdat.hpp"
#include "crc.hpp"
#include "util.hpp"

#ifdef _WIN32
#include "win32/pch.h"
#endif

TTDat::TTDat()
:   infoOffset(0),
    newInfoOffset(0),
    nameInfoOffset(0),
    crcsOffset(0),
    infoSize(0),
    fileCount(0),
    fileNameCount(0),
    fileNamesSize(0),
    fileNamesOffset(0),
    fileOffsOffset(0),
    infoType(0),
    errorState(TTDAT_NO_ERROR),
    datCompressed(false),
    infoLoc(DAT),
    crc64(false),
    fileList(nullptr),
    nameList(nullptr)
{}

TTDat::TTDat(std::string filePath, std::string fileName)
:   infoOffset(0),
    newInfoOffset(0),
    nameInfoOffset(0),
    crcsOffset(0),
    infoSize(0),
    fileCount(0),
    fileNameCount(0),
    fileNamesSize(0),
    fileNamesOffset(0),
    fileOffsOffset(0),
    infoType(0),
    errorState(TTDAT_NO_ERROR),
    datFilePath(filePath),
    datFileName(fileName),
    datCompressed(false),
    infoLoc(DAT),
    crc64(false),
    fileList(nullptr),
    nameList(nullptr)
{
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
    alloc_lists();

    get_file_names();
    get_file_offsets();
    get_crcs();

    match_crcs();
}

TTDat::~TTDat(){
    if (datFile.is_open())
        datFile.close();
    
    if (hdrFile.is_open())
        hdrFile.close();

    if (fileList != nullptr) delete[] fileList;
    if (nameList != nullptr) delete[] nameList;
}

void TTDat::open_dat_file (std::string fileName){
    datFile.open(fileName, std::ios::binary);
}

void TTDat::openDatFile (std::string pathName, std::string fileName){
    open_dat_file(pathName + fileName);
}

int TTDat::get_info_offset () {
    int tmp = ttdatutil::get_int(datFile, S_LONG, 0);

        if (tmp & INFO_AND) {
            tmp ^= INFO_XOR;
            tmp <<= INFO_SHIFT;
            tmp += INFO_ADD;
        }

    return tmp;
}

bool TTDat::is_new_format() {
    char* infoTypeStr = ttdatutil::long_to_str(infoType);
    char *fileCountStr = ttdatutil::long_to_str(fileCount);

    if ((!strncmp(infoTypeStr, "\x34\x43\x43\x2e", 4) || !strncmp(infoTypeStr, "\x2e\x43\x43\x34", 4)) ||
       (!strncmp(fileCountStr, "\x34\x43\x43\x2e", 4) || !strncmp(fileCountStr, "\x2e\x43\x43\x34", 4))) {
        delete[] infoTypeStr;
        delete[] fileCountStr;
        return true;
    }

    delete[] infoTypeStr;
    delete[] fileCountStr;
    return false;
}

void TTDat::decompress_dat () {
    /* Decompress dat to new file */
    /* Close datFile and reopen with new decompressed dat */
}

bool TTDat::check_cmp2 () {
    char* cmpStr = new char[17];
    datFile.seekg(0);
    datFile.read(cmpStr, 16);

    if (strncmp(cmpStr, "CMP2CMP2CMP2CMP2", 16) == 0) {
        datCompressed = true;
        delete[] cmpStr;
        return true;
    }
    
    delete[] cmpStr;
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

void TTDat::get_dat_info() {
        std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

        if (infoLoc) {
            infoOffset = 12;
            infoSize = ttdatutil::get_int_be(hdrFile, S_LONG, 0); // Size of .HDR - 4
            infoType = ttdatutil::get_int(hdrFile, S_LONG);
            fileCount = ttdatutil::get_int(hdrFile, S_LONG);
        } else {
            infoOffset = get_info_offset();
            infoSize = ttdatutil::get_int(datFile, S_LONG, 4);
            infoType = ttdatutil::get_int(datFile, S_LONG, infoOffset);
            fileCount = ttdatutil::get_int(datFile, S_LONG);
        }
        
        if ((newFormat = is_new_format())) {
            infoType = ttdatutil::get_int_be(infoFile, S_LONG, infoOffset + 12);
            newFormatVersion = ttdatutil::get_int_be(infoFile, S_LONG);
            fileCount = ttdatutil::get_int_be(infoFile, S_LONG);
            fileNameCount = ttdatutil::get_int_be(infoFile, S_LONG);
            fileNamesSize = ttdatutil::get_int_be(infoFile, S_LONG);
            fileNamesOffset = infoOffset + 32;
            nameInfoOffset = fileNamesSize + fileNamesOffset;
            fileOffsOffset = nameInfoOffset + (fileNameCount * 12) + 4;
            crcsOffset = fileOffsOffset + (fileCount * 16) + 8;
            get_crc_size();
        } else {
            newInfoOffset = infoFile.tellg();
            nameInfoOffset = newInfoOffset + (fileCount * 16) + 4;
            fileNameCount = ttdatutil::get_int(infoFile, S_LONG, nameInfoOffset - 4);
            nameFieldSize = (infoType <= -5) ? 12 : 8;
            fileNamesOffset = (fileNameCount * nameFieldSize) + nameInfoOffset + 4;
            crcsOffset = ttdatutil::get_int(infoFile, S_LONG, fileNamesOffset - 4) + fileNamesOffset - 4;
            fileNamesSize = crcsOffset - fileNamesOffset - 4;
            get_crc_size();
            fileOffsOffset = infoOffset + (newInfoOffset - infoOffset); // FIXME: Probably breaks HDRs, needs testing
        }
}

void TTDat::get_file_names() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    unsigned int currOffset;

    nameList = new fileName[fileNameCount];
    
    if (newFormat) {
        /* New Format */
        unsigned int nameOffset;

        currOffset = nameInfoOffset + 4;
        infoFile.seekg(currOffset);

        for (int i = 0; i < (fileNameCount); i++) {
            nameOffset = ttdatutil::get_int_be(infoFile, S_LONG);
            nameList[i].previous = ttdatutil::get_int_be(infoFile, S_SHORT);

            if (newFormatVersion >= 2)
                infoFile.ignore(S_SHORT);

            nameList[i].next.u = ttdatutil::get_int_be(infoFile, S_SHORT);
            currOffset = infoFile.tellg();

            if (i != 0) {
                infoFile.seekg(nameOffset + fileNamesOffset);
                std::getline(infoFile, nameList[i].fileName, '\0');
            }

            if (i != 0 && nameList[i].previous != 0) // if previous is 0, path is root dir
                nameList[i].fileName = nameList[nameList[i].previous].fileName + "\\" + nameList[i].fileName;
            
            if (nameList[i - 1].next.u == 0) {
                if (crc64) {
                    crcNameList[ttdatcrc::crc_fnv_1a_64(ttdatutil::to_upper(nameList[i].fileName).c_str())] = nameList[i].fileName;
                } else {
                    crcNameList[ttdatcrc::crc_fnv_1a_32(ttdatutil::to_upper(nameList[i].fileName).c_str())] = nameList[i].fileName;
                }
            }

            infoFile.seekg(currOffset + 2);
        }
    } else {
        /* Old format  *This implementation could use some work, but it works as it is */
        infoFile.seekg(nameInfoOffset);
        int currOffset;
        int nameOffset;
        std::string tmpPath = "";
        for (int i = 0; i < fileNameCount; i++) {
            nameList[i].next.s = ttdatutil::get_int(infoFile, S_SHORT);
            nameList[i].previous = ttdatutil::get_int(infoFile, S_SHORT);
            nameOffset = ttdatutil::get_int(infoFile, S_LONG);
            if (infoType <= -5) infoFile.ignore(S_LONG);
            currOffset = infoFile.tellg();

            if (nameOffset >= 0) {
                infoFile.seekg(fileNamesOffset + nameOffset);
                std::getline(infoFile, nameList[i].fileName, '\0');
                infoFile.seekg(currOffset);
            } else {
                nameList[i].fileName = "";
            }

            if (nameList[i].previous != 0 && i != 0) tmpPath = nameList[(nameList[i].previous)].pathName;
            nameList[i].pathName = tmpPath;

            if (nameList[i].next.s > 0) {
                if (nameList[i].fileName != "") {
                    tmpPath += nameList[i].fileName + "\\";
                }
            }
            crcNameList[ttdatcrc::crc_fnv_1a_32(ttdatutil::to_upper(nameList[i].pathName + nameList[i].fileName).c_str())] = nameList[i].pathName + nameList[i].fileName;
        }
    }
}

void TTDat::get_file_offsets() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

    fileList = new fileData[fileCount];

    infoFile.seekg(fileOffsOffset);

    if (newFormat) {
        infoType = ttdatutil::get_int_be(infoFile, S_LONG);       // FIXME: The quickbms script has these checks, but in the files I've tested, these values
        fileCount = ttdatutil::get_int_be(infoFile, S_LONG);      // appear to be the same as those at the beginning of the info offset +4 and +8, respectively.
                                                                  // This needs More Testing; It's possible that some dat variants use a mix of this version number
        unsigned int offsetOr;

        for (unsigned int i = 0; i < fileCount; i++) {
            if (infoType <= -11) {
                fileList[i].fileOffset = ttdatutil::get_int_be(infoFile, S_LONGLONG);
            } else {
                fileList[i].fileOffset = ttdatutil::get_int_be(infoFile, S_LONG);
            }

            fileList[i].fileZSize = ttdatutil::get_int_be(infoFile, S_LONG);
            fileList[i].fileSize = ttdatutil::get_int_be(infoFile, S_LONG);

            if (infoType <= -13) {
                fileList[i].filePacked = (fileList[i].fileOffset >> 56) ? 1 : 0; // This should be looked into. This seems to correlate with compression type
                fileList[i].fileOffset &= 0xFFFFFFFFFFFFFFul;
            } else if (infoType <= -10) {
                fileList[i].filePacked = (fileList[i].fileSize >> 31) ? 1 : 0;
                fileList[i].fileSize &= 0x7FFFFFFFul;
            } else {
                fileList[i].filePacked = ttdatutil::get_int_be(infoFile, S_BYTE);
                infoFile.ignore(S_SHORT);
                offsetOr = ttdatutil::get_int_be(infoFile, S_BYTE);
                fileList[i].fileOffset = (fileList[i].fileOffset << 8) | offsetOr;
            }
        }
    } else {
        unsigned short offsetAdd;

        for (int i{0}; i < fileCount; i++){
            fileList[i].fileOffset = ttdatutil::get_int(infoFile, S_LONG);
            fileList[i].fileZSize = ttdatutil::get_int(infoFile, S_LONG);
            fileList[i].fileSize = ttdatutil::get_int(infoFile, S_LONG);
            fileList[i].filePacked = ttdatutil::get_int(infoFile, 3);
            offsetAdd = ttdatutil::get_int(infoFile, S_BYTE);
            if (infoType != -1)
                fileList[i].fileOffset <<= 8;

            fileList[i].fileOffset += offsetAdd;
        }
    }
}

void TTDat::get_crc_size() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

    /*
     * The method used to determine the size of the CRCs requires more research.
     * The following implementation is adapted from the quickbms script and is a little hacky.
     * This is subject to change. Error handling should be implemented at the least.
     */

    unsigned int crcs32EndOffset = crcsOffset + (fileCount * 4);
    unsigned int crcs64EndOffset = crcsOffset + (fileCount * 8);
    int falsePositiveCheck;

    if (newFormat) {
        falsePositiveCheck = ttdatutil::get_int_be(infoFile, S_LONG, crcs32EndOffset);
    } else {
        falsePositiveCheck = ttdatutil::get_int(infoFile, S_LONG, crcs32EndOffset);
    }

    if (falsePositiveCheck == 0 || falsePositiveCheck == 9){  // 9 for The Force Awakens, otherwise the long at the end of these lists is usually zero
        crc64 = false;
        return;
    }

    if (crcs64EndOffset < (infoOffset + infoSize))
        crc64 = true;

}

void TTDat::get_crcs() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    unsigned short intSize = (crc64) ? S_LONGLONG : S_LONG;

    infoFile.seekg(crcsOffset);

    if (newFormat) {
        for (int i = 0; i < fileCount; i++) {
            fileList[i].nameCrc = ttdatutil::get_int_be(infoFile, intSize);
        }
    } else {
        for (int i = 0; i < fileCount; i++) {
            fileList[i].nameCrc = ttdatutil::get_int(infoFile, intSize);
        }
    }   
}

void TTDat::match_crcs() {
    for (int i = 0; i < fileCount; i++){
        if (crcNameList.find(fileList[i].nameCrc) != crcNameList.end()) {
            fileList[i].fileName = crcNameList.at(fileList[i].nameCrc);
        }
    }

}

void TTDat::alloc_lists() {
    // There's probably a better way to do this, but this works.
    // Future error handling might render this function unneeded.

    if (fileCount * sizeof(fileData) > 1073741824) {
        errorState = TTDAT_OFFSET_ERROR;
        return;
    }

    fileList = new fileData[fileCount];

    if (fileNameCount * sizeof(fileName) > 1073741824) {
        errorState = TTDAT_OFFSET_ERROR;
        return;
    }

    nameList = new fileName[fileNameCount];
}