//
// Created by Damian on 2018-11-15.
//

#include <algorithm>
#include "CoevolutionEngineST.h"

bool CoevolutionEngineST::setPopulation(const size_t &popSize, const size_t childCnt, const size_t &genSize,
                                        const double lowerBound, const double upperBound)
{
    if(popSize < childCnt || lowerBound > upperBound || (childCnt%2 != 0))
        return false;

    pCalcPopulation.reset(new Population(popSize, childCnt,genSize));
    x.clear();
    y.clear();
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

const Genotype * CoevolutionEngineST::solve(std::function<double(Genotype)> func, engineStopCriteria criteria, double mutationVariance, int & iterationsCount, int numberOfThreads)
{
    if(pCalcPopulation == nullptr)
    {
        std::cout << "Population not set" << std::endl;
        return nullptr;
    }
    unsigned int iters = 0, iterationsCounter = 0;

    Genotype * bestFits[numberOfThreads];
    bestFits[0] = const_cast<Genotype*>(pCalcPopulation->at(0));

    mBestFitError = getBestFitError(bestFits[0]);

    double err = mBestFitError;

    while(!CheckTerminationCriteria(criteria, iters, bestFits[0]))
    {
        #pragma omp parallel for
        for(int thread=1;thread<=numberOfThreads;thread++){
            pCalcPopulation->cross(0.5, mGenerator, thread, numberOfThreads);
            pCalcPopulation->mutate(mutationVariance, mGenerator, thread, numberOfThreads);
            bestFits[thread - 1] = const_cast<Genotype*>(pCalcPopulation->getBestFit(func, thread, numberOfThreads));
        }

        std::sort(bestFits, bestFits + numberOfThreads,
                  [&](Genotype*  a, Genotype*  b)
                  {return func(*a) < func(*b);});

        iterationsCounter++;
        if(pCalcPopulation->at(0)->size() == 2)
        {
            x.push_back(iterationsCounter);
            y.push_back(func(*pCalcPopulation->at(0)));
        }
    }
    iterationsCount = iterationsCounter;
    return bestFits[0];
}

bool CoevolutionEngineST::CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters, const Genotype * bestFit)
{
    double thisIterBestError = getBestFitError(bestFit);

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
    {
        ++iters;
        return false;
    }

    iters = 0;
    mBestFitError = thisIterBestError;
    return false;
}

double CoevolutionEngineST::getBestFitError(const Genotype * bestFit)
{   //Calculating error ONLY (!!) where function minimum is point (0,0,0,...)
    //Fit error calculated as MEAN SQUARE ERROR of all values:
    double bestFitError = 0;
    for(unsigned int i = 0; i < bestFit->size();++i)
    {
        bestFitError += pow(((bestFit)->at(i)).first,2)/bestFit->size();
    }
    return bestFitError;
}

void CoevolutionEngineST::printPopulation()
{
    pCalcPopulation->printPopulation();
}