/**
 * @file uav.cpp
 * @brief UAV Class
 * @date 2017/04
 */
#include <iomanip>
#include <algorithm>
#include <cstdlib>
#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"

 /**
  * @brief UAVを初期位置に配置
  *
  * @detail 全セルからランダムに選択したセルを現在地に設定
  */
void UAV::locateAtInitialCell(int cellNum) {
	int candidate = rGenerator.intBetween(0, cellNum - 1);
	curCell = candidate;
}

/**
 * @brief もろもろ初期化
 * @params i UAVのID
 */
void UAV::init(unsigned int i) {
	id = i;
	init_move();	// 移動関係初期化
	init_cellInfo();	// セル情報初期化
}

// for debugging //////////////////////////////////////////////////////
/**
 * @brief 全情報を出力
 */
void UAV::showAllInfo() {
	std::cout << "\n showAllInfo (UAV " << id << ")";

	// 現在地＆移動ログ
	std::cout << "\n  currentCell:" << curCell << " (";
	for (unsigned int i = 0; i < moveLog.size(); i++) {
		std::cout << "<" << moveLog[i];
	}
	if (moveLog.empty()) { std::cout << "no moveLog"; }
	std::cout << ")";

	// セル情報
	unsigned int width = ((params.getUavNum() - 1) / 10) + 1;
	std::string temp = std::to_string(params.getCovAttenuation());
	std::reverse(temp.begin(), temp.end());
	bool flag = false;
	int precision = 0;
	for (int i = 0; ; i++) {
		if (flag) {
			precision++;
			if (temp[i] == '.') { break; }
		}
		if (!flag && temp[i] != '0') {
			flag = true;
			if (temp[i] == '.') { break; }
		}
		if (i == 16) {
			std::cout << "\n Error! (at UAV::showAllInfo()";
			std::exit(1);
		}
	}
	std::cout << "\n  cellInfo (assignedUAV:coverage):";
	for (unsigned int i = 0; i < coverageMap.size(); i++) {
		if (i % params.getCellCols() == 0) {
			std::cout << "\n ";
			if ((i / params.getCellCols()) % 2 == 1) { std::cout << "  "; }
		}
		std::cout << "   " << setw(width) << right << UavAssignment[i] << ":";
		std::cout << fixed << setprecision(precision) << coverageMap[i];
	}

	std::cout << std::endl;

}
