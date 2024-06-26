#include "util.hpp"

int64_t ttdatutil::get_int(std::ifstream& file, unsigned short size, unsigned int offset) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    uint64_t int64 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, 4);

    for (int i = size - 1; i >= 0; i--)
        int64 = (int64 << 8) | static_cast<unsigned int>(bytes[i]);

    free(bytes);
    return int64;
}

int64_t ttdatutil::get_int_be(std::ifstream& file, unsigned short size, unsigned int offset) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    uint64_t int64 = 0;

    file.clear();
    file.seekg(offset, file.beg);
    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int64 = (int64 << 8) | static_cast<unsigned int>(bytes[i]);

    free(bytes);
    return int64;
}

int64_t ttdatutil::get_int(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    uint64_t int64 = 0;

    file.read((char*)bytes, size);

    for (int i = size - 1; i >= 0; i--)
        int64 = (int64 << 8) | static_cast<unsigned int>(bytes[i]);

    free(bytes);
    return int64;
}

int64_t ttdatutil::get_int_be(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    uint64_t int64 = 0;

    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int64 = (int64 << 8) | static_cast<unsigned int>(bytes[i]);

    free(bytes);
    return int64;
}

char* ttdatutil::long_to_str(int integer) {
    char* intStr = new char[5];
    memcpy(intStr, &integer, 4);
    intStr[4] = '\0';
    
    return intStr;
}

std::string ttdatutil::to_upper(std::string str) {
    size_t length = str.length();

    for (int i{0}; i < length; i++) {
        str[i] = std::toupper(str[i]);
    }

    return str;
}