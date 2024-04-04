#include <iostream>

#include "win32.hpp"

enum LIBTTDAT_API loc {
    DAT = 0,
    HDR = 1
};

enum LIBTTDAT_API TTDatError {
    TTDAT_NO_ERROR = 0,
    TTDAT_FILE_ERROR = 1,
    TTDAT_OFFSET_ERROR = 2,
    TTDAT_GENERAL_ERROR = 255
};

struct LIBTTDAT_API fileData {
    std::string fileName;
    unsigned long nameCrc;
    unsigned int nameOffset;
    unsigned int fileOffset;
    unsigned int fileSize;
    unsigned int fileZSize;
    unsigned int filePacked;
};

struct LIBTTDAT_API fileName {
    std::string fileName;
    std::string pathName;
    unsigned short previous;
    union {
        unsigned int u;
        short s;
        } next;
};