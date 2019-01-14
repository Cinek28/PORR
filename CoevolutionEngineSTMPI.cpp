//
// Created by marcin on 13.01.19.
//

#include "CoevolutionEngineSTMPI.h"
#include <mpi.h>
#include <algorithm>

#define DEBUG(...) printf(__VA_ARGS__)

#define SHARE_RESULTS_ITERS 200


CoevolutionEngineSTMPI::CoevolutionEngineSTMPI(const double & desiredError,const unsigned int & noOfItersWithoutImprov)
        : CoevolutionEngineST(desiredError,noOfItersWithoutImprov)
{
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
}

const Genotype* CoevolutionEngineSTMPI::solve(std::function<double(Genotype)> func, engineStopCriteria criteria,
                                              double mutationVariance, int &iterationsCount, int numberOfThreads)
{

    if(pCalcPopulation == nullptr)
    {
        std::cout << "Population not set" << std::endl;
        return nullptr;
    }
    unsigned int iters = 0, iterationsCounter = 0;

    Genotype * bestFits[numberOfThreads];
    bestFits[0] = const_cast<Genotype*>(pCalcPopulation->at(0));

    mBestFitError = getBestFitError(bestFits[0]);

    double err = mBestFitError;
    stop = false;

    while(1)
    {
//#pragma omp parallel for
        for(int thread=1;thread<=numberOfThreads;thread++){
            pCalcPopulation->cross(0.5, mGenerator, thread, numberOfThreads);
            pCalcPopulation->mutate(mutationVariance, mGenerator, thread, numberOfThreads);
            bestFits[thread - 1] = const_cast<Genotype*>(pCalcPopulation->getBestFit(func, thread, numberOfThreads));
        }

        std::sort(bestFits, bestFits + numberOfThreads,
                  [&](Genotype*  a, Genotype*  b)
                  {return func(*a) < func(*b);});

        iterationsCounter++;

        if(iterationsCounter%SHARE_RESULTS_ITERS == 0)
        {
            int owner = getBestResultOwner();
            shareBestResultGenotype(owner);
            MPI_Barrier(MPI_COMM_WORLD);
            if(CheckTerminationCriteria(criteria, iters, pCalcPopulation->at(0)))
            {
                break;
            }
        }
    }
    DEBUG("[%d] Out of while loop.\n", rank);
    MPI_Barrier(MPI_COMM_WORLD);
    iterationsCount = iterationsCounter;
    return pCalcPopulation->at(0);
}

int CoevolutionEngineSTMPI::getBestResultOwner()
{
    MPI_Status status;

    int bestErrorOwnerID;
    double error = getBestFitError(pCalcPopulation->at(0));
    if (rank != 0) {
        // slave
        DEBUG("[%d] Sending best result = %f to master\n", rank, error);
        MPI_Send(&error, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
    } else {
        // master
        std::vector<std::pair<double, int>> results;
        results.emplace_back(error, 0);

        double tempError;

        for (auto i = 1; i < comm_size; ++i) {
            MPI_Recv(&tempError, 1, MPI_DOUBLE, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            results.emplace_back(tempError, status.MPI_SOURCE);
            DEBUG("[0] Receiving error = %f from %d\n", tempError, status.MPI_SOURCE);
        }
        std::sort(results.begin(), results.end());
        bestErrorOwnerID = results.begin()->second;
    }
    MPI_Bcast(&bestErrorOwnerID, 1, MPI_INT, 0, MPI_COMM_WORLD);

    DEBUG("[%d] bestErrorOwnerID = %d\n", rank, bestErrorOwnerID);

    return bestErrorOwnerID;
}

int CoevolutionEngineSTMPI::getStop()
{
    MPI_Status status;

    int isStop = stop;
    if (rank != 0) {
        // slave
        DEBUG("[%d] Sending stop = %d to master\n", rank, isStop);
        MPI_Send(&isStop, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
    } else {
        // master
        std::vector<int> stops;
        stops.emplace_back(stop);

        int tempStop = 0;

        for (auto i = 1; i < comm_size; ++i) {
            MPI_Recv(&tempStop, 1, MPI_INT, MPI_ANY_SOURCE, 1, MPI_COMM_WORLD, &status);
            stops.emplace_back(tempStop);
            DEBUG("[0] Receiving stop = %d from %d\n", tempStop, status.MPI_SOURCE);
        }
        std::sort(stops.begin(), stops.end());
        isStop = stops.back();
    }
    MPI_Bcast(&isStop, 1, MPI_INT, 0, MPI_COMM_WORLD);
    stop = isStop;

    DEBUG("[%d] Stop = %d\n", rank, stop);

    return stop;
}



void CoevolutionEngineSTMPI::shareBestResultGenotype(const int &ownerID)
{
    std::vector<double> buffer;
    int size = pCalcPopulation->at(0)->size();
    buffer.reserve(size);

    if (rank == ownerID) {
        for (int i = 0; i < size;++i)
            buffer.push_back(pCalcPopulation->at(0)->at(i).first);
    }

    MPI_Bcast(buffer.data(), size * sizeof(double), MPI_BYTE, ownerID, MPI_COMM_WORLD);
    DEBUG("[%d] Getting best Genotype buffer from = %d\n", rank, ownerID);

    if (rank == ownerID)
        return;

    for (int i = 0; i < size; ++i)
        pCalcPopulation->setGenotypeValue(0,i,buffer[i]);
}