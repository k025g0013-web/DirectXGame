#pragma once
#include "KamataEngine.h"

class UpdateWorldTransform {};

KamataEngine::Matrix4x4 MakeAffineMatrix(
	KamataEngine::Vector3& scale, KamataEngine::Vector3& rotation, KamataEngine::Vector3& translation
);

void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform);