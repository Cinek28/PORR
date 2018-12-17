//
// Created by Damian on 2018-11-15.
//

#include "CoevolutionEngineST.h"

bool CoevolutionEngineST::setPopulation(const size_t &popSize, const size_t childCnt, const size_t &genSize,
                                        const double lowerBound, const double upperBound)
{
    if(popSize < childCnt || lowerBound > upperBound || (childCnt%2 != 0))
        return false;

    for(int i=0;i<populationCnt;i++){
        pCalcPopulation[i].reset(new Population(popSize, childCnt,genSize));
    }

    return init(lowerBound, upperBound);
}

bool CoevolutionEngineST::init(const double &lowerBound, const double &upperBound)
{
    if(lowerBound > upperBound || pCalcPopulation == nullptr)
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
    if(pCalcPopulation == nullptr)
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
        for(int i=0;i<populationCnt;i++){
            if(CheckTerminationCriteria(criteria, iters[i], ompOn, i)){
                calcFinished = true;
                break;
            }
            pCalcPopulation[i]->cross(mGenerator, ompOn);
            pCalcPopulation[i]->mutate(mutationVariance, mGenerator, ompOn);
            pCalcPopulation[i]->getBestFit(func, ompOn);
        }
        if(iterationsCounter % 10 == 0){

            //getalldata
            //sort all data
            //write all data to threads
            //std::sort(pPopulationData->begin(), pPopulationData->end(),
            //              [&](const std::unique_ptr<Genotype> & a, const std::unique_ptr<Genotype> & b)
            //                    {return func(*a.get()) < func(*b.get());});
        }
        iterationsCounter++;
    }
    iterationsCount = iterationsCounter;
    return pCalcPopulation->at(0);
}


bool CoevolutionEngineST::CheckTerminationCriteria(engineStopCriteria criteria, unsigned int & iters, bool ompOn, int populationIter)
{
    double thisIterBestError = getBestFitError(ompOn, populationIter);

    if(criteria == DESIRED_ERROR)
    {
        if(thisIterBestError < mDesiredError)
        {
            mBestFitError[populationIter] = thisIterBestError;
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

double CoevolutionEngineST::getBestFitError(bool ompOn, int populationIterator)
{   //Calculating error ONLY (!!) where function minimum is point (0,0,0,...)
    //Fit error calculated as MEAN SQUARE ERROR of all values:
    double bestFitError = 0;
    //TODO DONE OMP Parallel
    if(ompOn){
        for(unsigned int i = 0; i < pCalcPopulation[populationIterator]->at(0)->size();++i)
        {
            bestFitError += pow(((pCalcPopulation[populationIterator]->at(0))->at(i)).first,2)/pCalcPopulation[populationIterator]->at(0)->size();
        }
    }
    else{
        for(unsigned int i = 0; i < pCalcPopulation[populationIterator]->at(0)->size();++i)
        {
            bestFitError += pow(((pCalcPopulation[populationIterator]->at(0))->at(i)).first,2)/pCalcPopulation[populationIterator]->at(0)->size();
        }
    }
    return bestFitError;
}