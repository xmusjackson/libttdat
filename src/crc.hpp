#include <iostream>
#include <cstring>
#include <cstdint>

#define FNV64_OFFSET_BIAS 0xCBF29CE484222325
#define FNV64_PRIME 0x100000001B3

#define FNV32_OFFSET_BIAS 0x811c9dc5
#define FNV32_PRIME 0x199933  // This is not actually a prime, but appears to be what is used to CRC the names

namespace ttdatcrc {

    /**
     * @param string Cstring to be hashed
     * 
     * @brief Uses the Fowler/Noll/Vo 1A algorithm to generate a 64-bit hash from a given cstring.
     * @return Unsigned 64-bit integer
    */
    uint64_t crc_fnv_1a_64 (const char* string);

    /**
     * @param string Cstring to be hashed
     * 
     * @brief Uses the Fowler/Noll/Vo 1A algorithm to generate a 32-bit hash from a given cstring.
     * @return Unsigned 32-bit integer
     * @note The prime used in this function is not actually a prime (0x199933) but appears to be the number used internally to create the crcs in tt games dat files.
    */
    uint32_t crc_fnv_1a_32 (const char* string);
    
}
