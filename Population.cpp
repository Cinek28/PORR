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

void Population::cross(const double &crossingCoeff)
{
    unsigned int noOfCrossedGenotypes = mChildrenCount;
    if(crossingCoeff < 1.0)
        noOfCrossedGenotypes = crossingCoeff*mChildrenCount;

    //Randomly shuffle elements and get just first <crossingCoeff>*children_size number of elements:
    std::random_shuffle ( std::advance(pPopulationData->begin(), mPopulationSize), pPopulationData->end() );

    for(int i = mPopulationSize + 1; i < mPopulationSize + noOfCrossedGenotypes; i = i+2)
    {

    }

}