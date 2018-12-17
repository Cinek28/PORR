#include <chrono>
#include <iostream>
#include <QApplication>
#include <fstream>
#include <iomanip>
#include <cstring>
#include "Widget.h"
#include "CoevolutionEngineST.h"
#include "utils.h"

int main(int argc, char* argv[]) {

    std::string args;
    if(argc > 1)
    {
        args = argv[1];
    }

    CoevolutionEngineST cov;
    std::function<double(Genotype)> optimizedFunc1;
    std::function<double(Genotype)> optimizedFunc2;
    initializeOptimizationFunctions(optimizedFunc1, optimizedFunc2);

    if(atoi(args.c_str()))
    {
        QApplication a(argc, argv);
        Widget w;
        w.show();
        return a.exec();
    }
    else
    {
        return test();
    }
}