#pragma once
/**
 * @file cell.hpp
 * @brief セル（シミュレーション領域を正六角形で分割）クラス
 *  - 縦params.rows個×横params.cols個に配置
 *  - 最も左上のセルがID0, 右に向けて1, 2, 3, ...
 *  - セルID0の右下に接しているセルがID(cols+1)
 * @date 2024/6/19
 */
#include <vector>

class Cell
{
    int id;
    double coverage;  //カバレッジ値[0, 1]
    std::vector<int> nbrCells; //隣接セル群のID

public:
    Cell(){
        coverage = 0;
    }
    ~Cell() = default;  //コンストラクタとデストラクタ

    //getter
    int getId(){ return id; }
    double getCoverage(){ return coverage; }
    std::vector<int> getNbrCells(){ return nbrCells; }

    //setter
    void setID(int i){ id = i; }

    void attenuateCoverage();
    void UAVarrived();
    void UAVarrived(int); 
};