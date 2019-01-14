#### Parallel and distributed programming

The project is developed for university course completion. The main task of the project is to
learn OpenMP and OpenCL to check differences in computing between sequential and parallel programs.
The project developed using C++ language.


#### Tasks:
Prepare an algorithm in 3 versions:

* sequential version

* paralleled version on shared memory machines

* paralleled version using distributed machines (CPUs with local memory)

#### Dependencies:

* CMake 2.8 or newer 
* OpenMP 4.0 or newer
* MPICH 3.3
* matplotlib-cpp for GUI and plots preparation

#### Building:

## Using cmake and make tools (Linux):

````
git clone --recursive https://github.com/Cinek28/PORR.git
mkdir cmake-build
cd cmake-build
cmake
make -j 4
````

## Using Visual Studio 2017 (Windows):

In order to build project with Visual Studio, GCC compiler needs to be set.
Detailed instruction on how to set compiler for Visual Studio in project settings here:
[Using GCC on Visual Studio 2017](https://blogs.msdn.microsoft.com/vcblog/2017/03/07/use-any-c-compiler-with-visual-studio/)

## 3 versions of program:

In order to compile program to run as sequential, you need to add definition:

````
#define MODE_SEQ
````

For parallel version using OpenMP the definition is:

````
#define MODE_OMP
````

For parallel version using MPI the definition is:

````
#define MODE_MPI
````

## Running MPI version of algorithm:

In order to run MPI version you need to use 'mpirun' in the same folder with compiled source. 
Mpirun is provided with MPICH:

Command syntax:

````
mpirun -np <no_of_processes> ./PORR
````
 

