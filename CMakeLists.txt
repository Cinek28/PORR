cmake_minimum_required (VERSION 2.6)
cmake_policy(SET CMP0012 NEW)
project (Parallel_computing)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=c++0x -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -Wall")

if(Debug)
  message("Debug is on")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Og")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Og")
else()
  message("Release is on")
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0")
endif(Debug)



find_package(OpenMP)
if(OPENMP_FOUND)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif(OPENMP_FOUND)
add_executable(Parallel_computing main.cpp CoevolutionEngineST.cpp CoevolutionEngineST.h)
