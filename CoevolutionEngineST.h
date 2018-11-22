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
public:
    enum engineStopCriteria
    {
        NO_OF_ITERS_WITHOUT_IMPROV = 0,
        DESIRED_ERROR
    };

private:

    unsigned int mNoOfItersWithoutImprov;
    double mDesiredError, mBestFitError;
    std::default_random_engine mGenerator;
    std::unique_ptr<Population> pCalcPopulation = nullptr;

    bool CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters);

public:

    CoevolutionEngineST(const double & desiredError = 0.1,const unsigned int & noOfItersWithoutImprov = 20):
            mNoOfItersWithoutImprov(noOfItersWithoutImprov),
            mDesiredError(desiredError)
    {
        mGenerator.seed(time(NULL));
    };

    bool setPopulation(const size_t &popSize, const size_t childCnt, const size_t & genSize,
                       const double lowerBound = -1.0, const double upperBound = 1.0);

    bool init(const double & lowerBound, const double & upperBound);

    const Genotype * solve(std::function<double (Genotype)> func, engineStopCriteria criteria);

    double getDesiredError() const {return mDesiredError;};
    void setDesiredError(const double & desiredError) { mDesiredError = desiredError;};

    double getBestFitError();

    unsigned int getNoOfItersWithoutImprov() const {return mNoOfItersWithoutImprov;};
    void setNoOfItersWithoutImprov(const unsigned int noOfIters) {mNoOfItersWithoutImprov = noOfIters;};

    void printPopulation();
};

#endif //PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H