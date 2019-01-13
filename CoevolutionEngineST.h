//
// Created by Damian on 2018-11-15.
//

#ifndef PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H
#define PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H

#include <random>
#include <iostream>
#include <omp.h>
#include "Population.h"

class CoevolutionEngineST {
public:

    enum engineStopCriteria
    {
        NO_OF_ITERS_WITHOUT_IMPROV = 0,
        DESIRED_ERROR
    };

    std::vector<double> x,y;

protected:

    unsigned int mNoOfItersWithoutImprov;
    double mDesiredError, mBestFitError;
    std::default_random_engine mGenerator;
    std::unique_ptr<Population> pCalcPopulation = nullptr;

public:

    CoevolutionEngineST(const double & desiredError = 0.1,const unsigned int & noOfItersWithoutImprov = 20):
            mNoOfItersWithoutImprov(noOfItersWithoutImprov),
            mDesiredError(desiredError)
    {
        std::random_device r;
        mGenerator.seed(r());
        x.clear();
        y.clear();
    };

    bool CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters, const Genotype * bestFit);
    bool setPopulation(const size_t &popSize, const size_t childCnt, const size_t & genSize,
                       const double lowerBound = -1.0, const double upperBound = 1.0);

    bool init(const double & lowerBound, const double & upperBound);

    virtual const Genotype * solve(std::function<double (Genotype)> func, engineStopCriteria criteria, double mutationVariance, int & iterationsCount, int numberOfThreads);

    double getDesiredError() const {return mDesiredError;};
    void setDesiredError(const double & desiredError) { mDesiredError = desiredError;};

    static double getBestFitError(const Genotype *);

    unsigned int getNoOfItersWithoutImprov() const {return mNoOfItersWithoutImprov;};
    void setNoOfItersWithoutImprov(const unsigned int noOfIters) {mNoOfItersWithoutImprov = noOfIters;};

    void printPopulation();
};

#endif //PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H