/**
 * @file cell.cpp
 * @biref セルクラス
 * @date 2024/06/18
 */
#include "simulator.hpp"
#include "cell.hpp"
#include <algorithm>

//カバレッジ減衰
void Cell::attenuateCoverage(){
    if(area == 0 ){
        coverage -= params.getCovAttenuation();
    }else{
        coverage -= params.getCovAttenuation() * 2.0;
    }
    if (coverage < 0){ coverage = 0; }
}

/**
 * @brief UAVが到着
 *
 * カバレッジを1に更新
 */
void Cell::UAVarrived() {
	coverage = 1;
}
void Cell::UAVarrived(int u) {
	coverage = 1;
}

void Cell::setArea(){
    area = 1;
    std::cout << "\n重点探索エリアは"<< id << "です";
}