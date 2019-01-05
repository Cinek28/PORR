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

        int numberOfThreads = 4;
        bool calculationsPerformed;
        cov.setDesiredError(0.01);
        cov.setNoOfItersWithoutImprov(100);
        omp_set_dynamic(0);
        omp_set_num_threads(numberOfThreads);
        calculationsPerformed = performCalculations(cov, optimizedFunc1, 400, 48, 30, -40, 40, CoevolutionEngineST::DESIRED_ERROR, "Function1", 0.3, numberOfThreads);
        if(!calculationsPerformed)
            return -1;
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
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 400, 48, 30, -30, 30, CoevolutionEngineST::DESIRED_ERROR, "Function2", 0.3, numberOfThreads);
        if(!calculationsPerformed)
            return -1;
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