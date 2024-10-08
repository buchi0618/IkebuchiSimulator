/**
 * @file uav-cellinfo.cpp
 * @brief UAV Class：UAVが保持するセル情報まわりの関数ひとまとめ
 * @date 2017/12/27
 */
#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"

/**
 * @brief 初期化
 */
void UAV::init_cellInfo() {
	// カバレッジマップは全セル0で初期化
	coverageMap.resize(params.getCellNum());
	for (auto &c : coverageMap) { c = 0.0; }

	// 担当マップは全セル自分で初期化
	UavAssignment.resize(params.getCellNum());
	for (auto &c : UavAssignment) { c = id; }
}

/**
 * @brief セルに訪問
 * @params c 訪問したセルのID
 * @detail 自身のカバレッジマップで自分の位置のカバレッジを1.0に
 */
void UAV::arrive(unsigned int c) {
	coverageMap[c] = 1;
}

/**
 * @brief カバレッジマップ上でカバレッジ減衰
 */
void UAV::attenuateCovMap() {
	for (unsigned int c = 0; c < coverageMap.size(); c++) {
		if (c != curCell) {
			coverageMap[c] -= params.getCovAttenuation();
			if (coverageMap[c] < 0) { coverageMap[c] = 0; }
		}
	}
}

/**
 * @brief カバレッジ平均の計算
 */

double UAV::covAve(int start, int end){
	if(start < 0 || start >= params.getCellNum() || end < 0 || end >=params.getCellNum() || start > end){
		//debug
		return 2;
	}
	double total = 0.0;
	double num = 0.0;
	for(int i = start; i <= end; i++){
		total += coverageMap[i];
		num++;
	}
	//std::cout <<"coverageAve: "<< total / num <<endl;
	return total / num;
}

/**
 * @brief　カバレッジ共有
 */
void UAV::shareCovmap(UAV &uav1,UAV &uav2){
    for(unsigned int i; i < coverageMap.size(); i++){
        if(uav1.coverageMap[i] < uav2.coverageMap[i]){
            //探索されているかどうかの更新
            uav1.coverageMap[i] = uav2.coverageMap[i];
        }else{
            //
            uav2.coverageMap[i] = uav1.coverageMap[i];
        }
    }
	//std::cout << "通信完了！"<< std::endl;
}

/**
 * @brief  隣接セルの最小値を持つセル番号を出力
 */
double UAV::minadjscov(int cur, int num){
	//
	//int dst = rGenerator.intBetween(0, moveca2.size() - 1);
	std::vector<int> wasadjs = cKnow.getAdjCells(cur);
	std::vector<int> adjs = cKnow.getAdjCells(num);
	double minValue = 1.00;
	for(unsigned int i = 0; i < adjs.size();i++){
		//
		for(unsigned int j = 0; j < wasadjs.size();j++){
			//
			if(adjs[i] != wasadjs[j]){
				if(coverageMap[adjs[i]] < minValue){
				//
				minValue = coverageMap[adjs[i]];
				}
			}
			
		}
		
	}

	return minValue;
}