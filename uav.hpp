#pragma once
//
// UAV Class
//
#include <deque>

class UAV
{
	unsigned int id;	// UAV ID
	unsigned int curCell;	// CurrentCell

	// uav-move: 移動関係 //////////////////////////////////
	std::deque<int> moveLog;	// 過去に移動したセルのログ

	/** 移動モード
	 * STOP: 停止
	 * RANDOMWALK：ランダムウォーク
	 * ROUNDTRIP: 往復移動
	 */
	enum class MMode { STOP, RANDOMWALK, ROUNDTRIP ,Nomalmode ,NEXTLOW };
	MMode mMode;

	/** 移動方向
	* 停止中は STOP
	* 右方向から反時計まわりに R, UR, UL, L, LL, LR
	*/
	enum class MDirection : int { STOP = 0, R = 1, UR = 2, UL = 3, L = -1, LL = -2, LR = -3 };
	MDirection mDirection;

	// uav-cellinfo: セル情報関係 ///////////////////////////
	std::vector<double> coverageMap;	// coverageMap
	std::vector<int> UavAssignment;		// 担当マップ：各セルを担当しているUAVの情報

public:
	// constructors & destructor
	UAV() = default;
	~UAV() = default;

	// getters
	unsigned int getID() { return id; }
	unsigned int getCurCell() { return curCell; }
	std::vector<double> getCoverageMap() { return coverageMap; }

	// setters

	// uav.cpp ////////////////////////////////////////////
	void locateAtInitialCell(int);	// 初期配置
	void init(unsigned int);	// もろもろ初期化

	void showAllInfo();	// (for debugging)全情報を出力

	// uav-move.cpp ///////////////////////////////////////
	void init_move();	// 初期化
	void move();	// 1セル移動
	void moveRandomWalk();	// ランダムウォーク
	void moveRoundTrip();	// 往復移動
	void pushMoveLog(unsigned int);	// 移動ログにセルを追加
	void moveNextlowcell();
	void movenomal();
	void moveLowcoverage(); //平均カバレッジが低い方向へ移動

	// uav-cellinfo.cpp ///////////////////////////////////
	void init_cellInfo();	// 初期化
	void arrive(unsigned int);	// 訪問（カバレッジを1.0に）
	void attenuateCovMap();	// カバレッジマップ上でカバレッジ減衰
	double covAve(int start, int end);
	void shareCovmap(UAV &uav1,UAV &uav2);//カバレッジ共有
	double minadjscov(int cur,int num);
};

