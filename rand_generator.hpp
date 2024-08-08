#pragma once
/**
 * @file rand_generator.hpp
 * @brief 乱数生成器
 * @date 2017/04/21
 */

#include <random>
#include <time.h>

class RandGenerator {
	std::mt19937_64 engine;

public:
	RandGenerator() = default;
	~RandGenerator() = default;

	// シードを初期化：random_deviceはMinGWで固定値を吐き出すのでTimeで対応
	void initSeed() {
		engine.seed(time(NULL));
	}

	// 整数値 [min, max]
	int intBetween(int min, int max) {
		std::uniform_int_distribution<int> range(min, max);
		return range(engine);
	}

};
