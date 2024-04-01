cmake_minimum_required(VERSION 3.5)
project(libttdat_test CXX)

add_executable(libttdat_test)
target_sources(libttdat_test PRIVATE ttdat_test.cpp)

target_link_libraries(libttdat_test PRIVATE ttdat)