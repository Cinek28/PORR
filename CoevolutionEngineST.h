//
// Created by Damian on 2018-11-15.
//

#ifndef PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H
#define PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H

#include <random>
#include <iostream>
#include <omp.h>
#include "Population.h"

#define populationCnt 5

class CoevolutionEngineST {
public:

    enum engineStopCriteria
    {
        NO_OF_ITERS_WITHOUT_IMPROV = 0,
        DESIRED_ERROR
    };

    std::string enum2cChar[2] = {
            "NO_OF_ITERS_WITHOUT_IMPROV",
            "DESIRED_ERROR",
    };

private:

    unsigned int mNoOfItersWithoutImprov[populationCnt] = {0};
    double mDesiredError;
    double mBestFitError[populationCnt];
    std::default_random_engine mGenerator;
    std::unique_ptr<Population> pCalcPopulation[populationCnt] = {nullptr};
    

    bool CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters, bool ompOn, int populationIter);

public:

    CoevolutionEngineST(const double & desiredError = 0.1,const unsigned int & noOfItersWithoutImprov = 20):
            mDesiredError(desiredError)
    {
        std::random_device r;
        mGenerator.seed(r());
    };

    bool setPopulation(const size_t &popSize, const size_t childCnt, const size_t & genSize,
                       const double lowerBound = -1.0, const double upperBound = 1.0);

    bool init(const double & lowerBound, const double & upperBound);

    const Genotype * solve(std::function<double (Genotype)> func, engineStopCriteria criteria, double mutationVariance, int & iterationsCount, bool ompOn);

    double getDesiredError() const {return mDesiredError;};
    void setDesiredError(const double & desiredError) { mDesiredError = desiredError;};

    double getBestFitError(bool ompOn, int populationIter);

    void printPopulation();
};

#endif //PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H