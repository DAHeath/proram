cmake_minimum_required (VERSION 3.10)
project (PrORAM)

set(CMAKE_BUILD_TYPE Release)

add_definitions(-Wfatal-errors)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-pthread -Wall -march=native -O3 -maes -mrdseed -DNDEBUG")


file(GLOB sources "src/*.cc")


add_executable(PrORAM ${sources})
target_include_directories(PrORAM PUBLIC src)
