//
// Created by Damian on 2018-11-15.
//

#ifndef PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H
#define PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H

#define F1_DESIRED_VALUE 0
#define F2_DESIRED_VALUE 0

#include <random>
#include <iostream>
#include "Population.h"

class CoevolutionEngineST {

private:

    unsigned int mNoOfItersWithoutImprov = 20;
    double mDesiredError = 0.1;
    std::default_random_engine mGenerator;
    std::unique_ptr<Population> pCalcPopulation = nullptr;

public:

    enum engineStopCriteria
    {
        NO_OF_ITERS_WITHOUT_IMPROV = 0,
        DESIRED_ERROR
    };

    CoevolutionEngineST(const double & desiredError = 0.1, unsigned int noOfItersWithoutImprov = 20);

    bool setPopulation(const size_t &popSize, const size_t childCnt, const size_t & genSize,
                       const double lowerBound = -1.0, const double upperBound = 1.0);

    bool init(const double & lowerBound, const double & upperBound);

    bool solve(std::function<void (Genotype, Genotype)> func, engineStopCriteria criteria);

    double CalculateF1(double *x);
    double CalculateF2(double *x);

    void CalculateFitnessF1(Population population);
    void CalculateFitnessF2(Population population);

    bool CheckTerminationCriteria();

};

#endif //PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H