#include <chrono>
#include <iostream>
#include <assert.h>
#include "CoevolutionEngineST.h"

# define NUMBER_THREADS 10

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

    cov.setPopulation(100,20,1, -40, 40);

    std::function<double (Genotype)> optimizedFunc1 = [](Genotype genotype)
        {
            double sum = 0;
            double prod = 1.0;
            //TODO: Could be parallel
            for(unsigned int i = 0; i < genotype.size();++i)
            {
                sum += pow(genotype[i].first,2);
                prod *= cos(genotype[i].first/(i+1));
            }
            return 1./40.0*sum+1-prod;
        };

    std::function<double (Genotype)> optimizedFunc2 = [](Genotype genotype)
    {
        double sum = 0;
        //TODO: Could be parallel
        for(unsigned int i = 0; i < genotype.size();++i)
        {
            sum += pow(genotype[i].first,2);
        }
        return sum;
    };

    const Genotype* gen1 = cov.solve(optimizedFunc1, CoevolutionEngineST::engineStopCriteria::NO_OF_ITERS_WITHOUT_IMPROV);
    std::cout << "Solution: " << std::endl;
    for(unsigned int i = 0; i < gen1->size(); ++i)
    {
        std::cout << gen1->at(i).first << " ";
    }

    cov.setPopulation(500,80,2, -40, 40);
    cov.setNoOfItersWithoutImprov(500);

    const Genotype* gen2 = cov.solve(optimizedFunc2, CoevolutionEngineST::engineStopCriteria::NO_OF_ITERS_WITHOUT_IMPROV);

    std::cout << "Solution: " << std::endl;
    for(unsigned int i = 0; i < gen2->size(); ++i)
    {
        std::cout << gen2->at(i).first << ", ";
    }

    return 0;
}
