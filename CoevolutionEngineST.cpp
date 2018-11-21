//
// Created by Damian on 2018-11-15.
//

#include "CoevolutionEngineST.h"
#include <math.h>
#include <memory>

#define MUTATION_PROBABILITY 0.1
#define CROSSING_OVER_PERCENTAGE 0.25

CoevolutionEngineST::CoevolutionEngineST( const double &desiredError,
                                         unsigned int noOfItersWithoutImprov):
    mDesiredError(desiredError),
    mNoOfItersWithoutImprov(noOfItersWithoutImprov) {}

bool CoevolutionEngineST::setPopulation(const size_t &popSize, const size_t childCnt, const size_t &genSize,
                                        const double lowerBound = -1.0, const double upperBound = 1.0)
{
    if(popSize < childCnt || lowerBound > upperBound)
        return false;

    pCalcPopulation.reset(std::make_unique<Population>(popSize, childCnt,genSize));
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

bool CoevolutionEngineST::solve(std::function<void(Genotype, Genotype)> func, engineStopCriteria criteria)
{
    if(pCalcPopulation == nullptr)
        std::cout << "Population not set" << std::endl;
        return false;

    pCalcPopulation()
}

double CoevolutionEngineST::CalculateF1(double *x){
    double ex2 = 0, prodcosxi = 1;
    for(int i = 0; i < argumentsCount; i++){
        ex2 += x[i] * x[i];
        prodcosxi = prodcosxi*cos(x[i]/i);
    }
    return ex2 / 40 + 1 - prodcosxi;

}

double CoevolutionEngineST::CalculateF2(double *x){
    double ex2 = 0, ecos2px = 0, exp1 = 0, exp2 = 0;
    for(int i = 0; i < argumentsCount; i++){
        ex2 += x[i] * x[i];
        ecos2px += cos(2 * M_PI * x[i]);
    }
    exp1 = exp(-0.2 * sqrt(ex2 / argumentsCount));
    exp2 = exp(ecos2px / argumentsCount);
    return 20 * exp1 - exp2 + 20 + M_E;
}

void CoevolutionEngineST::CalculateFitnessF1(Population population) {
    for(int i=0;i<populationsCount;i++){
        population.Individuals[i].output = CalculateF1(population.Individuals[i].x);
        population.Individuals[i].error = abs(population.Individuals[i].output - F1_DESIRED_VALUE);
    }
}

void CoevolutionEngineST::CalculateFitnessF2(Population population) {
    for(int i=0;i<populationsCount;i++){
        population.Individuals[i].output = CalculateF2(population.Individuals[i].x);
        population.Individuals[i].error = abs(population.Individuals[i].output - F2_DESIRED_VALUE);
    }
}

bool CoevolutionEngineST::CheckTerminationCriteria(Population *populations) {
    for(int i=0;i<populationsCount;i++){
        for(int j=0;j<populationSize;j++){
            if(populations[j].Individuals[j].error <= desiredError)
                return true;
        }
    }
    return false;
}