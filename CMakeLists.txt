# set the minimum cmake version
cmake_minimum_required(VERSION 3.0)

# set the project name
project(moviedb VERSION 1.0)

# set C++17 standard
set(CMAKE_CXX_STANDARD 17)

# libraries
include_directories(src/include/)

# sources
file(GLOB SOURCES src/Main.cpp src/cpp/*.cpp)

# specify executable
add_executable(moviedb ${SOURCES})
