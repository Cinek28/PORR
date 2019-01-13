cmake_minimum_required(VERSION 3.1)
project(PORR)

set(CMAKE_CXX_STANDARD 17)


## ------ Cuda
#find_package(CUDA REQUIRED)
#include_directories(${CUDA_INCLUDE_DIRS})
#link_directories(${CUDA_LIBRARY_DIRS})
#set(CUDA_SEPARABLE_COMPILATION ON)
##SET(CUDA_PROPAGATE_HOST_FLAGS OFF)
#set(CUDA_ARCH -gencode arch=compute_35,code=sm_35)
#set(CUDA_NVCC_FLAGS ${CUDA_NVCC_FLAGS} ${CUDA_ARCH} -g -G -Xcompiler)


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(.)

find_package(PythonLibs 2.7)
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -fopenmp -O3")

add_executable(PORR
        CoevolutionEngineSTMPI.cpp
        CoevolutionEngineSTMPI.h
        CoevolutionEngineST.cpp
        CoevolutionEngineST.h
        main.cpp
        Population.cpp
        Population.h
        matplotlibcpp.h
        )


target_include_directories(PORR PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(PORR PRIVATE "${MPI_LIBRARIES}" ${PYTHON_LIBRARIES})