//
// 神様目線
//
#include "simulator.hpp"

double God::generateCovvariance(std::vector<Cell> cells){
    //
    double sum_cov = 0;
    for(unsigned int i = 0;i < cells.size();i++){
        double x = cells[i].getCoverage() - CoverageAve;
        sum_cov += x * x;
    }
    CoverageVari = sum_cov/(cells.size());
    return CoverageVari;
}

double God::generateCovaverage(std::vector<Cell> cells){
    //
    double sum_cov = 0;
    for(unsigned int i = 0;i < cells.size();i++){
        sum_cov += cells[i].getCoverage();
    }
    CoverageAve = sum_cov/(cells.size());
    return CoverageAve;
}