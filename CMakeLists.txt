cmake_minimum_required(VERSION 3.13)
project(AlgorithmsCPP)

set(CMAKE_CXX_STANDARD 17)

file(GLOB SOURCE_FILES *.cpp */*.cpp)
add_executable(AlgorithmsCPP ${SOURCE_FILES})