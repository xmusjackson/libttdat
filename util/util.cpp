#include "util.hpp"

char* longToStr(u_int32_t integer) {
    char* intStr = (char*)malloc(5);
    strlcpy(intStr, (char*)&integer, 5);
    
    return intStr;
}

std::string toUpper(std::string& string) {
    std::transform(string.begin(), string.end(), string.begin(), ::toupper);

    return string;
}