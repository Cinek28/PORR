//
// Created by marcin on 13.11.18.
//

#include "Population.h"
#include <algorithm>
#include <random>
#include <cmath>


Population::Population(const size_t &popSize, const size_t &childCnt, const size_t &genSize):
    mPopulationSize(popSize),
    mChildrenCount(childCnt)
{
    pPopulationData = std::make_unique<std::vector<Genotype>>(mPopulationSize + mChildrenCount);

    for_each(pPopulationData->begin(), pPopulationData->end(), [](auto & genotype){
        genotype.reset(new Genotype(genSize));}
    );
}

void Population::init(const int &lowerBound, const int &upperBound)
{
    mLowerBound = lowerBound;
    mUpperBound = upperBound;

    srand48(time(nullptr));

    for_each(pPopulationData->begin(), pPopulationData->end(), [](auto & genotype) {
        for(int i = 0; i < genotype->size(); ++i)
        {
            genotype[i].first() = mLowerBound + (mUpperBound - mLowerBound) * drand48();
            genotype[i].second() = drand48();
        }}
    );
}

const Genotype * Population::getBestFit(std::function<void(const Genotype &, const Genotype &)> func)
{
    //Sequential version:
    std::sort(pPopulationData.begin(), pPopulationData.end(), func);

    //TODO: Parallel sorting algorithm (best merge/heap sort):

    const Genotype * bestFittingGenotype = pPopulationData.begin()->get();
    return bestFittingGenotype;

}

void Population::cross(const double &crossingCoeff, const std::default_random_engine &generator)
{
    unsigned int noOfCrossedGenotypes = mChildrenCount;
    if(crossingCoeff < 1.0 && crossingCoeff >= 0.0)
        noOfCrossedGenotypes = crossingCoeff*mChildrenCount;

    //Randomly shuffle elements and get just first <crossingCoeff>*children_size number of elements:
    std::random_shuffle ( pPopulationData->begin(), std::advance(pPopulationData->begin(), mPopulationSize) );

    //TODO: OMP Parallel
    for(int i = 0; i < noOfCrossedGenotypes; ++i)
    {
        for(int j = 0; j < mPopulationData[i]->size(); ++j)
        {
            mPopulationData[i + mPopulationSize][j] = mPopulationData[2*i + generator() % 2][j];
        }
    }
}

void Population::mutate(const double &normalDistVariance, const std::default_random_engine &generator)
{
    std::normal_distribution<double> distribution(0.0,normalDistVariance);
    //TODO: OMP Parallel
    for(int i = mPopulationSize + 1; i < mPopulationSize + noOfCrossedGenotypes; i = i+2)
    {
        for(int j = 0; j < mPopulationData[i]->size(); ++j)
        {
            pPopulationData[i][j].second() *= exp(distribution(generator));
            std::normal_distribution<double> distr(0.0,pPopulationData[i][j].second());
            pPopulationData[i][j].first() += pPopulationData[i][j].second()*distr(generator);
        }
    }
}