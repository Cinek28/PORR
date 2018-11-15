//
// Created by Damian on 2018-11-15.
//

#include "CoevolutionEngineST.h"
#include <math.h>

#define M_PI 3.14159265358979323846
#define M_E 2.718281828459

double CoevolutionEngineST::calculateF1(double x[], int n){
    double ex2 = 0, prodcosxi = 1;
    for(int i = 0; i < n; i++){
        ex2 += x[i] * x[i];
        prodcosxi = prodcosxi*cos(x[i]/i);
    }
    return ex2 / 40 + 1 - prodcosxi;

}

double CoevolutionEngineST::calculateF2(double x[], int n){
    double ex2 = 0, ecos2px = 0, exp1 = 0, exp2 = 0;
    for(int i = 0; i < n; i++){
        ex2 += x[i] * x[i];
        ecos2px += cos(2 * M_PI * x[i]);
    }
    exp1 = exp(-0.2 * sqrt(ex2 / n));
    exp2 = exp(ecos2px / n);
    return 20 * exp1 - exp2 + 20 + M_E;
}