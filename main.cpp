#include <chrono>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "CoevolutionEngineST.h"
#include "utils.h"
//#include "matplotlibcpp.h"

//namespace plt = matplotlibcpp;



int main(int argc, char* argv[]) {

    std::string args;
    if(argc > 1)
    {
        args = argv[1];
    }

    if(!atoi(args.c_str()))
    {
        // Set the size of output image to 1200x780 pixels

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

        // Plot line from given x and y data. Color is selected automatically.
//        plt::figure_size(800, 600);
//        // Add graph title
//        plt::title("Algorithm progress for first function");
//        plt::xlabel("Iterations");
//        plt::ylabel("Function value");
//
//        // Set x-axis to interval
//        plt::xlim(-2., cov.x.back());
//        plt::ylim(cov.y.back(), cov.y.front());
//        plt::plot(cov.x, cov.y, "r-");
//        plt::show();
// liczba dzieci podzielna przez 2*liczbe watkow, a wielkosc populacji przez liczbe watkow

        // Plot line from given x and y data. Color is selected automatically.
        // Set x-axis to interval
//        plt::title("Algorithm progress for second function");
//        plt::xlabel("Iterations");
//        plt::ylabel("Function value");
//        plt::xlim(cov.x.front(), cov.x.back());
//        plt::ylim(cov.y.back(), cov.y.front());
//        plt::plot(cov.x, cov.y, "r-");
//        plt::show();
    }
    else
    {
        return test();
    }
    return 0;
}