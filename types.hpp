#include <iostream>

enum loc {
    DAT = 0,
    HDR = 1
};

enum TTDatError {
    NO_ERROR = 0,
    FILE_ERROR = 1,
    OFFSET_ERROR = 2,
    GENERAL_ERROR = 255
};

struct fileData {
    std::string fileName;
    unsigned long long crc;
    unsigned int offset;
    unsigned int nameOffset;
    unsigned int fileSize;
    unsigned short fileID;
    unsigned short pathID;
};