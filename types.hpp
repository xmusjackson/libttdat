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
    std::string crc;
    std::string fileName;
    std::string filePath;
    int offset;
    int fileSize;
    int fileID;
};
