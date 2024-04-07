#include "crc.hpp"

uint64_t ttdatcrc::crc_fnv_1a_64 (const char* string) {
    uint64_t crc = FNV64_OFFSET_BIAS;
    size_t size = std::strlen(string);
    
    for (int i{0}; char c{string[i]}; i++ ) {
        crc ^= c;
        crc *= FNV64_PRIME;
    }

    return crc;
}

uint32_t ttdatcrc::crc_fnv_1a_32 (const char* string) {
    uint32_t crc = FNV32_OFFSET_BIAS;
    size_t size = std::strlen(string);
    
    for (int i{0}; char c{string[i]}; i++ ) {
        crc ^= c;
        crc *= FNV32_PRIME;
    }

    return crc;
}