//
// Created by marcin on 13.11.18.
//

#include "Population.h"
#include <algorithm>
#include <omp.h>


Population::Population(const size_t &popSize, const size_t &childCnt, const size_t &genSize):
    mPopulationSize(popSize),
    mChildrenCount(childCnt)
{
    pPopulationData.reset(new std::vector<std::unique_ptr<Genotype>>(mPopulationSize + mChildrenCount));

    for_each(pPopulationData->begin(), pPopulationData->end(), [&genSize](std::unique_ptr<Genotype> & genotype){
        genotype.reset(new Genotype(genSize));}
    );
}

void Population::init(const int &lowerBound, const int &upperBound)
{
    mLowerBound = lowerBound;
    mUpperBound = upperBound;

    srand48(time(nullptr));

    std::cout << "Initiating population" << std::endl;

    for_each(pPopulationData->begin(), pPopulationData->end(), [&](std::unique_ptr<Genotype> &  genotype) {
        std::cout << "Genotype: { ";
        for(unsigned int i = 0; i < genotype->size(); ++i)
        {
            genotype->at(i).first = mLowerBound + (mUpperBound - mLowerBound) * drand48();
            genotype->at(i).second = drand48();
            std::cout << "x[" << i << "]=" << genotype->at(i).first << ", s[" << i << "]=" << genotype->at(i).second << ", ";
        }
        std::cout << "} " << std::endl; }
    );
}

const Genotype* Population::at(unsigned int index)
{
    return pPopulationData->at(index).get();
}

const Genotype * Population::getBestFit(std::function<double (Genotype &)> func)
{
    //Sequential version:
    std::sort(pPopulationData->begin(), pPopulationData->end(),
              [&](const std::unique_ptr<Genotype> & a, const std::unique_ptr<Genotype> & b)
                    {return func(*a.get()) < func(*b.get());});

    //TODO: Parallel sorting algorithm (best merge/heap sort):

    const Genotype * bestFittingGenotype = pPopulationData->at(0).get();
    return bestFittingGenotype;
}

void Population::cross(const double &crossingCoeff, std::vector<std::mt19937> &generator)
{
    unsigned int noOfCrossedGenotypes = mChildrenCount;
    if(crossingCoeff < 1.0 && crossingCoeff >= 0.0)
        noOfCrossedGenotypes = crossingCoeff*mChildrenCount;

    //Randomly shuffle elements and get just first <crossingCoeff>*children_size number of elements:
    auto iter = std::next(pPopulationData->begin(), mPopulationSize);
    std::random_shuffle ( pPopulationData->begin(), iter );
    //TODO: OMP Parallel
    unsigned int size = pPopulationData->at(0)->size(); // all vectors are of the same size
    #pragma omp parallel for schedule(static) collapse(2) default(shared)
    for (unsigned int i = 0; i < noOfCrossedGenotypes; ++i) {
        for (unsigned int j = 0; j < size; ++j) {
            pPopulationData->at(i + mPopulationSize)->at(j) =
                    pPopulationData->at(i + i%2)->at(j);
        }
    }
}

void Population::mutate(const double &normalDistVariance, std::vector<std::mt19937> &generator)
{
    unsigned int size = pPopulationData->at(0)->size(); // all vectors are of the same size
    //TODO: OMP Parallel
    #pragma omp parallel for schedule(static) collapse(2) default(shared)
    for (unsigned int i = mPopulationSize; i < mPopulationSize + mChildrenCount; ++i) {
        for (unsigned int j = 0; j < size; ++j) {

            pPopulationData->at(i)->at(j).second *= exp(randNorm(0.0, normalDistVariance,
                                                                 generator[omp_get_thread_num()]));

            pPopulationData->at(i)->at(j).first += pPopulationData->at(i)->at(j).second
                                                   * randNorm(0.0, pPopulationData->at(i)->at(j).second,
                                                              generator[omp_get_thread_num()]);
            if (pPopulationData->at(i)->at(j).first > mUpperBound)
                pPopulationData->at(i)->at(j).first = mUpperBound;
            else if (pPopulationData->at(i)->at(j).first < mLowerBound)
                pPopulationData->at(i)->at(j).first = mLowerBound;
        }
    }
}

void Population::printPopulation() const
{
    std::cout << "Population:" << std::endl;
    for_each(pPopulationData->begin(), pPopulationData->end(), [&](std::unique_ptr<Genotype> &  genotype) {
        std::cout << "Genotype: { ";
        for(unsigned int i = 0; i < genotype->size(); ++i)
        {
            std::cout << "x[" << i << "]=" << genotype->at(i).first << ", s[" << i << "]=" << genotype->at(i).second << ", ";
        }
        std::cout << "} " << std::endl; }
    );
}

double Population::randNorm (double mu, double sigma, std::mt19937& generator)
{
    double U1, U2, W, mult;
    double X1;
    do
    {
        U1 = -1 + ((double)generator()/generator.max()) * 2;
        U2 = -1 + ((double)generator()/generator.max()) * 2;
        W = pow (U1, 2) + pow (U2, 2);
    }
    while (W >= 1 || W == 0);

    mult = sqrt ((-2 * log (W)) / W);
    X1 = U1 * mult;

    return (mu + sigma * X1);
}