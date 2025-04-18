/**
 * @file uav-move.cpp
 * @brief UAV Class：UAVの移動関係の関数ひとまとめ
 * @date 2017/12/26
 */
#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"
#include <algorithm>

/**
 * @brief 初期化
 * @detail 最初は停止状態
 */
void UAV::init_move() {
	mMode = MMode::STOP;
	mDirection = MDirection::STOP;
}

/**
 * @brief 1セル分移動
 */
void UAV::move() {
	// 現在地をログに追加
	pushMoveLog(curCell);

	// とりあえずランダムウォーク
	switch (mMode) {
	case MMode::STOP:
		mMode = MMode::MOVEIKEBUCHI;
	case MMode::RANDOMWALK:
		moveRandomWalk();
		break;
	case MMode::ROUNDTRIP:
		moveRoundTrip();
		break;
	case MMode::NEXTLOW:
		moveNextlowcell();
		break;
	case MMode::MOVEITO:
		moveito();
		break;
	case MMode::MOVEIKEBUCHI:
		moveikebuchi();
		break;
	default:
		std::cout << "Invalid Mobility Mode! (at UAV::move())" << std::endl;
		exit(1);
	}

	// 到着処理
	//std::cout << "\nuav" << id << " : "<<curCell;
	arrive(curCell);	// 到着したセルのカバレッジを1に
	attenuateCovMap();	// カバレッジマップでカバレッジ減衰
}

/**
 * @brief ランダムウォーク
 * 
 * @detail 現在地からの隣接セルのうち、ランダムに選択したものに現在地を再設定
 */
void UAV::moveRandomWalk() {
	// 現在地からの隣接セル群を取得
	std::vector<int> nbrs = cKnow.getAdjCells(curCell);
	int dst = rGenerator.intBetween(0, nbrs.size() - 1);

	curCell = nbrs[dst];
}

/**
* @brief 往復モード
*
* @detail 左右に移動して、突き当たったら上下列に移動して戻る
*/
void UAV::moveRoundTrip() {
}

/**
 * @brief 移動ログにセルを追加
 * @params c 追加するセルのID
 * @detail 要素数は params.getUavMoveLogNum() に限定
 */
void UAV::pushMoveLog(unsigned int c) {
	moveLog.push_front(c);
	if (moveLog.size() > params.getUavMoveLogNum()) { moveLog.pop_back(); }

}

/**
 * @brief 隣接セルのカバレッジが一番低い方向へ移動
 * 
 */
void UAV::moveNextlowcell(){
	std::vector<int> adjs = cKnow.getAdjCells(curCell);//現在地の隣接セル
	double cvr = 1.00 ; //
	int dst = 0; //
	for(unsigned int i = 0; i < adjs.size(); i++){
		if(coverageMap[i] < cvr){

				cvr = coverageMap[i];
				dst = i;
		}
	}
	curCell = adjs[dst];
}

/**
 * @brief ito手法 改
 */

void UAV::moveito(){
	std::vector<int> adjs = cKnow.getAdjCells(curCell);//現在地の隣接セル
	std::vector<double> adjsCov;//隣接セルのカバレッジの値
	std::vector<int> moveca;//移動候補
	moveca.push_back(curCell);
	std::vector<int> movepriority;

	for(unsigned int i = 0; i < adjs.size(); i++){
		//
		adjsCov.push_back(coverageMap[adjs[i]]);
	}

	std::vector<std::pair<double,int>> CellnumArray;
	for (unsigned int i = 0; i < adjsCov.size(); i++ ){
		CellnumArray.emplace_back(adjsCov[i],adjs[i]);
	}

	std::sort(CellnumArray.begin(), CellnumArray.end(), [this](const auto& a, const auto& b) {
        if (a.first != b.first) {
            return a.first < b.first; // 値1で昇順
        }
        return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
    });

	//std::cout << "UAV: "<< id << "CellnumArray: "<< CellnumArray.size() << std::endl;

	for(unsigned int i = 0; i < adjs.size();i++){
		if(cKnow.exsistuav(CellnumArray[i].second) == false){
			curCell = CellnumArray[i].second;
			return;
		}else{
			//std::cout << "UAV EXSIST!!" <<std::endl;
		}
	}
}

/**
 * @brief ikebuchi手法
 */

