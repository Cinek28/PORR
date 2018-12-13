#include <chrono>
#include <iostream>
#include <assert.h>
#include "CoevolutionEngineST.h"
#include <fstream>
#include <iomanip>
#include <sstream>
#include <ctime>

# define NUMBER_THREADS 10

void initializeOptimizationFunctions(std::function<double(Genotype)> &optimizedFunc1,
                                     std::function<double(Genotype)> &optimizedFunc2);

void write_text_to_log_file(const std::string &text);

double test_parallel(int num_steps) {
    int i;
    double x, pi, sum = 0.0, step;

    step = 1.0 / (double) num_steps;

#pragma omp parallel for reduction(+:sum) private(x)
    for (i = 1; i <= num_steps; i++) {
        assert( omp_get_num_threads() == NUMBER_THREADS );
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

    int n_thread = NUMBER_THREADS;   // number of threads in parallel regions
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


int main(int argc, char* argv[]) {

    //initialTest(argc, argv);
    CoevolutionEngineST cov;
    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

    cov.setPopulation(100,20,1, -40, 40);
    double startF1 = omp_get_wtime( );
    const Genotype* gen1 = cov.solve(optimizedFunc1, CoevolutionEngineST::engineStopCriteria::NO_OF_ITERS_WITHOUT_IMPROV);
    double endF1 = omp_get_wtime( );
    std::cout << "Solution: " << std::endl;
    for(unsigned int i = 0; i < gen1->size(); ++i)
    {
        std::cout << gen1->at(i).first << ", ";
    }
    std::cout << "\nExecution Time: " << endF1-startF1 << " [s]" << std::endl;


    cov.setPopulation(500,80,2, -40, 40);
    cov.setNoOfItersWithoutImprov(500);
    double startF2 = omp_get_wtime( );
    const Genotype* gen2 = cov.solve(optimizedFunc2, CoevolutionEngineST::engineStopCriteria::NO_OF_ITERS_WITHOUT_IMPROV);
    double endF2 = omp_get_wtime( );
    std::cout << "Solution: " << std::endl;
    for(unsigned int i = 0; i < gen2->size(); ++i)
    {
        std::cout << gen2->at(i).first << ", ";
    }
    std::cout << "\nExecution Time: " << endF2-startF2 << " [s]" << std::endl;

    return 0;
}

void initializeOptimizationFunctions(std::function<double(Genotype)> &optimizedFunc1,
                                     std::function<double(Genotype)> &optimizedFunc2) {
    optimizedFunc1= [](Genotype genotype)
        {
            double sum = 0, prod = 1.0;
            //TODO: Could be parallel, better not, for larger genotypes whole method should be in separate threads
            for(unsigned int i = 0; i < genotype.size();++i)
            {
                sum += pow(genotype[i].first,2);
                prod *= cos(genotype[i].first/(i+1));
            }
            return 1./40.0*sum+1-prod;
        };
    optimizedFunc2= [](Genotype genotype)
    {
        double ex2 = 0, ecos2px = 0;
        //TODO: Could be parallel, better not, for larger genotypes whole method should be in separate threads
        for(unsigned int i = 0; i < genotype.size();++i)
        {
            ex2 += pow(genotype[i].first,2);
            ecos2px += cos(2 * M_PI * genotype[i].first);
        }
        return 20 * exp(-0.2 * sqrt(ex2 / genotype.size())) - exp(ecos2px / genotype.size()) + 20 + M_E;
    };
}

void write_text_to_log_file( const std::string &text )
{
    time_t _tm =time(NULL );
    struct tm * curtime = localtime ( &_tm );
    auto datetimeNow = asctime(curtime);
    std::ofstream log_file(
            "../results/log_file.txt", std::ios_base::out | std::ios_base::app );
    log_file << datetimeNow << text << std::endl;

}