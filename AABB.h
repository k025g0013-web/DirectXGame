#pragma once
#include "KamataEngine.h"

// AABB
struct AABB {
	KamataEngine::Vector3 min = {};
	KamataEngine::Vector3 max = {};
};