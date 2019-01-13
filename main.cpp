#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "CoevolutionEngineST.h"
#include "CoevolutionEngineSTMPI.h"
#include "utils.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

#define MODE_MPI

int main(int argc, char* argv[]) {

    std::string args;
    if(argc > 1)
    {
        args = argv[1];
    }

#ifdef MODE_OPEN_MP
        CoevolutionEngineST cov;
        std::function<double(Genotype)> optimizedFunc1;
        std::function<double(Genotype)> optimizedFunc2;
        initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

        bool calculationsPerformed;
        cov.setDesiredError(0.01);
        cov.setNoOfItersWithoutImprov(100);
        omp_set_dynamic(0);
        omp_set_num_threads(1);
        calculationsPerformed = performCalculations(cov, optimizedFunc1, 400, 48, 30, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, 1);
        if(!calculationsPerformed)
            return -1;
    // liczba dzieci podzielna przez 2*liczbe watkow, a wielkosc populacji przez liczbe watkow
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 400, 48, 30, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, 1);
        if(!calculationsPerformed)
            return -1;
    }
#elif defined(MODE_OMP)
    testOMP();

//        CoevolutionEngineST cov;
//        std::function<double(Genotype)> optimizedFunc1;
//        std::function<double(Genotype)> optimizedFunc2;
//        initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);
//
//        int numberOfThreads = 4;
//        bool calculationsPerformed;
//        cov.setDesiredError(0.01);
//        cov.setNoOfItersWithoutImprov(100);
//        omp_set_dynamic(0);
//        omp_set_num_threads(numberOfThreads);
//        calculationsPerformed = performCalculations(cov, optimizedFunc1, 400, 48, 30, -40, 40, CoevolutionEngineST::DESIRED_ERROR, "Function1", 0.3, numberOfThreads);
//        if(!calculationsPerformed)
//            return -1;
//        // Plot line from given x and y data. Color is selected automatically.
////        plt::figure_size(800, 600);
////        // Add graph title
////        plt::title("Algorithm progress for first function");
////        plt::xlabel("Iterations");
////        plt::ylabel("Function value");
////
////        // Set x-axis to interval
////        plt::xlim(-2., cov.x.back());
////        plt::ylim(cov.y.back(), cov.y.front());
////        plt::plot(cov.x, cov.y, "r-");
////        plt::show();
//// liczba dzieci podzielna przez 2*liczbe watkow, a wielkosc populacji przez liczbe watkow
//        calculationsPerformed = performCalculations(cov, optimizedFunc2, 400, 48, 30, -30, 30, CoevolutionEngineST::DESIRED_ERROR, "Function2", 0.3, numberOfThreads);
//        if(!calculationsPerformed)
//            return -1;
//        // Plot line from given x and y data. Color is selected automatically.
//        // Set x-axis to interval
////        plt::title("Algorithm progress for second function");
////        plt::xlabel("Iterations");
////        plt::ylabel("Function value");
////        plt::xlim(cov.x.front(), cov.x.back());
////        plt::ylim(cov.y.back(), cov.y.front());
////        plt::plot(cov.x, cov.y, "r-");
////        plt::show();
    }
#elif defined(MODE_MPI)

    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);
    std::vector<int> sizes = {2,10,20,50,100};
    std::vector<int> pops = {200,200,400,600,600};
    std::vector<int> childs = {32,32,64,96,192};
    std::ostringstream popInitStr;
    MPI_Init(nullptr,nullptr);
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    for(int i = 0; i < sizes.size(); ++i)
    {
        if(rank == 0)
            std::cout << "GenSize= " << sizes[i] << std::endl;
        performCalculationsMPI(optimizedFunc1, pops[i], childs[i], sizes[i], -40, 40,
                               CoevolutionEngineST::DESIRED_ERROR, "Function1", 0.3);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
            std::cout << "GenSize= " << sizes[i] << std::endl;
        performCalculationsMPI(optimizedFunc1, pops[i], childs[i], sizes[i], -30, 30,
                               CoevolutionEngineST::DESIRED_ERROR, "Function2", 0.3);
    }
    MPI_Finalize();
    return 0;
#endif
}