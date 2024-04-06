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
    crc64(false)
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
    crc64(false)
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

    delete[] fileList;
    delete[] fileNames;
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
        } else {
            newInfoOffset = infoFile.tellg();
            nameInfoOffset = newInfoOffset + (fileCount * 16) + 4;
            fileNameCount = ttdatutil::get_int(infoFile, S_LONG, nameInfoOffset - 4);
            nameFieldSize = (infoType <= -5) ? 12 : 8;
            fileNamesOffset = (fileNameCount * nameFieldSize) + nameInfoOffset + 4;
            crcsOffset = ttdatutil::get_int(infoFile, S_LONG, fileNamesOffset - 4) + fileNamesOffset - 4;
            fileNamesSize = crcsOffset - fileNamesOffset - 4;
        }
}

void TTDat::get_file_names() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);
    unsigned int currOffset;

    if (fileNameCount * sizeof(fileName) > 1073741824) { // If fileNameCount would result in massive amounts of memory being allocated (something is wrong)
        errorState = TTDAT_OFFSET_ERROR;   // This is rudamentary error checking and a new system should be devised
        return;
    }

    fileNames = new fileName[fileNameCount];
    
    if (newFormat) {
        /* New Format */
        int nameOffset;

        currOffset = nameInfoOffset + 4;
        infoFile.seekg(currOffset);

        for (int i = 0; i < (fileNameCount); i++) {
            nameOffset = ttdatutil::get_int_be(infoFile, S_LONG);

            fileNames[i].previous = ttdatutil::get_int_be(infoFile, S_SHORT);
            if (newFormatVersion >= 2)
                infoFile.ignore(S_SHORT);

            fileNames[i].next.u = ttdatutil::get_int_be(infoFile, S_SHORT);
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
            fileNames[i].next.s = ttdatutil::get_int(infoFile, S_SHORT);
            fileNames[i].previous = ttdatutil::get_int(infoFile, S_SHORT);
            nameOffset = ttdatutil::get_int(infoFile, S_LONG);
            if (infoType <= -5) infoFile.ignore(S_LONG);
            currOffset = infoFile.tellg();
            if (nameOffset >= 0) {
                infoFile.seekg(fileNamesOffset + nameOffset);
                std::getline(infoFile, fileNames[i].fileName, '\0');
                infoFile.seekg(currOffset);
            } else {
                fileNames[i].fileName = "";
            }
            if (fileNames[i].previous != 0 && i != 0) tmpPath = fileNames[(fileNames[i].previous)].pathName;
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
    
    if (fileCount * sizeof(fileData) > 1073741824) {     // If fileCount would result in massive amounts of memory being allocated (something is wrong)
        errorState = TTDAT_OFFSET_ERROR;                 // This is rudamentary error checking and a new system should be devised
        return;
    }

    fileList = new fileData[fileCount];

    infoFile.seekg(fileOffsOffset);

    if (newFormat) {
        infoType = ttdatutil::get_int_be(infoFile, S_LONG);       // FIXME: The quickbms script has these checks, but in the files I've tested, these values
        fileCount = ttdatutil::get_int_be(infoFile, S_LONG);      // appear to be the same as those at the beginning of the info offset +4 and +8, respectively.
                                                            // This needs More Testing; It's possible that some dat variants use a mix of this version number

        unsigned int offsetOr;
        for (unsigned int i = 0; i < fileCount; i++) {
            if (infoType <= -13) {
                fileList[i].filePacked = ttdatutil::get_int_be(infoFile, S_SHORT);
                infoFile.ignore(S_SHORT);
                fileList[i].fileOffset = ttdatutil::get_int_be(infoFile, S_LONG);
            } else if (infoType <= -11) {
                fileList[i].fileOffset = ttdatutil::get_int_be(infoFile, S_LONGLONG);
            } else {
                fileList[i].fileOffset = ttdatutil::get_int_be(infoFile, S_LONG);
            }

            fileList[i].fileZSize = ttdatutil::get_int_be(infoFile, S_LONG);
            fileList[i].fileSize = ttdatutil::get_int_be(infoFile, S_LONG);

            if (infoType <= -13) {
                fileList[i].filePacked = fileList[i].filePacked ? 2 : 0;
            } else if (infoType <= -10) {
                fileList[i].filePacked = (fileList[i].fileSize >> 31) ? 2 : 0;
                fileList[i].fileSize &= 0x7FFFFFFF;
            } else {
                fileList[i].filePacked = ttdatutil::get_int_be(infoFile, S_BYTE);
                infoFile.ignore(S_SHORT);
                offsetOr = ttdatutil::get_int_be(infoFile, S_BYTE);
                fileList[i].fileOffset |= offsetOr;
            }
        }
    } else {
        /* FIXME: Need to implement fileOffsOffset for old format first */
    }
}

void TTDat::get_crc_size() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

    /*
     * The method used to determine the size of the CRCs requires more research.
     * The following implementation is adapted from the quickbms script and is a little hacky.
     * This is subject to change. Error handling should be implemented at the least.
     */

    int crcs32EndOffset = crcsOffset + (fileCount * 4);
    int crcs64EndOffset = crcsOffset + (fileCount * 8);
    int falsePositiveCheck;

    if (newFormat) {
        falsePositiveCheck = ttdatutil::get_int_be(infoFile, S_LONG, crcs32EndOffset);
    } else {
        falsePositiveCheck = ttdatutil::get_int(infoFile, S_LONG, crcs32EndOffset);
    }

    if (falsePositiveCheck == 0 || falsePositiveCheck == 9){  // 9 for The Force Awakens, otherwise the end of these lists are usually zero
        crc64 = false;
        return;
    }

    if (crcs64EndOffset < (infoOffset + infoSize))
        crc64 = true;

}

void TTDat::get_crcs() {
    std::ifstream& infoFile = ((infoLoc) ? hdrFile : datFile);

}