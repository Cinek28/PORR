//
// Created by marcin on 17.12.18.
//

#ifndef PORR_UTILS_H
#define PORR_UTILS_H

void initializeOptimizationFunctions(std::function<double(Genotype)> &optimizedFunc1,
                                     std::function<double(Genotype)> &optimizedFunc2) {
    optimizedFunc1= [](Genotype genotype)
    {
        double sum = 0, prod = 1.0;//TODO DONE simd
        if(OMP_ON){
//            #pragma omp simd
            for(unsigned int i = 0; i < genotype.size();++i)
            {
                sum += pow(genotype[i].first,2);
                prod *= cos(genotype[i].first/(i+1));
            }
        }
        else{
            for(unsigned int i = 0; i < genotype.size();++i)
            {
                sum += pow(genotype[i].first,2);
                prod *= cos(genotype[i].first/(i+1));
            }
        }

        return 1./40.0*sum+1-prod;
    };
    optimizedFunc2= [](Genotype genotype)
    {
        double ex2 = 0, ecos2px = 0;//TODO DONE simd
        if(OMP_ON){
//            #pragma omp simd
            for (unsigned int i = 0; i < genotype.size(); ++i) {
                ex2 += pow(genotype[i].first, 2);
                ecos2px += cos(2 * M_PI * genotype[i].first);
            }
        }
        else{
            for (unsigned int i = 0; i < genotype.size(); ++i) {
                ex2 += pow(genotype[i].first, 2);
                ecos2px += cos(2 * M_PI * genotype[i].first);
            }
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
                         bool isOMPOn) {

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
               << ", criteria: " << cov.enum2cChar[criteria];
    write_text_to_log_file(popInitStr.str());
    int iterationsCount;
    double startF1 = omp_get_wtime( );
    const Genotype* gen1 = cov.solve(optimizedFunc, criteria, mutationVariance, iterationsCount, isOMPOn);
    double endF1 = omp_get_wtime( );
    double singleIterationExecutionTime = 1000 * (double)(endF1-startF1)/(double)iterationsCount;
    std::ostringstream popResultStr, popExecTimeStr;
    std::cout << "Solution: " << std::endl;
    popResultStr << "Solution: ";
    for(unsigned int i = 0; i < gen1->size(); ++i)
    {
        std::cout << gen1->at(i).first << ", ";
        popResultStr << gen1->at(i).first << ", ";
    }
    write_text_to_log_file(popResultStr.str());
    std::cout << "\nExecution Time: " << endF1 - startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]" << std::endl;
    popExecTimeStr << "Execution Time: " << endF1-startF1 << " [s], Single Iteration Execution Time: " << singleIterationExecutionTime << " [ms]";
    write_text_to_log_file(popExecTimeStr.str());
    return true;
}

bool test()
{
    bool isOMPOn = false;
    bool calculationsPerformed;
//        for(int i = 1; i <= 8; ++i)
//        {
    for(int j = 1;j <= 15; ++j)
    {
//                omp_set_num_threads(i);
        cov.setDesiredError(0.1);
        cov.setNoOfItersWithoutImprov(800);
        calculationsPerformed = performCalculations(cov, optimizedFunc1, 100, 50, j, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function1", 0.3, isOMPOn);
        if(!calculationsPerformed)
            return -1;
        calculationsPerformed = performCalculations(cov, optimizedFunc2, 100, 50, j, -40, 40, CoevolutionEngineST::NO_OF_ITERS_WITHOUT_IMPROV, "Function2", 0.3, isOMPOn);
        if(!calculationsPerformed)
            return -1;
    }
//        }
}



#endif //PORR_UTILS_H
