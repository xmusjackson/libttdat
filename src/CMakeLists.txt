cmake_minimum_required(VERSION 3.5)
project(ttdat CXX)

set(SOURCE_FILES crc.cpp util.cpp ttdat.cpp)

if(WIN32)
    add_subdirectory(win32)
endif()

add_library(ttdat SHARED ${SOURCE_FILES})

if(WIN32)
    target_compile_definitions(ttdat PRIVATE LIBTTDAT_EXPORTS)
endif()