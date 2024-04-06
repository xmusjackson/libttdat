#include "util.hpp"

int ttdatutil::get_int(std::ifstream& file, unsigned short size, unsigned int offset) {
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

int ttdatutil::get_int_be(std::ifstream& file, unsigned short size, unsigned int offset) {
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

int ttdatutil::get_int(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = size - 1; i >= 0; i--)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

int ttdatutil::get_int_be(std::ifstream& file, unsigned short size) {
    unsigned char* bytes = (unsigned char*)malloc(size);
    int int32 = 0;

    file.read((char*)bytes, size);

    for (int i = 0; i <= size - 1; i++)
        int32 = (int32 << 8) + bytes[i];

    free(bytes);
    return int32;
}

char* ttdatutil::long_to_str(int integer) {
    char* intStr = new char[5];
    memcpy(intStr, &integer, 4);
    intStr[4] = '\0';
    
    return intStr;
}

const char* ttdatutil::to_upper(char* string) {
    size_t length = std::strlen(string);

    for (int i{0}; i < length; i++) {
        string[i] = std::toupper(string[i]);
    }

    return string;
}