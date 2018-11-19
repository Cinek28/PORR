//
// Created by Damian on 2018-11-15.
//

#include "CoevolutionEngineST.h"
#include "Population.h"
#include <math.h>
#include <random>

#define MUTATION_PROBABILITY 0.1
#define CROSSING_OVER_PERCENTAGE 0.25

CoevolutionEngineST::CoevolutionEngineST(int argumentsCount, int populationsCount, double desiredError) {
    this->argumentsCount = argumentsCount;
    this->desiredError = desiredError;
    this->populationsCount = populationsCount;
    this->populationSize = populationSize;
    speed = 2;
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

Population CoevolutionEngineST::InitializePopulation(){
    Population population = Population();
    population.Individuals = new Individual[populationSize];

    for(int i=0;i<populationSize;i++){
        population.Individuals[i] = Individual();
        population.Individuals[i].x = new double[argumentsCount];

        std::normal_distribution<double> distribution(0, 3);

        for(int j=0;j<argumentsCount;j++){
            population.Individuals[i].x[j] = lowerBound + distribution(generator);
            while(population.Individuals[i].x[j] < lowerBound || population.Individuals[i].x[j] > upperBound){
                population.Individuals[i].x[j] = lowerBound + distribution(generator);
            }
        }
    }
    return population;
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

Individual * CoevolutionEngineST::PerformCrossingOver(Population population) {
    int individualsForCrossingOverCount = (int)(CROSSING_OVER_PERCENTAGE * populationSize);
    Individual * children = new Individual[individualsForCrossingOverCount];
    for(int i=0;i<individualsForCrossingOverCount;i++){
        int parent1Index = rand();//TODO change to std::random_device rd;
        int parent2Index = rand();//TODO change to std::random_device rd;
        //TODO crossing-over creating child
        //TODO children add child
    }
    //TODO return children
}
void CoevolutionEngineST::PerformMutation(Individual *children) {
    //TODO mutation of all children with probability in each gene  MUTATION_PROBABILITY
}
//TODO method to choose the new population and calculate new speed with 1/5 method

//delta from 1 to 0.1 in the end: delta -= 0.01
//std::normal_distribution<double> distribution(0,2.0);