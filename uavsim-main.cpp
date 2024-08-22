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
	fWriter.openFile(coverageLogFileName);
	fWriter.openFile(uavLocLogFileName);

	// セル数を初期化
	std::vector<Cell> cells(params.getCellNum());
	for (unsigned int i = 0; i < cells.size(); i++) {
		cells[i].setID(i);
	}

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
			c.attenuateCoverage();
		}

		// UAV移動
		for (auto &u: uavs){
			u.move();
			cells[u.getCurCell()].UAVarrived();	// 実際のカバレッジも更新
		}
		//カバレッジ共有
		for (size_t i = 0; i < uavs.size(); ++i) {
        for (size_t j = i + 1; j < uavs.size(); ++j) {
            //std::cout << "\n(" << elements[i] << ", " << elements[j] << ")\n";
			uavs[i].shareCovmap(uavs[i],uavs[j]);
			uavs[j].shareCovmap(uavs[j],uavs[i]);
        }
    }

		//
		// 各種ログ書き出し
		//
		std::string log = "";
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

		for (auto &u : uavs) {
			u.showAllInfo();
		}

	}// 時間経過ココまで

	// 各種書込み用ファイルクローズ
	fWriter.closeFile(coverageLogFileName);
	fWriter.closeFile(uavLocLogFileName);
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
