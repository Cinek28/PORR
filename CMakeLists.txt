cmake_minimum_required(VERSION 3.1)
project(PORR)

set(CMAKE_CXX_STANDARD 14)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Charts REQUIRED)

include_directories(.)


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -O2")

add_executable(PORR
        CoevolutionEngineST.cpp
        CoevolutionEngineST.h
        main.cpp
        Population.cpp
        Population.h
        Widget.cpp
        Widget.h)

target_link_libraries( PORR Qt5::Widgets Qt5::Charts)
