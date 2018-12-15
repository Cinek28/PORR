//
// Created by marcin on 13.11.18.
//

#ifndef PARALLEL_COMPUTING_POPULATION_H
#define PARALLEL_COMPUTING_POPULATION_H

#include <vector>
#include <memory>
#include <iostream>
#include <functional>
#include <random>


typedef std::vector<std::pair<double,double>> Genotype;

class Population
{

public:

    Population(const size_t & popSize = 20, const size_t & childCnt = 1, const size_t & genSize = 1);

    void init(const int & lowerBound, const int & upperBound);

    void cross(std::default_random_engine &generator);

    void mutate(const double &normalDistVariance, std::default_random_engine &generator);

    const Genotype * getBestFit(std::function<double (Genotype &)> func);

    const Genotype * at(unsigned int index);

    size_t getPopulationSize() const { return mPopulationSize;};
    size_t getChildrenCount() const { return mChildrenCount;};
    size_t getSize() const {return mPopulationSize + mChildrenCount;};

    double getUpperBound() const {return mUpperBound;};
    double getLowerBound() const {return mLowerBound;};

    void printPopulation() const;


private:

    std::unique_ptr<std::vector<std::unique_ptr<Genotype>>> pPopulationData;

    size_t mPopulationSize;
    size_t mChildrenCount;

    double mUpperBound = -1;
    double mLowerBound = 1;


};


#endif //PARALLEL_COMPUTING_POPULATION_H
