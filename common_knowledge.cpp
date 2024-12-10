/**
* @file common_knowledge.cpp
* @brief 全UAV／シンクにとって事前に知っている共通知識クラス
* @date 2017/4/19
*/
#include <queue>
#include "simulator.hpp"

// セル隣接行列 adjCells を生成
void CommonKnowledge::generateAdjCells(int cols, int rows) {
#ifdef DEBUG
	std::cout << "\nstart: CommonKnowledge::generateAdjCells()";
#endif

	// セル数分のサイズ確保
	int arrSize = cols * rows;

	// まずはゼロで初期化
	adjCells.resize(arrSize);
	for (int i = 0; i < arrSize; i++) {
		adjCells[i].resize(arrSize);
		for (int j = 0; j < arrSize; j++) { adjCells[i][j] = 0; }
	}

	// セルiと隣接するセルは…
	for (int i = 0; i < arrSize; i++) {
		// 左隣 (i-1)：左端セル以外は左(-1)隣接
		if (i % cols != 0) { adjCells[i][i - 1] = -1; }

		// 右隣 (i+1)：右端セル以外は右(1)隣接
		if (i % cols != cols - 1) { adjCells[i][i + 1] = 1; }

		// 最上段を0列目として、偶数列の場合 → 上下の段は右にズレて配置されている
		if ((i / cols) % 2 == 0) {
			if (i - cols - 1 >= 0 && i % cols != 0) { adjCells[i][i - cols - 1] = 3; }	// 左上(3)隣接（左端セル以外）
			if (i + cols - 1 < arrSize && i % cols != 0) { adjCells[i][i + cols - 1] = -2; }	// 左下(-2)隣接（左端セル以外）
			if (i - cols >= 0) { adjCells[i][i - cols] = 2; }	// 右上(2)隣接
			if (i + cols < arrSize) { adjCells[i][i + cols] = -3; }	// 右下(-3)隣接
		}
		// 大上段を0列目として、奇数列の場合 → 上下の段は左にズレて配置されている
		else {
			if (i - cols >= 0) { adjCells[i][i - cols] = 3; }	// 左上(3)隣接
			if (i + cols < arrSize) { adjCells[i][i + cols] = -2; }	// 左下(-2)隣接
			if (i - cols + 1 >= 0 && i % cols != cols - 1) { adjCells[i][i - cols + 1] = 2; }	// 右上(2)隣接（右端セル以外）
			if (i + cols + 1 < arrSize && i % cols != cols - 1) { adjCells[i][i + cols + 1] = -3; }	// 右下(-3)隣接（右端セル以外）
		}
	}
#ifdef DEBUG
	std::cout << "\nend: CommonKnowledge::generateAdjCells()";
#endif
}

/**
 * adjCellsを用いてセル間距離行列 distBetweenCells を生成
 */
void CommonKnowledge::generateDistBetweenCells() {
#ifdef DEBUG
	std::cout << "\nstart: CommonKnowledge::generateDistBetweenCells()";
#endif

	// 初期化
	distBetweenCells.resize(params.getCellNum());
	for (unsigned int i = 0; i < distBetweenCells.size(); i++) {
		distBetweenCells[i].resize(params.getCellNum());
		for (unsigned int j = 0; j < distBetweenCells[i].size(); j++) {
			distBetweenCells[i][j] = -1;
		}
	}

	// 距離の算出＆格納
	std::queue<int> fQueue, sQueue;
	int distance;
	for (unsigned int i = 0; i < distBetweenCells.size(); i++) {
		fQueue.push(i);
		distance = 0;
		distBetweenCells[i][i] = 0;
		
		while (!(fQueue.empty() && sQueue.empty())) {
			if (fQueue.empty()) {
				fQueue.swap(sQueue);
				distance++;
			}
			else {
				int next = fQueue.front();
				fQueue.pop();
				std::vector<int> adjs = getAdjCells(next);
				for (unsigned int j = 0; j < adjs.size(); j++) {
					if (distBetweenCells[i][adjs[j]] == -1) {
						sQueue.push(adjs[j]);
						distBetweenCells[i][adjs[j]] = distance + 1;
					}
				}
			}
		}
	}

#ifdef DEBUG
	std::cout << "\nend: CommonKnowledge::generateDistBetweenCells()";
#endif
}
/**
 * 
 */

