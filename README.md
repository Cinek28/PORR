##### Parallel and distributed programming

The project is developed for university course completion. The main task of the project is to
learn OpenMP and OpenCL to check differences in computing between sequential and parallel programs.
The project developed using C++ language.


#### Tasks:
Prepare an algorithm in 3 versions:

* sequential version

* paralleled version on shared memory machines

* paralleled version using GPUs

* paralleled version using distributed machines (CPUs with local memory)

#### Dependencies:

* CMake 2.8 or newer 
* OpenMP 4.0 or newer
* OpenCL (version not specified yet)
* Qt5 for GUI and plots preparation (to be determined)

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

