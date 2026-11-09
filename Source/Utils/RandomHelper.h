#pragma once
#include <random>

namespace Tuna {
	// 乱数生成器
	inline std::mt19937 randomEngine{ std::random_device{}() };

	/// <summary>
	/// 指定した範囲の乱数(float)を生成する
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	inline float RandHelperFloat(float min, float max) {
		std::uniform_real_distribution<float> distribution(min, max);
		return distribution(randomEngine);
	}

	/// <summary>
	/// 指定した範囲の整数乱数を生成する
	/// </summary>
	/// <param name="min">最小値</param>
	/// <param name="max">最大値</param>
	inline int RandHelperInt(int min, int max) {
		if (max <= min) {
			return min;
		}

		std::uniform_int_distribution<int> distribution(min, max);
		return distribution(randomEngine);
	}

}