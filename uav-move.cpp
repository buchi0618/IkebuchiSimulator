/**
 * @file uav-move.cpp
 * @brief UAV Class：UAVの移動関係の関数ひとまとめ
 * @date 2017/12/26
 */
#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"

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
		mMode = MMode::Nomalmode;
	case MMode::RANDOMWALK:
		moveRandomWalk();
		break;
	case MMode::ROUNDTRIP:
		moveRoundTrip();
		break;
	case MMode::NEXTLOW:
		moveNextlowcell();
		break;
	case MMode::Nomalmode:
		movenomal();
		break;
	default:
		std::cout << "Invalid Mobility Mode! (at UAV::move())" << std::endl;
		exit(1);
	}

	// 到着処理
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
 * @brief カバレッジ平均が低いほうへ移動
 */

void UAV::movenomal(){
	//列の数
	int numcols = params.getCellCols();
	int numrows = params.getCellRows();
	//セルの数
	unsigned int cellnums = numcols * numrows;
	//行番号
	//int row = curCell / numcols ;
	//列番号
	int col = curCell % numcols ;
	// 閾値
	double p  =params.getthreshold();

	//現在いるセルから左にあるセルの数
	int left = col ;
	//現在いるセルから右にあるセルの数
	int right = numcols - col -1 ;
	//現在いるセルから上にあるセルの数
	//int up = (row) * numcols;
	//現在いるセルから下にあるセルの数
	//int down = (numrows - row - 1) * numcols;
	//左側のカバレッジの平均
	double leftAve = covAve(curCell - left, curCell - 1);
	//右側のカバレッジの平均
	double rightAve = covAve(curCell + 1, curCell + right);
	//上側のカバレッジの平均
	double upAve = covAve( 0, curCell - left - 1);
	//下側のカバレッジの平均
	double downAve = covAve(curCell + right, numcols * numrows - 1);

	//debug 
	std::cout << "UAV ID: " << id << std::endl;
	std::cout << "col: " << col << std::endl;
	std::cout << "Cellnum: "<< cellnums << std::endl;
	std::cout << "Current Cell: " << curCell << std::endl;
	// std::cout << "Current Cell left : " << left << std::endl;
	// std::cout << "Current Cell right : " << right << std::endl;
	// std::cout << "Current Cell up: " << up << std::endl;
	// std::cout << "Current Cell down: " << down << std::endl;
	// std::cout << "curCell - left : " << curCell - left << std::endl;
	// std::cout << "curCell + right : " << curCell + right << std::endl;
	// std::cout << "curCell - left - 1 : " << curCell - left - 1 << std::endl;
	// std::cout << "numcols * numrows - 1 :" << numcols * numrows - 1  << std::endl;
    //std::cout << "Left Ave: " << leftAve << std::endl;
    //std::cout << "Right Ave: " << rightAve << std::endl;
	std::cout << "curcell + numcols: "<< curCell + numcols<< std::endl;
    std::cout << "Up Ave: " << upAve << std::endl;
    std::cout << "Down Ave: " << downAve << std::endl;


	//端で上下に移動する場合
	if(col == 0 || col == numcols -1)
	{
		//上下のカバレッジ平均の低いほうへ移動
		if( upAve < p || downAve < p){
		if(upAve < downAve && curCell - numcols > 0){
			curCell = curCell - numcols;
		}
		else if(curCell + numcols < cellnums)
		{
			curCell = curCell + numcols;
			//debug
			std::cout << "downのほうが低い"<< std::endl;
		}
		//平均が同じ場合はランダムに移動方向を決める
		else if(upAve == downAve)
		{
			//
			int rnd = rGenerator.intBetween(0 , 1);
			if(rnd == 0){
				curCell = curCell + numcols;
			}
			else if(rnd == 1){
				curCell = curCell - numcols;
			}
			std::cout << "やあ"<< std::endl;
		}
		
		}
		return;
	}
	//左右に移動できる場合
	else{
		//左右のカバレッジ平均の低いほうへ移動
		if( leftAve < p || rightAve < p){
		if(leftAve < rightAve){
			curCell = curCell - 1;
		}else{
			curCell = curCell + 1;
		}
		}
		//debug 
		std::cout << "ああ"<< std::endl;
		return;
	}
	

}
/**
 * @brief カバレッジが低いほうに移動
 * ito 手法
 * 
 */

void UAV::moveLowcoverage(){
	//
}