#include <iostream>
#include <cstring>
#include <fstream>

namespace TTDatUtil {

        /**
         * std::ifstream& file : Input file stream
         * unsigned short size : Size of integer in bytes
         * unsigned int offset : Offset in file to read integer from
         * 
         * Gets an integer from the input file of size "size" at offset "offset" and returns it as a signed integer.
         * Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as little endian.
         * 
         * Returns: Signed 32 bit integer
        */
        int get_int(std::ifstream& /*file*/, unsigned short /*size*/, unsigned int /*offset*/);

        /**
         * std::ifstream& file : Input file stream
         * unsigned short size : Size of integer in bytes
         * unsigned int offset : Offset in file to read integer from
         * 
         * Gets an integer from the input file of size "size" at offset "offset" and returns it as a signed integer.
         * Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * Returns: Signed 32 bit integer
        */
        int get_int_be(std::ifstream& /*file*/, unsigned short /*size*/, unsigned int /*offset*/);

        /**
         * std::ifstream& file : Input file stream
         * unsigned short size : Size of integer in bytes
         * 
         * Gets an integer from the input file of size "size" at the current offset and returns it as a signed integer.
         * Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as little endian.
         * 
         * Returns: Signed 32 bit integer
        */
        int get_int(std::ifstream& /*file*/, unsigned short /*size*/);

        /**
         * std::ifstream& file : Input file stream
         * unsigned short size : Size of integer in bytes
         * 
         * Gets an integer from the input file of size "size" at the current offset and returns it as a signed integer.
         * Casting can be done to get the desired integer type (unsigned, signed). This function parses the location
         * as big endian.
         * 
         * Returns: Signed 32 bit integer
        */
        int get_int_be(std::ifstream& /*file*/, unsigned short /*size*/);

        /* 
         * integer : Integer to convert to string
         * 
         * Gets a string from the contents of a 32 bit signed integer location. This gets the
         * little endian representation. Be sure to free the string!
         * 
         * 
         * Returns: A cstring containing the values from the int's memory location 
         */
        char* long_to_str(int);
}