void CommonKnowledge::generateUavexsistCells(std::vector<UAV> uavs){
	//UAV存在行列を生成
	uavexsistcells.resize(params.getCellNum());
	for(unsigned int i = 0; i < uavexsistcells.size();i++){
		uavexsistcells[i] = 0;
	}

	for(unsigned int i = 0; i < uavexsistcells.size(); i++ ){
		for(unsigned int j = 0; j < uavs.size(); j++){
			if(uavs[j].getCurCell() == i){
				uavexsistcells[i]  = 1;
				//std::cout << "\n UAVがいるセルは"<< i;
			}
		}	
	}
}
// 共有情報を初期化
void CommonKnowledge::init(Parameters params) {
#ifdef DEBUG
	std::cout << "\nstart: CommonKnowledge::init()";
#endif

	// セル隣接行列 adjCells を生成
	generateAdjCells(params.getCellCols(), params.getCellRows());

	// セル間距離行列 distBetweenCells を生成
	generateDistBetweenCells();

#ifdef DEBUG
	std::cout << "\nend: CommonKnowledge::init()";
#endif
}

// セルiとjは隣接している？
bool CommonKnowledge::isAdjacent(int i, int j) {
	if (adjCells[i][j] == 0) { return false; }
	else { return true; }
}

/**
 * セルiの隣接セルID
 * @param i 基準になるセルのID
 * @param direction 方向（右から反時計回りに r / ur / ul / l / ll / lr)
 * @return 該当する隣接セルが存在する場合はそのセルのID、存在しない場合は -1
 */
int CommonKnowledge::adjCellOf(int i, std::string direction) {
	std::vector<int> adjs = adjCells[i];
	for (unsigned int j=0; j < adjs.size(); j++) {
		if ((direction == "r" && adjs[j] == 1) ||
			(direction == "ur" && adjs[j] == 2) ||
			(direction == "ul" && adjs[j] == 3) ||
			(direction == "l" && adjs[j] == -1) ||
			(direction == "ll" && adjs[j] == -2) ||
			(direction == "lr" && adjs[j] == -3)) {
			return j;
		}
	}
	return -1;	// not found
}

// セルiに隣接しているセル群は？
std::vector<int> CommonKnowledge::getAdjCells(int i) {
	std::vector<int> result;

	std::vector<int> adjs = adjCells[i];
	for (unsigned int j = 0; j < adjs.size(); j++) {
		if (adjs[j] != 0) {
			result.push_back(j);
		}
	}
	return result;
}

/**
 * セル間の距離
 * @params i,j 距離を求めたい2セル
 * @return セルiとセルjの距離
 */
int CommonKnowledge::getDistance(int i, int j) {
	return distBetweenCells[i][j];
}

/**
 * そのセルにUAVがいるかどうか
 */
void CommonKnowledge::setexsistuav(UAV uavs){
	//
	std::deque<int> logs = uavs.getmovelog();
	for(unsigned int i = 0; i < uavexsistcells.size(); i++ ){
		
		if(static_cast<unsigned int>(logs.front()) == i){
			uavexsistcells[i] = 0;
			//std::cout << "\nここには存在していません" << i;
		}
		if(uavs.getCurCell() == i){
			uavexsistcells[i]  = 1;
			//std::cout << "\n UAVがいるセルは"<< i;
		}
	}
	//std::cout << "\n";
	// for(unsigned int i = 0; i < uavexsistcells.size(); i++){
	// 	std::cout << " "<<uavexsistcells[i];
	// }
	//std::cout << " cell0 :" << uavexsistcells[0]<< "cell 20:" << uavexsistcells[20];
}

bool CommonKnowledge::exsistuav(int cell_num){
	
	/*std::cout << "\n";
	 for(unsigned int i = 0; i < uavexsistcells.size(); i++){
	 	std::cout << " "<<uavexsistcells[i];
	 }*/
	//std::cout << " cell0 :" << uavexsistcells[0]<< "cell 20:" << uavexsistcells[20];
	//std::cout << "\ncell" << cell_num<< " exsist:"<< uavexsistcells[0];
	if(uavexsistcells[cell_num] == 0){
		return false;//inai
	}else if(uavexsistcells[cell_num] == 1){
		return true;//iru
	}
	
}

// for debugging /////////////////////////////////////////////////////
/** 
 * 隣接行列を全出力
 */
void CommonKnowledge::showAllCellsInfo() {
#ifdef DEBUG
	std::cout << "\nadjCells";
	for (int i = 0; i < params.getCellNum(); i++) {
		std::cout << "\ncell" << i << ":";
		for (int j = 0; j < params.getCellNum(); j++) {
			if (adjCells[i][j] >= 0) { std::cout << " "; }
			std::cout << " " << adjCells[i][j];
		}
	}
	std::cout << std::endl;
#endif
}
