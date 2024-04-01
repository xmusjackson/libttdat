# These CMakeLists need a lot of work. They will improve as I learn more about cmake
cmake_minimum_required(VERSION 3.5)
project(libttdat CXX)

set(CMAKE_CXX_STANDARD 20)

set(LIBTTDAT_INSTALL_INCLUDE_DIR ${PROJECT_SOURCE_DIR}/include)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

if(!WIN32)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++23 -c -O2 -Werror -Wall -Wextra -Wpedantic -Wshadow")
endif()

include_directories(${LIBTTDAT_INSTALL_INCLUDE_DIR})

add_subdirectory(test)
add_subdirectory(src)