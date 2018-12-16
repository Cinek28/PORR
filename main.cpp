#include <chrono>
#include <iostream>
#include <assert.h>
#include "CoevolutionEngineST.h"
#include <fstream>
#include <iomanip>
#include <cstring>

void initializeOptimizationFunctions(std::function<double(Genotype)> &optimizedFunc1,
                                     std::function<double(Genotype)> &optimizedFunc2) {
    optimizedFunc1= [](Genotype genotype)
    {
        double sum = 0, prod = 1.0;//TODO simd
        for(unsigned int i = 0; i < genotype.size();++i)
        {
            sum += pow(genotype[i].first,2);
            prod *= cos(genotype[i].first/(i+1));
        }
        return 1./40.0*sum+1-prod;
    };
    optimizedFunc2= [](Genotype genotype)
    {
        double ex2 = 0, ecos2px = 0;//TODO simd
        for(unsigned int i = 0; i < genotype.size();++i)
        {
            ex2 += pow(genotype[i].first,2);
            ecos2px += cos(2 * M_PI * genotype[i].first);
        }
        return -20 * exp(-0.2 * sqrt(ex2 / genotype.size())) - exp(ecos2px / genotype.size()) + 20 + M_E;
    };
}

void write_text_to_log_file( const std::string &text )
{
    time_t _tm =time(NULL );
    struct tm * curtime = localtime ( &_tm );
    auto datetimeNow = asctime(curtime);
    datetimeNow[strlen(datetimeNow) - 1] = 0;
    std::ofstream log_file(
            "../log_file.log", std::ios_base::out | std::ios_base::app );
    if(log_file.is_open())
    {
        log_file << datetimeNow << " | " << text << std::endl;
        log_file.close();
    }
    else
    {
        std::cout << "Log file not open! Cannot write logs to file." << std::endl;
    }
}

double test_parallel(int num_steps) {
    int i;
    double x, pi, sum = 0.0, step;

    step = 1.0 / (double) num_steps;

#pragma omp parallel for reduction(+:sum) private(x)
    for (i = 1; i <= num_steps; i++) {
        assert( omp_get_num_threads() == 10 );
        x = (i - 0.5) * step;
        sum = sum + 4.0 / (1.0 + x*x);
    }

    pi = step * sum;
    return pi;
}

double test_sequential(int num_steps) {
    int i;
    double x, pi, sum = 0.0, step;

    step = 1.0 / (double) num_steps;

    for (i = 1; i <= num_steps; i++) {
        x = (i - 0.5) * step;
        sum = sum + 4.0 / (1.0 + x*x);
    }

    pi = step * sum;
    return pi;
}

void initialTest(int argc, char* argv[]) {
    double   d;
    int n = 1000000;

    auto begin = std::chrono::_V2::system_clock::now();

    if (argc > 1)
        n = atoi(argv[1]);

    int n_thread = 10;   // number of threads in parallel regions
    omp_set_dynamic(0);              // off dynamic thread adjust
    omp_set_num_threads(n_thread);   // set the number of threads

    d = test_parallel(n);
    auto end = std::chrono::_V2::system_clock::now();
    std::cout << "For " << n << " steps, pi = " << d << ", "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " milliseconds" << std::endl;

    d = test_sequential(n);
    end = std::chrono::_V2::system_clock::now();
    std::cout << "For " << n << " steps, pi = " << d << ", "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " milliseconds" << std::endl;
}

bool performCalculations(CoevolutionEngineST &cov, const std::function<double(Genotype)> &optimizedFunc, int popSize,
                         int childCnt, int genSize, int lowerBound, int upperBound,
                         CoevolutionEngineST::engineStopCriteria criteria, std::string optimizedFuncName, double mutationVariance) {

    std::ostringstream popInitStr;
    bool populationSet = cov.setPopulation(popSize, childCnt, genSize, lowerBound, upperBound);
    if(!populationSet){
        popInitStr << "Population initialization error!";
        write_text_to_log_file(popInitStr.str());
        return false;
    }

    popInitStr << "Population initialized with params:"
               << " popSize: " << popSize
               << ", childCnt: " << childCnt
               << ", genSize: " << genSize
               << ", lowerBound: " << lowerBound
               << ", upperBound: " << upperBound
               << ", function: " << optimizedFuncName
               << ", criteria: " << cov.enum2cChar[criteria];
    write_text_to_log_file(popInitStr.str());
    double startF1 = omp_get_wtime( );
    const Genotype* gen1 = cov.solve(optimizedFunc, criteria, mutationVariance);
    double endF1 = omp_get_wtime( );
    std::ostringstream popResultStr, popExecTimeStr;
    std::cout << "Solution: " << std::endl;
    popResultStr << "Solution: ";
    for(unsigned int i = 0; i < gen1->size(); ++i)
    {
        std::cout << gen1->at(i).first << ", ";
        popResultStr << gen1->at(i).first << ", ";
    }
    write_text_to_log_file(popResultStr.str());
    std::cout << "\nExecution Time: " << endF1 - startF1 << " [s]" << std::endl;
    popExecTimeStr << "Execution Time: " << endF1-startF1 << " [s]";
    write_text_to_log_file(popExecTimeStr.str());
    return true;
}

int main(int argc, char* argv[]) {

    //initialTest(argc, argv);
    CoevolutionEngineST cov;
    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

//    std::vector<std::pair<double,double>> vect = std::vector<std::pair<double,double>>();
//    for(int i=0;i<10;i++)
//        vect.push_back({30,0});
//
//    double F2Result = optimizedFunc2(vect);
//    double F1Result = optimizedFunc1(vect);

    bool calculationsPerformed;
    cov.setDesiredError(0.1);
    cov.setNoOfItersWithoutImprov(300);
    calculationsPerformed = performCalculations(cov, optimizedFunc1, 30, 12, 3, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3);
    if(!calculationsPerformed)
        return -1;
    cov.setDesiredError(0.1);
    cov.setNoOfItersWithoutImprov(300);
    calculationsPerformed = performCalculations(cov, optimizedFunc2, 30, 12, 3, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3);
    if(!calculationsPerformed)
        return -1;
    return 0;
}