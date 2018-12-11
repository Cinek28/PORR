//
// Created by Damian on 2018-11-15.
//

#include "CoevolutionEngineST.h"

#define MUTATION_VARIANCE 0.1
#define CROSSING_OVER_PERCENTAGE 0.25


bool CoevolutionEngineST::setPopulation(const size_t &popSize, const size_t childCnt, const size_t &genSize,
                                        const double lowerBound, const double upperBound)
{
    if(popSize < childCnt || lowerBound > upperBound)
        return false;

    pCalcPopulation.reset(new Population(popSize, childCnt,genSize));
    return init(lowerBound, upperBound);
}

bool CoevolutionEngineST::init(const double &lowerBound, const double &upperBound)
{
    if(lowerBound > upperBound || pCalcPopulation == nullptr)
    {
        std::cout << "Parameters initiated incorrectly (lower bound > upper bound or population not set" << std::endl;
        return false;
    }
    pCalcPopulation->init(lowerBound, upperBound);
    std::cout << "Population initiated correctly. Lower bound: " << lowerBound << " upper bound: "
              << upperBound << std::endl;
    return true;
}

const Genotype * CoevolutionEngineST::solve(std::function<double(Genotype)> func, engineStopCriteria criteria)
{
    if(pCalcPopulation == nullptr)
    {
        std::cout << "Population not set" << std::endl;
        return nullptr;
    }
    unsigned int iters = 0;

    mBestFitError = getBestFitError();

    std::cout << "Starting optimization. Iter 0." << std::endl;

    while(!CheckTerminationCriteria(criteria, iters))
    {
//        std::cout << "Iters without improvement: " << iters << " Best fit error: " << mBestFitError <<  std::endl;
        pCalcPopulation->cross(CROSSING_OVER_PERCENTAGE, mGenerator);
        pCalcPopulation->mutate(MUTATION_VARIANCE, mGenerator);
        pCalcPopulation->getBestFit(func);
//        printPopulation();
    }

    std::cout << "Iters without improvement: " << iters << " Best fit error: " << mBestFitError <<  std::endl;
    return pCalcPopulation->at(0);
}


bool CoevolutionEngineST::CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters)
{
    double thisIterBestError = getBestFitError();

    if(criteria == DESIRED_ERROR)
    {
        if(thisIterBestError < mDesiredError)
        {
            mBestFitError = thisIterBestError;
            return true;
        }
    }
    else if(criteria == NO_OF_ITERS_WITHOUT_IMPROV)
    {
        if(iters > mNoOfItersWithoutImprov)
        {
            return true;
        }
    }
    if(mBestFitError <= thisIterBestError)
        ++iters;
    else
        iters = 0;

    mBestFitError = thisIterBestError;
    return false;
}

double CoevolutionEngineST::getBestFitError()
{   //Calculating error ONLY (!!) where function minimum is point (0,0,0,...)
    //Fit error calculated as MEAN SQUARE ERROR of all values:
    double bestFitError = 0;
    for(unsigned int i = 0; i < pCalcPopulation->at(0)->size();++i)
    {
        bestFitError += pow(((pCalcPopulation->at(0))->at(i)).first,2)/pCalcPopulation->at(0)->size();
    }
    return bestFitError;
}

void CoevolutionEngineST::printPopulation()
{
    pCalcPopulation->printPopulation();
}