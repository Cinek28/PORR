//
// Created by Damian on 2018-11-15.
//

#ifndef PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H
#define PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H

#define F1_DESIRED_VALUE 0
#define F2_DESIRED_VALUE 0

#include <random>


class CoevolutionEngineST {

private:
    int populationsCount, populationSize, argumentsCount;
    double desiredError, speed;
    std::default_random_engine generator;
public:
    CoevolutionEngineST(int & argumentsCount, unsigned int & populationsCount, unsigned int & populationSize, double desiredError);

    double CalculateF1(double *x);
    double CalculateF2(double *x);
    Population InitializePopulation();

    void CalculateFitnessF1(Population population);
    void CalculateFitnessF2(Population population);

    bool CheckTerminationCriteria(Population populations[]);

    Individual * PerformCrossingOver(Population population);
    void PerformMutation(Individual children[]);

};

#endif //PARALLEL_COMPUTING_COEVOLUTIONENGINEST_H