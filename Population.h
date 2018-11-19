//
// Created by marcin on 13.11.18.
//

#ifndef PARALLEL_COMPUTING_POPULATION_H
#define PARALLEL_COMPUTING_POPULATION_H

#include <vector>
#include <memory>


class Population {

public:

    Population(unsigned int genSize = 1, unsigned int popSize = 20);

    void init(const int &minValue, const int &maxValue);

    void cross();

    void mutate();



private:

    std::unique_ptr<std::vector<std::pair<double,unsigned double>>> pPopulationData;

    unsigned int mGenotypeSize;
    unsigned int mPopulationSize;

    double mMinimum = -1;
    double mMaximum = 1;


};


#endif //PARALLEL_COMPUTING_POPULATION_H
