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

        std::function<double(Genotype)> optimizedFunc1;
        std::function<double(Genotype)> optimizedFunc2;
        initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

        int threadCountArr[] = { 8, 4, 1 };
        int generationSizeArr[] = { 2, 10, 20, 50, 100 };
        int populationSizeArr[] = { 400, 400, 400, 600, 1200 };
        int childrenCountArr[] = { 48, 48, 48, 64, 144 };
        for (int func = 0; func < 2; ++func) {
            std::string functionName = func == 0 ? "Function1" : "Function2";
            for (int p = 0; p < 3; ++p) {
                int numberOfThreads = threadCountArr[p];
                for (int j = 0; j < 5; ++j) {
                    CoevolutionEngineST cov;

                    int populationSize = populationSizeArr[j];
                    int childrenCount = childrenCountArr[j];
                    int generationSize = generationSizeArr[j];
                    Results * results = new Results(populationSize, childrenCount, generationSize, numberOfThreads);
                    cov.setDesiredError(0.1);
                    cov.setNoOfItersWithoutImprov(100);
                    omp_set_dynamic(0);
                    omp_set_num_threads(numberOfThreads);
                    for (int i = 0; i < 5; ++i) {
                        Result * result;
                        if (func == 0){
                            result = performCalculations(cov, optimizedFunc1, populationSize, childrenCount, generationSize, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, functionName, 0.3, numberOfThreads);
                        }
                        else {
                            result = performCalculations(cov, optimizedFunc2, populationSize, childrenCount, generationSize, -30, 30, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, functionName, 0.3, numberOfThreads);
                        }
                        results->avgExecutionTime += result->executionTime;
                        if (result->executionTime > results->maxExecutionTime)
                            results->maxExecutionTime = result->executionTime;
                        if (results->minExecutionTime > result->executionTime)
                            results->minExecutionTime = result->executionTime;
                        results->avgResult += result->result;
                        if (result->result > results->maxResult)
                            results->maxResult = result->result;
                        if (results->minResult > result->result)
                            results->minResult = result->result;
                        if(!result->success)
                            return -1;
                    }
                    std::ostringstream resultsStr;
                    resultsStr << functionName << ", no_of_iter: 100" << "\n";
                    resultsStr << "Population size: " << results->populationSize << "\n";
                    resultsStr << "Children count: " << results->childrenCount << "\n";
                    resultsStr << "Generation size: " << results->generationSize << "\n";
                    resultsStr << "Number of threads: " << results->noOfThreads << "\n";
                    resultsStr << "Average Execution Time: " << (results->avgExecutionTime / 5) << "[s]\n";
                    resultsStr << "Min Execution Time: " << results->minExecutionTime << "[s]\n";
                    resultsStr << "Max Execution Time: " << results->maxExecutionTime << "[s]\n";
                    resultsStr << "Average function value: " << (results->avgResult / 5) << "\n";
                    resultsStr << "Min function value: " << results->minResult << "\n";
                    resultsStr << "Max function value: " << results->maxResult << "\n";
                    std::cout << resultsStr.str();
                    write_text_to_log_file(resultsStr.str());
                }
            }
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