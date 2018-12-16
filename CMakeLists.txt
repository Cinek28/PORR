cmake_minimum_required(VERSION 3.12)
project(PORR)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -o3")

add_executable(PORR
        CMakeLists.txt
        CoevolutionEngineST.cpp
        CoevolutionEngineST.h
        main.cpp
        Population.cpp
        Population.h)
