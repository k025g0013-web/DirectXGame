#include "UpdateWorldTransform.h"

using namespace KamataEngine;

Matrix4x4 MakeAffineMatrix(Vector3& scale, Vector3& rotation, Vector3& translation) {
	// アフィン変換行列の作成
	Matrix4x4 affineMatrix = {};

	for (int i = 0; i < 4; i++) {
		affineMatrix.m[i][i] = 1.0f;
	}

	// アフィン変換行列
	affineMatrix.m[0][0] = scale.x * (cos(rotation.y) * cos(rotation.z));
	affineMatrix.m[0][1] = scale.x * (cos(rotation.y) * sin(rotation.z));
	affineMatrix.m[0][2] = scale.x * (-sin(rotation.y));

	affineMatrix.m[1][0] = scale.y * (sin(rotation.x) * sin(rotation.y) * cos(rotation.z) - cos(rotation.x) * sin(rotation.z));
	affineMatrix.m[1][1] = scale.y * (sin(rotation.x) * sin(rotation.y) * sin(rotation.z) + cos(rotation.x) * cos(rotation.z));
	affineMatrix.m[1][2] = scale.y * (sin(rotation.x) * cos(rotation.y));

	affineMatrix.m[2][0] = scale.z * (cos(rotation.x) * sin(rotation.y) * cos(rotation.z) + sin(rotation.x) * sin(rotation.z));
	affineMatrix.m[2][1] = scale.z * (cos(rotation.x) * sin(rotation.y) * sin(rotation.z) - sin(rotation.x) * cos(rotation.z));
	affineMatrix.m[2][2] = scale.z * (cos(rotation.x) * cos(rotation.y));

	affineMatrix.m[3][0] = translation.x;
	affineMatrix.m[3][1] = translation.y;
	affineMatrix.m[3][2] = translation.z;

	return affineMatrix;
};

void UpdateWorldTransform(KamataEngine::WorldTransform& worldTransform) {
	// スケール、回転、並行移動を合成して行列を計算する
	worldTransform.matWorld_ = MakeAffineMatrix(
		worldTransform.scale_, worldTransform.rotation_, worldTransform.translation_
	);

	// 定数バッファに転送する
	worldTransform.TransferMatrix();
};