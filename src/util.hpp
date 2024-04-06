#include <iostream>
#include <cstring>
#include <fstream>

namespace ttdatutil {

        /**
         * @brief Gets a little endian integer from the input file at a specified offset
         * @param file Input file stream
         * @param size Size of integer in bytes
         * @param offset Offset at which to read from
         * 
         * @note Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * @return Signed 32 bit integer
        */
        int get_int(std::ifstream& file, unsigned short size, unsigned int offset);

        /**
         * @brief Gets a big endian integer from the input file at a specified offset
         * @param file Input file stream
         * @param size Size of integer in bytes
         * @param offset Offset at which to read from
         * 
         * @note Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * @return Signed 32 bit integer
        */
        int get_int_be(std::ifstream& file, unsigned short size, unsigned int offset);

        /**
         * @brief Gets a little endian integer from the input file at the current offset
         * @param file Input file stream
         * @param size Size of integer in bytes
         * 
         * @note Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * @return Signed 32 bit integer
        */
        int get_int(std::ifstream& file, unsigned short size);

        /**
         * @brief Gets a big endian integer from the input file at the current offset
         * @param file Input file stream
         * @param size Size of integer in bytes
         * 
         * @note Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * @return Signed 32 bit integer
        */
        int get_int_be(std::ifstream& file, unsigned short size);

        /** 
         * @param integer Integer to convert to string
         * 
         * @brief Creates a string from the contents of a 32 bit integer location. 
         * @note This gets the little endian representation. Be sure to delete[] the string!
         *
         * @return A null terminated cstring 
         */
        char* long_to_str(int integer);

        /**
         * @param string String to convert to uppercase
         * 
         * @brief Converts a cstring to uppercase
         * @note This function uses std::toupper() and modifies the provided string
         * 
         * @return A pointer to the uppercase string
        */
        const char* to_upper(char* string);
}