/**
 * @file uav-cellinfo.cpp
 * @brief UAV Class：UAVが保持するセル情報まわりの関数ひとまとめ
 * @date 2017/12/27
 */
#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"
#include <algorithm>
#include <set>

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

	//エリア情報は全セル自分で初期化
	Areainfo.resize(params.getCellNum());
	for (auto &c : Areainfo){ c = 0; }
}

/**
 * @brief セルに訪問
 * @params c 訪問したセルのID
 * @detail 自身のカバレッジマップで自分の位置のカバレッジを1.0に
 */
void UAV::arrive(unsigned int c) {
	if (c >= coverageMap.size()) {
		std::cerr << "Index out of bounds: " << c << std::endl;
		return;  // またはエラー処理
	}
	//std::cout << "Number : "<<c << "\n";
	coverageMap[c] = 1;
}

/**
 * @brief カバレッジマップ上でカバレッジ減衰
 */
void UAV::attenuateCovMap() {
	//std::cout << "減衰!";
	for (unsigned int c = 0; c < coverageMap.size(); c++) {
		
		if (c != curCell) {
			if(Areainfo[c] == 0){
				coverageMap[c] -= params.getCovAttenuation();
				if (coverageMap[c] < 0) { coverageMap[c] = 0; }
			}else if(Areainfo[c] == 1){
				coverageMap[c] -= params.getCovAttenuation() * 2.0;
				if(coverageMap[c] < 0) { coverageMap[c] = 0;}
			}else{
				std::cout <<"errer";
			}
			
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
    for(unsigned int i = 0; i < coverageMap.size(); i++){
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
// double UAV::minadjscov(int num){
// 	//
// 	//int dst = rGenerator.intBetween(0, moveca2.size() - 1);
// 	std::vector<int> wasadjs = cKnow.getAdjCells(curCell);
// 	std::vector<int> adjs = cKnow.getAdjCells(num);
// 	double minValue = 1.00;
// 	// for(unsigned int i = 0; i < wasadjs.size();i++){
// 	// 	std::cout << "\nwasadjs " << wasadjs[i];
// 	// }

// 	// for(unsigned int i = 0; i < adjs.size();i++){
// 	// 	std::cout << "\nadjs " << adjs[i];
// 	// }

// 	for(unsigned int i = 0; i < adjs.size();i++){
// 		//
// 		for(unsigned int j = 0; j < wasadjs.size();j++){
// 			//
// 			if(adjs[i] != wasadjs[j]){
// 				if(coverageMap[adjs[i]] < minValue){
// 				//
// 				//std::cout << "\ncelladjs" << adjs[i] << " coverage " << coverageMap[adjs[i]];
// 				minValue = coverageMap[adjs[i]];
// 				}
// 			}else if(adjs[i] == 0 || adjs[i] == params.getCellCols() - 1 || adjs[i] == (params.getCellRows() - 1) * params.getCellCols() + 1 || adjs[i]  == params.getCellNum() - 1){
// 				//角の時
// 				std::cout << "\nUAV : "<< id <<"curcell :"<< curCell<<" 角です: " << adjs[i] ;
// 				minValue = coverageMap[adjs[i]];
// 			}
			
// 		}
		
// 	}

// 	return minValue;
// }


double UAV::minadjscov(int num){
	// std::cout << "\n----  uav " << id << " ----" << std::endl; 
	// std::cout << "num : "<< num << std::endl;
	std::vector<int> curadjs = cKnow.getAdjCells(curCell);
	std::vector<int> adjs = cKnow.getAdjCells(num);
	curadjs.push_back(curCell);
	std::set<int> Set1(curadjs.begin(), curadjs.end());

    std::vector<int> nums;
	// if (Set1.empty()) {
	// 	std::cerr << "Set1 is empty!" << std::endl;
	// }
	for (int i : adjs) {
    //std::cout << "Checking value: " << i << std::endl;
    if (Set1.find(i) == Set1.end()) {
       // std::cout << "Not in Set1, adding to nums: " << i << std::endl;
        nums.push_back(i);
    }
}
	

	// for(unsigned int i = 0; i < nums.size();i++){
	// 	std::cout << " nums: "<< nums[i] << std::endl;
	// }
	std::vector<double> numsCov;
	int intcurCell = curCell;
	int CellRows = params.getCellRows();
	int CellCols = params.getCellCols();
	if(!nums.empty()){
		for(unsigned int i = 0; i < nums.size();i++){
			numsCov.push_back(coverageMap[nums[i]]);
		}
	}else{
		//std::cout << "nums は空です"<< std::endl;
	}
	
	//std::cout << "UAV: "<< id << "num: "<< num <<"numsCov " << numsCov.size()<< std::endl;
	std::cout << CellCols - 1 << " " << (CellRows - 1) * CellCols << " " << CellCols * CellRows - 1  << std::endl;
	std::cout << CellCols  << " " << (CellRows - 2) * CellCols << " " << (CellRows - 1) * CellCols + 1 << std::endl;
	if(intcurCell == 1 || intcurCell == CellCols || intcurCell == (CellRows - 2) * CellCols || intcurCell == (CellRows - 1) * CellCols + 1){
		if(num == 0 || num == CellCols - 1 || num == (CellRows - 1) * CellCols || num == CellCols * CellRows - 1 ){
			numsCov.push_back(coverageMap[num]);
			//std::cout << " 角です" <<std::endl;
		}
	}
	
	for(int i = 0; i < params.getCellRows();i++){
		//std::cout << 2 * i * CellCols + 1<< " "<< 2 * i * CellCols - 2 << " "<< 2 * i * CellCols - 1 <<std::endl;
		if(intcurCell == 2 * i * CellCols + 1 || num == 2 * i * CellCols){
			numsCov.push_back(coverageMap[num]);
		}
		if(intcurCell == 2 * i * CellCols - 2 || num == 2 * i * CellCols - 1){
			numsCov.push_back(coverageMap[num]);
		}
	}

	std::sort(numsCov.begin(),numsCov.end());
	//std::cout << "check" << std::endl;
	// if(nums.empty()){
	// 	std::cout << "空です" << std::endl;
	// }
	// for(unsigned int i = 0; i < numsCov.size();i++){
	// 	std::cout << " numscov: "<< numsCov[i] ;
	// }
	//std::cout << CellCols << " " << CellCols* 2 - 2 << " " << (CellRows - 2) * CellCols << " " << (CellRows - 1) * CellCols + 1<< " " << (CellRows - 1) * CellCols - 2 << std::endl;
	//std::cout << "UAV: "<< id <<"numsCov " << numsCov.size()<< std::endl;
	return numsCov[0];
}

/**
 * @brief 重点探索エリアであることの判別
 * 	Areainfo = 1の時　「重点探索エリア」
 */

void UAV::setAreainfo(Cell cell){
	if(cell.getArea() == 1){
		Areainfo[cell.getId()] = cell.getArea();
		std::cout << "\nUAVが到着した重点探索エリアは :"<< cell.getId()<<"です";
	}
	
}
