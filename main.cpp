#include <omp.h>
#include <chrono>
#include <iostream>
#include "CoevolutionEngineST.h"

double test_parallel(int num_steps) {
    int i;
    double x, pi, sum = 0.0, step;

    step = 1.0 / (double) num_steps;

#pragma omp parallel for reduction(+:sum) private(x)
    for (i = 1; i <= num_steps; i++) {
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

#pragma omp parallel for reduction(+:sum) private(x)
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
    CoevolutionEngineST coevolutionEngineST = CoevolutionEngineST();

    double zeroVector[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    int f1Result = coevolutionEngineST.calculateF1(zeroVector, 10);
    int f2Result = coevolutionEngineST.calculateF2(zeroVector, 10);

    printf("F1: %lf\n", f1Result);
    printf("F2: %lf\n", f2Result);
}
