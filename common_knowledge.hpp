#pragma once
/**
 * @file common_knowledge.hpp
 * @brief 全UAV／シンクにとって事前に知っている共通知識クラス
 * @date 2017/4/18
 */
#include <vector>
#include "simulator.hpp"
#include "uav.hpp"

class CommonKnowledge
{
	/**
	 * セルの隣接関係
	 *  1:右(r)，2:右上(ur), 3:左上(ul), -1:左(l), -2:左下(ll), -3:右下(lr)
	 */
	std::vector< std::vector<int> > adjCells;

	// セル間の距離
	std::vector< std::vector<int> > distBetweenCells;

	//UAVがいるかどうか：１はいる、０はいない
	std::vector<int> uavexsistcells;
	
public:
	CommonKnowledge() = default;
	~CommonKnowledge() = default;

	void init(Parameters);	// initialize
	void generateAdjCells(int, int);
	void generateDistBetweenCells();
	void generateUavexsistCells(std::vector<UAV>);

	bool isAdjacent(int, int);	// セルは隣接している？
	int adjCellOf(int, std::string);	// 特定セルの隣接セルID（方向指定）
	std::vector<int> getAdjCells(int);	// あるセルに隣接しているセル群のID
	int getDistance(int, int);	// セル間の距離
	void setexsistuav(UAV);
	bool exsistuav(int);//特定のセルにUAVがいるかどうか

	// for debugging ////////////////////////////////////////////////
	void showAllCellsInfo(void);	// show adjCells;
};

