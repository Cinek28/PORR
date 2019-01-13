//
// Created by marcin on 13.01.19.
//

#ifndef PORR_COEVOLUTIONENGINESTMPI_H
#define PORR_COEVOLUTIONENGINESTMPI_H

#include "CoevolutionEngineST.h"


class CoevolutionEngineSTMPI: public CoevolutionEngineST {
public:

    CoevolutionEngineSTMPI(const double & desiredError = 0.1,const unsigned int & noOfItersWithoutImprov = 20);

    virtual const Genotype * solve(std::function<double (Genotype)> func, engineStopCriteria criteria, double mutationVariance,
                                   int & iterationsCount, int numberOfThreads) override ;

    int getBestResultOwner();

    void shareBestResultGenotype(const int & ownerID) ;
    int getStop();

    int comm_size;
    int rank;
    bool stop = false;
};


#endif //PORR_COEVOLUTIONENGINESTMPI_H
