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

    for(int i=0;i<populationCnt;i++){
        pCalcPopulation[i].reset(new Population(popSize, childCnt,genSize));
    }
    x.clear();
    y.clear();
    return init(lowerBound, upperBound);
}

bool CoevolutionEngineST::init(const double &lowerBound, const double &upperBound)
{
    if(lowerBound > upperBound || pCalcPopulation[0] == nullptr)
    {
        std::cout << "Parameters initiated incorrectly (lower bound > upper bound or population not set" << std::endl;
        return false;
    }
    for(int i=0;i<populationCnt;i++){
        pCalcPopulation[i]->init(lowerBound, upperBound);
    }
    std::cout << "Population initiated correctly. Lower bound: " << lowerBound << " upper bound: "
              << upperBound << std::endl;
    return true;
}

const Genotype * CoevolutionEngineST::solve(std::function<double(Genotype)> func, engineStopCriteria criteria, double mutationVariance, int & iterationsCount, bool ompOn)
{
    if(pCalcPopulation.size() == 0)
    {
        std::cout << "Population not set" << std::endl;
        return nullptr;
    }
    unsigned int iters[populationCnt] = {0};
    unsigned int iterationsCounter = 0;
    for(int i=0;i<populationCnt;i++){
        mBestFitError[i] = getBestFitError(ompOn, i);
    }

    bool calcFinished = false;


    while(!calcFinished)
    {
#pragma omp parallel for default(shared)
        for(int i=0;i<populationCnt;i++){
            if(CheckTerminationCriteria(criteria, iters[i], ompOn, i)){
                calcFinished = true;
            }
            pCalcPopulation[i]->cross(0.2,mGenerator, ompOn);
            pCalcPopulation[i]->mutate(mutationVariance, mGenerator, ompOn);
            pCalcPopulation[i]->getBestFit(func, ompOn);
        }
        if(iterationsCounter % 20 == 0){
            std::sort(pCalcPopulation.begin(), pCalcPopulation.end(),
                      [&](Population & a, Population & b)
                      {return func(*(a.at(0))) < func(*(b.at(0)));});
            for(int i =0; i < populationCnt;++i)
            {
                for(int j = 1; j < pCalcPopulation[i]->getSize();++j)
                {
                    for(int k = 0; k < pCalcPopulation[i]->at(j)->size(); ++k) {
                        pCalcPopulation[i].get()->at(j)->at(k).first = pCalcPopulation[0].get()->at(j)->at(k).first;
                        pCalcPopulation[i].get()->at(j)->at(k).second = pCalcPopulation[0].get()->at(j)->at(k).second;
                    }
                }
            }

        }
        iterationsCounter++;
    }
    iterationsCount = iterationsCounter;
    return pCalcPopulation[0]->at(0);
}


bool CoevolutionEngineST::CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters, bool ompOn
        , int populationIter)
{
    double thisIterBestError = getBestFitError(ompOn, populationIter);

    if(criteria == DESIRED_ERROR)
    {
        if(thisIterBestError < mDesiredError)
        {
            mBestFitError[populationIter] <= thisIterBestError;
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
    if(mBestFitError[populationIter] <= thisIterBestError)
    {
        ++iters;
        return false;
    }

    iters = 0;
    mBestFitError[populationIter] = thisIterBestError;
    return false;
}

double CoevolutionEngineST::getBestFitError(bool ompOn, int populationIter)
{   //Calculating error ONLY (!!) where function minimum is point (0,0,0,...)
    //Fit error calculated as MEAN SQUARE ERROR of all values:
    double bestFitError = 0;
    for(unsigned int i = 0; i < pCalcPopulation[0]->getSize();++i)
    {
        bestFitError += pow((pCalcPopulation[0]->at(i)->at(0).first,2))/pCalcPopulation[0]->getSize();
    }
    return bestFitError;
}

void CoevolutionEngineST::printPopulation()
{
    pCalcPopulation[0]->printPopulation();
}