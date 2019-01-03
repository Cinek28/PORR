cmake_minimum_required(VERSION 3.1)
project(PORR)

set(CMAKE_CXX_STANDARD 17)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

include_directories(.)

find_package(PythonLibs 2.7)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -O2")

add_executable(PORR
        CoevolutionEngineST.cpp
        CoevolutionEngineST.h
        main.cpp
        Population.cpp
        Population.h
        matplotlibcpp.h
        )

#target_include_directories(PORR PRIVATE ${PYTHON_INCLUDE_DIRS})
#target_link_libraries(PORR ${PYTHON_LIBRARIES})