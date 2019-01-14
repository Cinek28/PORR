//
// Created by marcin on 17.12.18.
//

#ifndef PORR_UTILS_H
#define PORR_UTILS_H

#include <sstream>
#include "mpi.h"
#include "CoevolutionEngineSTMPI.h"

std::string enum2cChar[2] = {
        "NO_OF_ITERS_WITHOUT_IMPROV",
        "DESIRED_ERROR",
};

void initializeOptimizationFunctions(std::function<double(Genotype)> &optimizedFunc1,
                                     std::function<double(Genotype)> &optimizedFunc2) {
    optimizedFunc1= [](Genotype genotype)
    {
        double sum = 0, prod = 1.0;
        for(unsigned int i = 0; i < genotype.size();++i)
        {
            sum += pow(genotype[i].first,2);
            prod *= cos(genotype[i].first/(i+1));
        }
        return 1./40.0*sum+1-prod;
    };
    optimizedFunc2= [](Genotype genotype)
    {
        double ex2 = 0, ecos2px = 0;
        for (unsigned int i = 0; i < genotype.size(); ++i) {
            ex2 += pow(genotype[i].first, 2);
            ecos2px += cos(2 * M_PI * genotype[i].first);
        }
        return -20 * exp(-0.2 * sqrt(ex2 / genotype.size())) - exp(ecos2px / genotype.size()) + 20 + M_E;
    };
}

void write_text_to_log_file( const std::string &text )
{
    time_t _tm =time(NULL );
    struct tm * curtime = localtime ( &_tm );
    auto datetimeNow = asctime(curtime);
    datetimeNow[strlen(datetimeNow) - 1] = 0;
    std::ofstream log_file(
            "./log_file.log", std::ios_base::out | std::ios_base::app );
    if(log_file.is_open())
    {
        log_file << datetimeNow << " | " << text << std::endl;
        log_file.close();
    }
    else
    {
        std::cout << "Log file not open! Cannot write logs to file." << std::endl;
    }
}

bool performCalculations(CoevolutionEngineST &cov, const std::function<double(Genotype)> &optimizedFunc, int popSize,
                         int childCnt, int genSize, int lowerBound, int upperBound,
                         CoevolutionEngineST::engineStopCriteria criteria, std::string optimizedFuncName, double mutationVariance,
                         int numberOfThreads) {

    std::ostringstream resultStr;

    double minimumExecutionTime = 999999, minimumETResult, minimumETSingleIterationTime;
    double maximumExecutionTime = 0, maximumETResult, maximumETSingleIterationTime;
    double meanExecutionTime = 0, meanETResult, meanETSingleIterationTime;

    std::cout << "Threads: " << numberOfThreads << ",  " << optimizedFuncName << ", n: " << genSize;

    for(int i=0;i<5;i++){
        bool populationSet = cov.setPopulation(popSize, childCnt, genSize, lowerBound, upperBound);
        if(!populationSet){
            return false;
        }
        int iterationsCount;
        double startF1 = omp_get_wtime( );
        const Genotype* gen1 = cov.solve(optimizedFunc, criteria, mutationVariance, iterationsCount, numberOfThreads);
        double endF1 = omp_get_wtime( );
        double singleIterationExecutionTime = 1000 * (double)(endF1-startF1)/(double)iterationsCount;
        std::cout << std::endl << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
        std::cout << "\nExecution Time: " << endF1 - startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]" << std::endl << std::endl;
        meanExecutionTime += (endF1 - startF1);
        meanETResult += optimizedFunc(*gen1);
        meanETSingleIterationTime += singleIterationExecutionTime;
        if(endF1 - startF1 < minimumExecutionTime){
            minimumExecutionTime = endF1 - startF1;
            minimumETSingleIterationTime = singleIterationExecutionTime;
            minimumETResult = optimizedFunc(*gen1);
        }
        if(endF1 - startF1 > maximumExecutionTime){
            maximumExecutionTime = endF1 - startF1;
            maximumETSingleIterationTime = singleIterationExecutionTime;
            maximumETResult = optimizedFunc(*gen1);
        }
    }

    meanExecutionTime = meanExecutionTime/5.;
    meanETResult = meanETResult/5.;
    meanETSingleIterationTime = meanETSingleIterationTime/5.;

    resultStr << "Threads: " << numberOfThreads << ",  " << optimizedFuncName << ", n: " << genSize <<
              ", meanET[s]: " << meanExecutionTime << ", meanSingleET[ms]: " << meanETSingleIterationTime << ", meanY: " << meanETResult <<
              ", minET[s]: " << minimumExecutionTime << ", minSingleET:[ms]: " << minimumETSingleIterationTime << ", minY: " << minimumETResult <<
              ", maxET[s]: " << maximumExecutionTime << ", maxSingleET[ms]: " << maximumETSingleIterationTime << ", maxY: " << maximumETResult;
    write_text_to_log_file(resultStr.str());

    return true;
}

