//
// Created by tomas on 08.01.2019.
//

#ifndef PORR_RESULTS_H
#define PORR_RESULTS_H

#include <cstring>

class Results{
public:
    Results(int popSize, int childrenCount, int generationSize, int noOfThreads);
    int populationSize;
    int childrenCount;
    int generationSize;
    int noOfThreads;
    double avgExecutionTime;
    double maxExecutionTime;
    double minExecutionTime;
    double avgResult;
    double minResult;
    double maxResult;
};

Results::Results(int popSize, int childrenCount, int generationSize, int noOfThreads) {
    this->populationSize = popSize;
    this->childrenCount = childrenCount;
    this-> generationSize = generationSize;
    this->noOfThreads = noOfThreads;
    avgExecutionTime = 0;
    avgResult = 0;
    minResult = 999999;
    minExecutionTime = 9999999;
    maxExecutionTime = 0;
    maxResult = 0;
}

class Result{
public:
    Result();
    Result(double execTime, double result);
    double result;
    double executionTime;
    bool success;
};

Result::Result(double execTime, double result) {
    success = true;
    this->result = result;
    this->executionTime = execTime;
}

Result::Result() {
    success = false;
}
#endif //PORR_RESULTS_H
