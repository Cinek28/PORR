//
// Created by marcin on 13.11.18.
//

#include "Population.h"
#include <algorithm>


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

    std::cout << "Initiating population" << std::endl;
    std::random_device r;
    std::default_random_engine generator(r());
    std::uniform_real_distribution<double> uniformDist(0.0,1.0);

    for_each(pPopulationData->begin(), pPopulationData->end(), [&](std::unique_ptr<Genotype> &  genotype) {
                 for (unsigned int i = 0; i < genotype->size(); ++i) {
                     genotype->at(i).first = mLowerBound + (mUpperBound - mLowerBound) * uniformDist(generator);
                     genotype->at(i).second = uniformDist(generator);
                 }
             }
    );
}

const Genotype* Population::at(unsigned int index)
{
    return pPopulationData->at(index).get();
}

const Genotype * Population::getBestFit(std::function<double (Genotype &)> func, int thread, int numberOfThreads)
{
    unsigned int threadChildrenCount = mChildrenCount/numberOfThreads;
    unsigned int threadPopulation = mPopulationSize/numberOfThreads;

    auto endIter = std::next(pPopulationData->begin(), thread * (threadPopulation + threadChildrenCount));
    auto beginningIter = std::next(pPopulationData->begin(), (thread - 1) * (threadPopulation + threadChildrenCount));

    std::sort(beginningIter, endIter,
              [&](const std::unique_ptr<Genotype> & a, const std::unique_ptr<Genotype> & b)
                    {return func(*a.get()) < func(*b.get());});

    const Genotype * bestFittingGenotype = pPopulationData->at((thread - 1) * (threadPopulation + threadChildrenCount)).get();
    return bestFittingGenotype;
}

void Population::cross(const double &crossingCoeff, std::default_random_engine &generator, int thread, int numberOfThreads)
{//http://www.scholarpedia.org/article/Evolution_strategies
    unsigned int threadChildrenCount = mChildrenCount/numberOfThreads;
    unsigned int threadPopulation = mPopulationSize/numberOfThreads;
    unsigned int noOfCrossedGenotypes = threadChildrenCount;
    if(crossingCoeff < 1.0 && crossingCoeff >= 0.0)
        noOfCrossedGenotypes = crossingCoeff*noOfCrossedGenotypes;

    //Randomly shuffle elements and get just first <crossingCoeff>*children_size number of elements:
    auto endIter = std::next(pPopulationData->begin(), (thread - 1) * (threadPopulation + threadChildrenCount) + threadPopulation);
    auto beginningIter = std::next(pPopulationData->begin(), (thread - 1) * (threadPopulation + threadChildrenCount));
    std::random_shuffle ( beginningIter, endIter );
    std::uniform_int_distribution<int> uniformDist(0,1);

    unsigned int size = pPopulationData->at((thread - 1)*(threadPopulation + threadChildrenCount))->size();
        for (unsigned int i = (thread - 1) * (threadPopulation + threadChildrenCount); i < (thread - 1) * (threadPopulation + threadChildrenCount) + noOfCrossedGenotypes; ++i) {
            for (unsigned int j = 0; j < size; ++j) {
                pPopulationData->at(i + threadPopulation)->at(j) = pPopulationData->at(i+uniformDist(generator))->at(j);
            }
        }
}

void Population::mutate(const double &normalDistVariance, std::default_random_engine &generator, int thread, int numberOfThreads)
{
    unsigned int threadChildrenCount = mChildrenCount/numberOfThreads;
    unsigned int threadPopulation = mPopulationSize/numberOfThreads;
    unsigned int size = pPopulationData->at((thread - 1)*(threadPopulation + threadChildrenCount))->size(); // all vectors are of the same size
    std::normal_distribution<double> distribution(0.0,normalDistVariance);
        for(unsigned int i = ((thread - 1) * (threadPopulation + threadChildrenCount) + threadPopulation); i < thread * (threadPopulation + threadChildrenCount); ++i)
        {
            for(unsigned int j = 0; j < size; ++j)
            {
                pPopulationData->at(i)->at(j).second *= exp(distribution(generator));
                std::normal_distribution<double> distr(0.0,pPopulationData->at(i)->at(j).second);
                pPopulationData->at(i)->at(j).first += pPopulationData->at(i)->at(j).second*distr(generator);
                if(pPopulationData->at(i)->at(j).first > mUpperBound)
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
