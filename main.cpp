#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "CoevolutionEngineST.h"
#include "utils.h"
#include "matplotlibcpp.h"

namespace plt = matplotlibcpp;

#define MODE_SEQ

int main(int argc, char* argv[]) {

    std::string args;
    if(argc > 1)
    {
        args = argv[1];
    }

#ifdef MODE_SEQ
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
#elif defined(MODE_OMP)
    CoevolutionEngineST cov;
    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

    int numberOfThreads = 8;
    bool calculationsPerformed;
    cov.setDesiredError(0.05);
    cov.setNoOfItersWithoutImprov(100);
    omp_set_dynamic(0);

    for(int threads=1;threads <= 12; threads++){
        if(!(threads == 1 || threads == 4 || threads == 8 || threads == 12))
            continue;
        omp_set_num_threads(threads);
        cov.setNoOfItersWithoutImprov(100);

        calculationsPerformed = performCalculations(cov, optimizedFunc1, 200, 32, 2, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 200, 32, 2, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, threads);
        if(!calculationsPerformed)
            return -1;

        calculationsPerformed = performCalculations(cov, optimizedFunc1, 200, 32, 10, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 200, 32, 10, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, threads);
        if(!calculationsPerformed)
            return -1;

        cov.setNoOfItersWithoutImprov(80);

        calculationsPerformed = performCalculations(cov, optimizedFunc1, 400, 64, 20, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 400, 64, 20, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, threads);
        if(!calculationsPerformed)
            return -1;

        cov.setNoOfItersWithoutImprov(65);

        calculationsPerformed = performCalculations(cov, optimizedFunc1, 600, 96, 50, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 600, 96, 50, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, threads);
        if(!calculationsPerformed)
            return -1;

        cov.setNoOfItersWithoutImprov(50);

        calculationsPerformed = performCalculations(cov, optimizedFunc1, 1200, 192, 100, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 1200, 192, 100, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, threads);
        if(!calculationsPerformed)
            return -1;
    }

#elif defined(MODE_MPI)

    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);
    std::vector<int> sizes = {2,10,20,50,100};
    std::vector<int> pops = {200,200,400,600,1200};
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
                               CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3);

        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
            std::cout << "GenSize= " << sizes[i] << std::endl;
        performCalculationsMPI(optimizedFunc1, pops[i], childs[i], sizes[i], -30, 30,
                               CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3);
    }
    MPI_Finalize();
#endif
    return 0;
}