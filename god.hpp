#pragma once
//
// 神様目線
//
#include"simulator.hpp"
#include<algorithm>
#include<vector>
#include"cell.hpp"

class God{
    double CoverageAve;
    double CoverageVari;

public:
    God(){
        CoverageAve  = 0;
        CoverageVari = 0;
    }
    ~God() = default;
    double generateCovvariance(std::vector<Cell>);
    double generateCovaverage(std::vector<Cell>);
    
};