void UAV::moveikebuchi(){
	//
	double diff = 1.00 - params.getthreshold();
	double backline = params.getthreshold() + (params.getCovAttenuation() * diff );
	
	if(TcCellnum < 0){
		std::vector<int> adjs = cKnow.getAdjCells(curCell);//現在地の隣接セル
		std::vector<double> adjsCov;//隣接セルのカバレッジの値
		std::vector<int> moveca;//移動候補
		moveca.push_back(curCell);
		std::vector<int> movepriority;

		for(unsigned int i = 0; i < adjs.size(); i++){
			//
			adjsCov.push_back(coverageMap[adjs[i]]);
		}

		std::vector<std::pair<double,int>> CellnumArray;
		for (unsigned int i = 0; i < adjsCov.size(); i++ ){
			CellnumArray.emplace_back(adjsCov[i],adjs[i]);
		}

		std::sort(CellnumArray.begin(), CellnumArray.end(), [this](const auto& a, const auto& b) {
			if (a.first != b.first) {
				return a.first < b.first; // 値1で昇順
			}
			return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
		});

		//std::cout << "UAV: "<< id << "CellnumArray: "<< CellnumArray.size() << std::endl;

		for(unsigned int i = 0; i < adjs.size();i++){
			if(cKnow.exsistuav(CellnumArray[i].second) == false){
				curCell = CellnumArray[i].second;
				return;
			}else{
				//std::cout << "UAV EXSIST!!" <<std::endl;
			}
		}
	}else{
		if(coverageMap[TcCellnum] < backline){
			int direction = cKnow.getDirection(curCell,TcCellnum);
			std::cout << "重点探索エリア"<<TcCellnum<<"は閾値を下回っています" << std::endl; 
			//std::cout << "現在地は" << curCell << "です" << std::endl;
			//std::cout << "位置は" << direction << "です"<< std::endl;
			
			std::vector<std::pair<double,int>> movepriorities;
			int Cellnum;
			if(direction == 0){
				//右
				Cellnum = cKnow.adjCellOf(curCell,"r");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 1){
				//右上
				Cellnum = cKnow.adjCellOf(curCell,"ur");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 2){
				//上
				Cellnum = cKnow.adjCellOf(curCell,"ur");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				Cellnum = cKnow.adjCellOf(curCell,"ul");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 3){
				//左上
				Cellnum = cKnow.adjCellOf(curCell,"ul");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 4){
				//左
				Cellnum = cKnow.adjCellOf(curCell,"l");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 5){
				//左下
				Cellnum = cKnow.adjCellOf(curCell,"ll");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 6){
				//下
				Cellnum = cKnow.adjCellOf(curCell,"lr");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				Cellnum = cKnow.adjCellOf(curCell,"ll");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 7){
				//右下
				Cellnum = cKnow.adjCellOf(curCell,"lr");
				if(cKnow.exsistuav(Cellnum) == false){
					curCell = Cellnum;
				}
			}else if(direction == 8){
				//左上左
				Cellnum = cKnow.adjCellOf(curCell,"l");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				Cellnum = cKnow.adjCellOf(curCell,"ul");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						//std::cout << "curcell case 8: " << Cellnum << std::endl;
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 9){
				//約上
				Cellnum = cKnow.adjCellOf(curCell,"ur");
				//std::cout << "ur: " << Cellnum << std::endl;
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				Cellnum = cKnow.adjCellOf(curCell,"ul");
				//std::cout << "ul: " << Cellnum << std::endl;
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 10){
				//右上右
				Cellnum = cKnow.adjCellOf(curCell,"ur");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				Cellnum = cKnow.adjCellOf(curCell,"r");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 11){
				//左下左
				Cellnum = cKnow.adjCellOf(curCell,"ll");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				Cellnum = cKnow.adjCellOf(curCell,"l");
				movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						//std::cout << "curcell case 11: " << Cellnum << std::endl;
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 12){
				//約下
				Cellnum = cKnow.adjCellOf(curCell,"lr");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				Cellnum = cKnow.adjCellOf(curCell,"ll");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}else if(direction == 13){
				//右下右
				Cellnum = cKnow.adjCellOf(curCell,"lr");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				Cellnum = cKnow.adjCellOf(curCell,"r");
				if(Cellnum >= 0){
					movepriorities.emplace_back(coverageMap[Cellnum],Cellnum);
				}
				std::sort(movepriorities.begin(), movepriorities.end(), [this](const auto& a, const auto& b) {
					if (a.first != b.first) {
						return a.first < b.first; // 値1で昇順
					}
					return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
				});
				for(unsigned int i = 0; i < movepriorities.size();i++){
					if(cKnow.exsistuav(movepriorities[i].second) == false){
						curCell = movepriorities[i].second;
						return;
					}else{
						//std::cout << "UAV EXSIST!!" <<std::endl;
					}
				}
			}
		}else{
			std::vector<int> adjs = cKnow.getAdjCells(curCell);//現在地の隣接セル
			std::vector<double> adjsCov;//隣接セルのカバレッジの値
			std::vector<int> moveca;//移動候補
			moveca.push_back(curCell);
			std::vector<int> movepriority;

			for(unsigned int i = 0; i < adjs.size(); i++){
				//
				adjsCov.push_back(coverageMap[adjs[i]]);
			}

			std::vector<std::pair<double,int>> CellnumArray;
			for (unsigned int i = 0; i < adjsCov.size(); i++ ){
				CellnumArray.emplace_back(adjsCov[i],adjs[i]);
			}

			std::sort(CellnumArray.begin(), CellnumArray.end(), [this](const auto& a, const auto& b) {
				if (a.first != b.first) {
					return a.first < b.first; // 値1で昇順
				}
				return minadjscov(a.second) < minadjscov(b.second); // 値2で昇順
			});

			//std::cout << "UAV: "<< id << "CellnumArray: "<< CellnumArray.size() << std::endl;

			for(unsigned int i = 0; i < adjs.size();i++){
				if(cKnow.exsistuav(CellnumArray[i].second) == false){
					curCell = CellnumArray[i].second;
					return;
				}else{
					//std::cout << "UAV EXSIST!!" <<std::endl;
				}
			}
		}
	}
}