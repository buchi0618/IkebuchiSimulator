#pragma once
/**
 * @file simulator.hpp
 * @brief 全ファイル共通で利用するヘッダ
 * @date 2017/04/21
 */

#include "parameters.hpp"
#include "common_knowledge.hpp"
#include "god.hpp"
#include "cell.hpp"
#include "rand_generator.hpp"
#include "file_writer.hpp"

// デバッグ用出力を行うかどうか
#define DEBUG

// レポート用出力を行うかどうか
#define REPORT

#if defined(DEBUG) || defined(REPORT)
#include <iostream>
#endif

// デフォルト設定ファイル
#define DEFAULT_PARAM_FILE "params/default.ini"

// 各種グローバル変数の宣言
extern Parameters params;
extern CommonKnowledge cKnow; // 共有知識
extern God god; // 神様目線さん
extern RandGenerator rGenerator;	// 乱数生成器
extern FileWriter fWriter;	// ファイル操作器