bool performCalculationsMPI(const std::function<double(Genotype)> optimizedFunc, int popSize,
                         int childCnt, int genSize, int lowerBound, int upperBound,
                         CoevolutionEngineST::engineStopCriteria criteria, std::string optimizedFuncName,
                            double mutationVariance) {

    double startF1, endF1;
    std::ostringstream resultStr;

    double minimumExecutionTime = 999999, minimumETResult, minimumETSingleIterationTime;
    double maximumExecutionTime = 0, maximumETResult, maximumETSingleIterationTime;
    double meanExecutionTime = 0, meanETResult, meanETSingleIterationTime;
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    Genotype* gen1 = nullptr;
    double singleIterationExecutionTime = 0;
    int iters = 0;

    CoevolutionEngineSTMPI cov;
    for(int i = 0; i < 1; ++i)
    {
        cov.setPopulation(popSize/8, childCnt/8, genSize, lowerBound, upperBound);
        cov.setDesiredError(0.1);
        cov.setNoOfItersWithoutImprov(100);

        int iterationsCount;
        if(rank == 0)
        {
            startF1 = omp_get_wtime( );
        }
        const Genotype* genTemp = cov.solve(optimizedFunc, criteria, mutationVariance, iterationsCount, 1);
        if(rank == 0)
        {
            gen1 = const_cast<Genotype*>(genTemp);
            iters = iterationsCount;
        }
        std::cout << "[" << rank  << "]" << "Waiting on barrier" << std::endl;
        MPI_Barrier(MPI_COMM_WORLD);

        if(rank == 0)
        {
            endF1 = omp_get_wtime( );
            singleIterationExecutionTime= 1000 * (double)(endF1-startF1)/(double)iters;
            std::cout << std::endl << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
            std::cout << "\nExecution Time: " << endF1 - startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]" << std::endl << std::endl;
            meanExecutionTime += (endF1 - startF1);
            meanETResult += optimizedFunc(*gen1);
            meanETSingleIterationTime += singleIterationExecutionTime;
            if(endF1 - startF1 < minimumExecutionTime){
                minimumExecutionTime = endF1 - startF1;
                minimumETSingleIterationTime = singleIterationExecutionTime;
                minimumETResult = optimizedFunc(*gen1);
            }
            if(endF1 - startF1 > maximumExecutionTime){
                maximumExecutionTime = endF1 - startF1;
                maximumETSingleIterationTime = singleIterationExecutionTime;
                maximumETResult = optimizedFunc(*gen1);
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    if(rank == 0)
    {
        meanExecutionTime = meanExecutionTime;
        meanETResult = meanETResult;
        meanETSingleIterationTime = meanETSingleIterationTime;

        resultStr << optimizedFuncName << ", n: " << genSize <<
                  ", meanET[s]: " << meanExecutionTime << ", meanSingleET[ms]: " << meanETSingleIterationTime << ", meanY: " << meanETResult <<
                  ", minET[s]: " << minimumExecutionTime << ", minSingleET:[ms]: " << minimumETSingleIterationTime << ", minY: " << minimumETResult <<
                  ", maxET[s]: " << maximumExecutionTime << ", maxSingleET[ms]: " << maximumETSingleIterationTime << ", maxY: " << maximumETResult;
        write_text_to_log_file(resultStr.str());
        std::cout << "Writing to file." << std::endl;
    }
    return true;
}

bool test()
{
    CoevolutionEngineST cov;
    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

    bool isOMPOn = true;
    bool calculationsPerformed;
    for(int i = 1; i <= 8; ++i)
    {
        std::vector<int> argSizes = {2,10,20,50,100};
        for(int j = 0;j <argSizes.size(); ++j)
        {
            omp_set_num_threads(i);
            cov.setDesiredError(0.1);
            cov.setNoOfItersWithoutImprov(300);
            calculationsPerformed = performCalculations(cov, optimizedFunc1, 30, 12, argSizes[j], -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.2, isOMPOn);
            if(!calculationsPerformed)
                return -1;
            calculationsPerformed = performCalculations(cov, optimizedFunc2, 30, 12, argSizes[j], -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.2, isOMPOn);
            if(!calculationsPerformed)
                return -1;
        }
    }
}



#endif //PORR_UTILS_H
