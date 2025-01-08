/**
 * UAV制御系ネタシミュレータ
 *
 * @file uavsim-main.cpp
 * @brief シミュレータメイン部
 * @date 2017/04
*/

#include "simulator.hpp"
#include "uav.hpp"
#include "cell.hpp"

#include <iostream>

//
// グローバル変数群の定義
//
Parameters params(DEFAULT_PARAM_FILE); // シミュレーション用パラメータ（デフォルト設定ファイルベースで初期化）
CommonKnowledge cKnow; // 共有知識
God god; // 神様目線さん
RandGenerator rGenerator;	// 乱数生成器
FileWriter fWriter;	// ファイル操作器

////////////////////////////////////////////////////////////////////////////////////////

// 第roundラウンドのシミュレーション実験
void runSim(unsigned int round) {
#ifdef REPORT
	std::cout << std::endl << "[Round " << round << "] ";
#endif

	// 各種書込み用ファイルオープン
	std::string coverageLogFileName = params.getCoverageLogFilePath() + std::to_string(round) + ".log";
	std::string uavLocLogFileName = params.getUavLocLogFilePath() + std::to_string(round) + ".log";
	std::string coverageAveFilename = params.getCoverageAveFilePath() +std::to_string(round) + ".log";
	std::string coverageVariFilename = params.getCoverageVariFilePath() + std::to_string(round) + ".log";
	std::string impcoveragefilename = params.getImpCoveragefilePath() + std::to_string(round) + ".log";
	std::string impAreanumsFilename = params.getimpAreaNumsFilePath() + std::to_string(round) + ".log";
	fWriter.openFile(coverageLogFileName);
	fWriter.openFile(uavLocLogFileName);
	fWriter.openFile(coverageAveFilename);
	fWriter.openFile(coverageVariFilename);
	fWriter.openFile(impcoveragefilename);
	fWriter.openFile(impAreanumsFilename);

	// セル数を初期化
	std::vector<Cell> cells(params.getCellNum());
	for (unsigned int i = 0; i < cells.size(); i++) {
		cells[i].setID(i);
	}
	//重点探索エリアを設定
	std::vector<int> ids;
	for(int i = 0;i < params.getCellNum();i++){
		ids.push_back(i);
	}
	std::vector<int> rids = rGenerator.randomVector(ids);
	for(int i = 0;i < params.getimpAreaNum();i++){
		std::cout << "\n重点探索エリアを設定します";
		for(unsigned int j = 0; j < cells.size();j++){
			if(static_cast<unsigned int>(rids[i]) == j){
				cells[j].setArea();
			}
		}
	}

	//重点エリアをlogに書き込む
	std::string log  = "";
	for(auto &c : cells){
		if(c.getArea() == 1){
			log += "\t"; 
			log += std::to_string(c.getId());
		}
	}
	fWriter.writeLine(impAreanumsFilename , log);
	// UAVを初期位置に配置
	std::vector<UAV> uavs(params.getUavNum());
	for (unsigned int i = 0; i < uavs.size(); i++) {
		uavs[i].init(i);	// もろもろ初期化処理
		do {
			uavs[i].locateAtInitialCell(params.getCellNum());
		} while (cells[uavs[i].getCurCell()].getCoverage());
		uavs[i].arrive(uavs[i].getCurCell());

		// 設置したセルのカバレッジを1に（「UAV到着しました」関数で代用）
		cells[uavs[i].getCurCell()].UAVarrived(i);
	}
	cKnow.generateUavexsistCells(uavs);
	// 時間経過開始！
	// ToDo: この中もうちょい整理しろやワレ。
	for (unsigned int now = 0; now < params.getSimDuration(); now++) {
#ifdef DEBUG
		std::cout << "\n - time: " << now;
#elif defined(REPORT)
		if ((now+1) % 10 == 0) { std::cout << "+"; }
		else { std::cout << "-"; }
#endif


		// 全セルカバレッジ減衰
		for (auto &c : cells) {
			//std::cout << "\n減衰！";
			c.attenuateCoverage();
		}
		//カバレッジ共有
		for (unsigned int i = 0; i < uavs.size(); i++) {
			for (unsigned int j = i + 1; j < uavs.size(); j++) {
			//std::cout << "\n(" << elements[i] << ", " << elements[j] << ")\n";

			if(cKnow.isAdjacent(uavs[i].getCurCell(),uavs[j].getCurCell())){
				//
				//std::cout << "\n i:"<< uavs[i].getCurCell() << ", j:"<<uavs[j].getCurCell();
				uavs[i].shareCovmap(uavs[i],uavs[j]);
				uavs[j].shareCovmap(uavs[j],uavs[i]);
				//std::cout << "\n通信完了 : "<< uavs[i].getID() <<" , " << uavs[j].getID();
			}
			
			}
		}
		// UAV移動
		for (auto &u: uavs){
			u.move();
			cells[u.getCurCell()].UAVarrived();	// 実際のカバレッジも更新
			cKnow.setexsistuav(u);
			u.setAreainfo(cells[u.getCurCell()]);
		}
		


		//
		// 各種ログ書き出し
		//
		log = "";
		// UAV位置
		for (auto &u : uavs) {
			log += "\t" + to_string(u.getCurCell());
		}
		fWriter.writeLine(uavLocLogFileName, log);
		// セルカバレッジ
		log = "";
		for (auto &c : cells) {
			log += "\t" + to_string(c.getCoverage());
		}
		fWriter.writeLine(coverageLogFileName, log);
		//uavが持っているカバレッジマップ
		log = ""; 
		for(auto &c : uavs){
			log = "";
			std::vector<double> cov = c.getCoverageMap();
			std::string uavcoverageFileName = params.getUavcoverageFilePath() + std::to_string(round) + "uav"+std::to_string(c.getID()) + ".log";
			fWriter.openFile(uavcoverageFileName);
			log += "\t";
			for(unsigned int i = 0; i < cov.size();i++){
				log += to_string(cov[i]);
			}
			fWriter.writeLine(uavcoverageFileName, log);
		}
		//カバレッジ平均
		log = "";
		log += "\t";
		double coverageAve = god.generateCovaverage(cells);
		log += to_string(coverageAve);
		fWriter.writeLine(coverageAveFilename, log);
		// //重点探索エリアのカバレッジ平均
		// std::vector<Cell> impcells;
		// log = "";
		// log += "\t";
		// for(auto &c : cells){
		// 	if(c.getArea() == 1){
		// 		impcells.push_back(c);
		// 	}
		// }
		// coverageAve = god.generateCovaverage(impcells);
		// log += to_string(coverageAve);
		// fWriter.writeLine(impcoveragefilename, log);
		
		//カバレッジ分散
		log = "";
		log += "\t";
		double coverageVari = god.generateCovvariance(cells);
		log += to_string(coverageVari);
		fWriter.writeLine(coverageVariFilename, log);
		
		//UAVのカバレッジを出力
		// for (auto &u : uavs) {
		// 	u.showAllInfo();
		// }

	}// 時間経過ココまで

	// 各種書込み用ファイルクローズ
	for(unsigned int i = 0; i < uavs.size(); i++ ){
		std::string uavcoverageFilename = params.getUavcoverageFilePath() + std::to_string(round) + "uav"+std::to_string(i) + ".log";
		fWriter.closeFile(uavcoverageFilename);
	}
	fWriter.closeFile(coverageLogFileName);
	fWriter.closeFile(uavLocLogFileName);
	fWriter.closeFile(coverageAveFilename);
	fWriter.closeFile(coverageVariFilename);
	fWriter.closeFile(impcoveragefilename);
	fWriter.closeFile(impAreanumsFilename);
	//std::cout <<"\n covrageAve: "<<god.generateCovaverage(cells);
	//std::cout <<"\n covrageVari: "<<god.generateCovvariance(cells);
}


////////////////////////////////////////////////////////////////////////////////////////
//
// メイン処理ココから
//
int main(int argc, char* argv[]) {

	// コマンドライン入力から各種パラメータを設定
	params.parseCmdline(argc, argv);

	// 共有知識をセット
	cKnow.init(params);

	// 乱数を初期化
	rGenerator.initSeed();

	// シミュレーション開始！
	for (unsigned int round = 0; round < params.getSimIteration(); round++) {
		runSim(round);
	}

	return 0;
}
