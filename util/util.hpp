#include <iostream>
#include <algorithm>
#include <cstring>

/* Get a string from the contents of a u_int32_t location. This gets the
   little endian representation.
   Returns: A cstring containing the values from the int's memory location
*/
char* longToStr(u_int32_t);

/*
 Unused for now, but I had a reason for this in mind at one point 
 Speaks for itself, converts a string to an upper. There's probably 
 a faster way to do this, but until this function gets used a lot, 
 this will work fine.

 Returns: Uppercase String
*/
std::string toUpper(std::string&);