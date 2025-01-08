/**
* @file common_knowledge.cpp
* @brief 全UAV／シンクにとって事前に知っている共通知識クラス
* @date 2017/4/19
*/
#include <queue>
#include <vector>
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
	// for(unsigned int j = 0; j < uavs.size(); j++){
	// 	std::cout << uavs[j].getID() <<std::endl;;
	// }	
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

/**
 * セル同士の位置関係
 * 方向を数字対応で出力,基本直線上を想定している
 * 右：0　右上：1　上：2　左上：3　左：4　左下：5　下：6　右下：7 
 * 方向が定まらなかったので,独自の方向指定をする
 * 左上左:8 約上:9 右上右:10 左下左:11 約下:12 右下右:13
 * 初めの引数を基準に位置関係を出力する
 */

int CommonKnowledge::getDirection(int num , int num2){
	std::vector<int> numups;
	std::vector<int> numdowns;
	std::vector<int> numtoprights;
	std::vector<int> numtoplefts;
	std::vector<int> numdownrights;
	std::vector<int> numdownlefts;
	int numRow = num / params.getCellCols();//numの行
	int num2Row = num2 / params.getCellCols();//num2の行
	int up = num;//上側のセル番号用
	int down = num;//下側セル番号用

	std::vector<int> mostrights; //偶数行の右端のセル番号を格納
	std::vector<int> mostlefts; //奇数行の左端のセル番号を格納

	for(int i = 0;i < params.getCellRows() + 1;i++){
		if(i % 2 == 0){
			//
			//std::cout << "mostright: "<< i * params.getCellCols() - 1 << std::endl;
			if(i * params.getCellCols() - 1 > 0){
				mostrights.push_back(i * params.getCellCols() - 1);
			}
			
		}else{
			//
			//std::cout << "mostleft: "<< (i - 1) * params.getCellCols() << std::endl;
			if((i - 1) * params.getCellCols() > 0){
				mostlefts.push_back((i - 1) * params.getCellCols());
			}
			
		}
	}

	if(numRow == num2Row ){
		//同じ列の場合
		if(num < num2){
			//右
			return 0;
		}else{
			//左
			return 4; 
		}
	}
	
	for(int i = 0; i < params.getCellRows();i++){
		//numの上側、下側をそれぞれ格納
		up -= params.getCellCols() * 2;
		
		if(up < num && up >= 0){
			numups.push_back(up);
			//std::cout << "up: "<< up << std::endl;
		}

		down += params.getCellCols() * 2;

		if(down > num && down < params.getCellNum()){
			numdowns.push_back(down);
			//std::cout << "down: "<<down << std::endl;
		}
	}

	for(unsigned int i = 0; i < numups.size();i++){
		//前上にある場合
		if(num2 == numups[i]){
			return 2;
		}
	}

	for(unsigned int i = 0; i < numdowns.size();i++){
		//真下にある場合
		if(num2 == numdowns[i]){
			return 6;
		}
	}

	int sub = num;//右上,左上用
	int afsub = -2;

	for(int i = 0;i < params.getCellRows();i++){
		//左上
		int row = numRow - i;
		if(row % 2 == 0){
			//
			sub -= params.getCellCols() + 1;
		}else{
			//
			sub -= params.getCellCols();
		}
		int key = -2;
		for(unsigned int j = 0;j < mostlefts.size();j++){
			if(num == mostlefts[j]){
				//std::cout << "抜け出し１"<<std::endl;
				key = 0;
				break;
			}
			
			if(mostlefts[j] == afsub){
				//std::cout << "抜け出し2"<<std::endl;
				key = 0;
				break;
			}
		}
		if(key == -2 && sub >= 0 && sub < num){
			//std::cout << "topleft: "<< sub <<std::endl;
			numtoplefts.push_back(sub);
		}

		if(key == 0){
			break;
		}

		afsub = sub;
	}

	for(int i = 0;i < params.getCellRows();i++){
		//右上
		int row = numRow - i;
		if(row % 2 == 0){
			//奇数
			sub -= params.getCellCols();
		}else{
			//偶数
			sub -= params.getCellCols() - 1;
		}
		int key = -2;
		for(unsigned int j = 0;j < mostrights.size();j++){
			if(num == mostrights[j]){
				//std::cout << "抜け出し１"<<std::endl;
				key = 0;
				break;
			}
			if(mostrights[j] == afsub){
				//std::cout << "抜け出し2"<<std::endl;
				key = 0;
				break;
			}
		}
		if(key == -2&&sub >= 0 && sub < num){
			//std::cout << "topright: "<< sub <<std::endl;
			numtoprights.push_back(sub);
		}
		if(key == 0){
			break;
		}

		afsub = sub;
	}
	

	for(unsigned int i = 0; i < numtoplefts.size();i++){
		//左上にある場合
		if(num2 == numtoplefts[i]){
			return 3;
		}
	}

	for(unsigned int i = 0; i < numtoprights.size();i++){
		//右上にある場合
		if(num2 == numtoprights[i]){
			return 1;
		}
	}

	//
	int add = num;
	int addmemo = -2;
	for(int i = 0;i < params.getCellRows();i++){
		//左下
		int row = numRow + i;
		if(row % 2 == 0){
			//奇数
			add += params.getCellCols()  - 1;
		}else{
			//偶数
			add += params.getCellCols();
		}
		int key = -2;
		for(unsigned int j = 0;j < mostrights.size();j++){
			//std::cout << mostlefts[j] << std::endl;
			//std::cout << addmemo << std::endl;
			if(num == 0||num == mostlefts[j]){
				//std::cout << "抜け出し１"<<std::endl;
				key = 0;
				break;
			}
			if(add < params.getCellNum() && add > num){
				//std::cout << "downleft: "<< add <<std::endl;
				numdownlefts.push_back(add);
			}
			if(mostlefts[j] == add||add > params.getCellNum()){
				//std::cout << "抜け出し2"<<std::endl;
				key = 0;
				break;
			}
		}
		if(key == 0){
			break;
		}
		addmemo = add;
	}

	add  = num;
	addmemo  = -2;

	for(int i = 0;i < params.getCellRows();i++){
		//右下
		int row = numRow + i;
		if(row % 2 == 0){
			//奇数
			add += params.getCellCols();
		}else{
			//偶数
			add += params.getCellCols() + 1;
		}
		int key = -2;
		for(unsigned int j = 0;j < mostrights.size();j++){
			if(num == mostrights[j]){
				//std::cout << "抜け出し１"<<std::endl;
				key = 0;
				break;
			}else if(mostrights[j] == addmemo){
				//std::cout << "抜け出し2"<<std::endl;
				key = 0;
				break;
			}
			
		}
		if(key == -2 && add < params.getCellNum() && add > num){
				//std::cout << "downright: "<< add <<std::endl;
				numdownrights.push_back(add);
		}
		if(key == 0){
			break;
		}
		addmemo = add;
	}

	for(unsigned int i = 0; i < numdownlefts.size();i++){
		//左下にある場合
		if(num2 == numdownlefts[i]){
			return 5;
		}
	}

	for(unsigned int i = 0; i < numdownrights.size();i++){
		//右下にある場合
		if(num2 == numdownrights[i]){
			return 7;
		}
	}

	//左上左
	std::vector<int> leftedges;
	for(int i = 0;i < params.getCellRows();i++){
		leftedges.push_back(i * params.getCellCols());
	}
	std::vector<int> toplefttops;
	for(unsigned int i = 0; i < numtoplefts.size();i++){
		int limit = numtoplefts[i] - leftedges[numtoplefts[i] / params.getCellCols()];
		//std::cout << "limit: " <<limit << std::endl;
		for(int j = 0; j < limit;j++){
			int value = leftedges[numtoplefts[i] / params.getCellCols()] + j;
			//std::cout << "toplefttop: " <<value << std::endl;
			toplefttops.push_back(value);
		}
	}
	
	for(unsigned int i = 0;i < toplefttops.size();i++){
		if(num2 == toplefttops[i]){
			return 8;
		}
	}

	//右上右
	std::vector<int> rightedges;
	for(int i = 1;i < params.getCellRows() + 1;i++){
		//std::cout << "rightedge: " <<i * params.getCellCols() - 1 << std::endl;
		rightedges.push_back(i * params.getCellCols() - 1);
	}

	std::vector<int> toprighttops;
	for(unsigned int i =0;i < numtoprights.size();i++){
		int limit = rightedges[numtoprights[i] / params.getCellCols()] - numtoprights[i];
		//std::cout << "limit: " <<limit << std::endl;
		for(int j = 0;j < limit;j++){
			int value = rightedges[numtoprights[i] / params.getCellCols()] - j;
			//std::cout << "value: " <<value << std::endl;
			toprighttops.push_back(value);
		}
	}

	for(unsigned int i = 0;i < toprighttops.size();i++){
		if(num2 == toprighttops[i]){
			return 10;
		}
	}

	//左下左
	std::vector<int> leftdownlefts;
	for(unsigned int i = 0;i < numdownlefts.size();i++){
		int limit = numdownlefts[i] - leftedges[numdownlefts[i] / params.getCellCols()];
		//std::cout << "limit: " <<limit << std::endl;
		for(int j = 0; j < limit;j++){
			int value = leftedges[numdownlefts[i] / params.getCellCols()] + j;
			//std::cout << "value: " <<value << std::endl;
			leftdownlefts.push_back(value);
		}
	}

	for(unsigned int i = 0; i < leftdownlefts.size();i++){
		if(num2 == leftdownlefts[i]){
			return 11;
		}
	}

	//右下右
	std::vector<int> rightdownrights;
	for(unsigned int i = 0;i < numdownrights.size();i++){
		int limit = rightedges[numdownrights[i] / params.getCellCols()] - numdownrights[i];
		//std::cout << "limit: " <<limit << std::endl;
		for(int j = 0; j < limit;j++){
			int value = rightedges[numdownrights[i] / params.getCellCols()] - j;
			//std::cout << "rightdownright: " <<value << std::endl;
			rightdownrights.push_back(value);
		}
	}

	for(unsigned int i = 0; i < rightdownrights.size();i++){
		if(num2 == rightdownrights[i]){
			return 13;
		}
	}

	if(num2 < num){
		//約上
		return 9;
	}else if( num2 > num){
		//約下
		return 12;
	}
	return 100;
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
	return -1;
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
