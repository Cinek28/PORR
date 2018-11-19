#include <omp.h>
#include <chrono>
#include <iostream>

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

    for (i = 1; i <= num_steps; i++) {
        x = (i - 0.5) * step;
        sum = sum + 4.0 / (1.0 + x*x);
    }

    pi = step * sum;
    return pi;
}

int main(int argc, char* argv[]) {
    double   d;
    int n = 1000000;

    auto begin = std::chrono::high_resolution_clock::now();

    if (argc > 1)
        n = std::atoi(argv[1]);

    d = test_parallel(n);
    auto end = std::chrono::high_resolution_clock::now();
    std::cout << "For " << n << " steps, pi = "<< d << ", "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " milliseconds" << std::endl;

    d = test_sequential(n);
    end = std::chrono::high_resolution_clock::now();
    std::cout << "For " << n << " steps, pi = "<< d << ", "
              << std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count()
              << " milliseconds" << std::endl;
}
