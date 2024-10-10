#pragma once
/**
 * @file parameters.hpp
 * @brief 各種パラメータ格納用クラス
 * @date 2017/04
*/

#include <string>

class Parameters
{
	// [sim] シミュレーション設定関係
	unsigned int simIteration;	// シミュレーション試行回数
	unsigned int simDuration;	// シミュレーション試行あたりの時間（ターン数）

	// [env] シミュレーション関係関係
	int uavNum;	// UAV数
	unsigned int uavMoveLogNum;	// UAVの移動ログ記録セル数
	int cellCols, cellRows;	// セル数（x方向、y方向）
	double coverageAttenuation; // カバレッジの減衰量／ターン
	double threshold; //閾値

	// [file] 入出力ファイル関係
	std::string coverageLogFilePath;
	std::string uavLocLogFilePath;
	std::string uavcoverageFilePath;

	// 設定ファイルを読み込んでパラメータを設定
	void setFromIniFile(std::string);

public:
	// constructors & destructors
	Parameters() = default;
	~Parameters() = default;
	Parameters(std::string);
	
	// getters
	unsigned int getSimIteration() { return simIteration; }
	unsigned int getSimDuration() { return simDuration; }
	int getUavNum() { return uavNum; }
	unsigned int getUavMoveLogNum() { return uavMoveLogNum; }
	int getCellCols() { return cellCols; }
	int getCellRows() { return cellRows; }
	double getCovAttenuation() { return coverageAttenuation; }
	std::string getCoverageLogFilePath() { return coverageLogFilePath; }
	std::string getUavLocLogFilePath() { return uavLocLogFilePath; }
	std::string getUavcoverageFilePath(){ return uavcoverageFilePath;}

	int getCellNum() { return cellCols * cellRows; }
	double getthreshold(){ return threshold; }

	// setters

	// コマンドライン引数をパース
	void parseCmdline(int, char*[]);

};