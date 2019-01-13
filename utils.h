//
// Created by marcin on 17.12.18.
//

#ifndef PORR_UTILS_H
#define PORR_UTILS_H

#include <sstream>
#include "mpi.h"

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
            "../log_file.log", std::ios_base::out | std::ios_base::app );
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

    std::ostringstream popInitStr;
    bool populationSet = cov.setPopulation(popSize, childCnt, genSize, lowerBound, upperBound);
    if(!populationSet){
        popInitStr << "Population initialization error!";
        write_text_to_log_file(popInitStr.str());
        return false;
    }

    popInitStr << "Population initialized with params:"
               << " popSize: " << popSize
               << ", childCnt: " << childCnt
               << ", genSize: " << genSize
               << ", lowerBound: " << lowerBound
               << ", upperBound: " << upperBound
               << ", function: " << optimizedFuncName
               << ", criteria: " << enum2cChar[criteria];
    write_text_to_log_file(popInitStr.str());
    int iterationsCount;
    double startF1 = omp_get_wtime( );
    const Genotype* gen1 = cov.solve(optimizedFunc, criteria, mutationVariance, iterationsCount, numberOfThreads);
    double endF1 = omp_get_wtime( );
    double singleIterationExecutionTime = 1000 * (double)(endF1-startF1)/(double)iterationsCount;
    std::ostringstream popXResultStr, popExecTimeStr, popYResultStr;
    std::cout << "Solution [" << optimizedFuncName <<  ", X]: " << std::endl;
    popXResultStr << "Solution [" << optimizedFuncName << ",X]: ";
    for(unsigned int i = 0; i < gen1->size(); ++i)
    {
        std::cout << gen1->at(i).first << ", ";
        popXResultStr << gen1->at(i).first << ", ";
    }
    std::cout << std::endl << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
    popYResultStr << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
    write_text_to_log_file(popXResultStr.str());
    write_text_to_log_file(popYResultStr.str());
    std::cout << "\nExecution Time: " << endF1 - startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]" << std::endl << std::endl;
    popExecTimeStr << "Execution Time: " << endF1-startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]";
    write_text_to_log_file(popExecTimeStr.str());
    return true;
}

bool performCalculationsMPI(const std::function<double(Genotype)> optimizedFunc, int popSize,
                         int childCnt, int genSize, int lowerBound, int upperBound,
                         CoevolutionEngineST::engineStopCriteria criteria, std::string optimizedFuncName, double mutationVariance) {

    std::ostringstream popInitStr;
    double startF1, endF1;
    MPI_Init(nullptr,nullptr);
    int comm_size;
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if(rank == 0)
    {
        popInitStr << "Population initialized with params:"
                   << " popSize: " << popSize
                   << ", childCnt: " << childCnt
                   << ", genSize: " << genSize
                   << ", lowerBound: " << lowerBound
                   << ", upperBound: " << upperBound
                   << ", function: " << optimizedFuncName
                   << ", criteria: " << enum2cChar[criteria];
        write_text_to_log_file(popInitStr.str());
    }

    Genotype* gen1 = nullptr;
    double singleIterationExecutionTime = 0;
    int iters = 0;

        CoevolutionEngineSTMPI cov;
        bool populationSet = cov.setPopulation(popSize, childCnt, genSize, lowerBound, upperBound);
        if(!populationSet){
            popInitStr << "Population initialization error!";
            write_text_to_log_file(popInitStr.str());
            return false;
        }

        cov.setDesiredError(0.01);
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
        std::ostringstream popXResultStr, popExecTimeStr, popYResultStr;
        std::cout << "Solution [" << optimizedFuncName <<  ", X]: " << std::endl;
        popXResultStr << "Solution [" << optimizedFuncName << ",X]: ";
        for(unsigned int i = 0; i < gen1->size(); ++i)
        {
            std::cout << gen1->at(i).first << ", ";
            popXResultStr << gen1->at(i).first << ", ";
        }
        std::cout << std::endl << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
        popYResultStr << "Solution [" << optimizedFuncName <<  ", Y]: " << optimizedFunc(*gen1);
        write_text_to_log_file(popXResultStr.str());
        write_text_to_log_file(popYResultStr.str());
        std::cout << "\nExecution Time: " << endF1 - startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]" << std::endl << std::endl;
        popExecTimeStr << "Execution Time: " << endF1-startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]";
        write_text_to_log_file(popExecTimeStr.str());
    }

    MPI_Finalize();

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
