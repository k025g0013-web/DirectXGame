#include "CameraController.h"
#include <algorithm>

using namespace KamataEngine;

// デストラクタ
CameraController::~CameraController() {};

void CameraController::Initialize() {
	// カメラの初期化
	camera_.Initialize();
}

void CameraController::Update() {
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform();
	// 追従対象の速度を参照
	const Vector3& targetVelocity = target_->GetVelocity();
	// 追従対象とオフセットと追尾対象の速度からカメラの目標座標を計算
	targetPosition_.x = targetWorldTransform.translation_.x + targetOffset_.x + targetVelocity.x * kVelocityBias;
	targetPosition_.y = targetWorldTransform.translation_.y + targetOffset_.y + targetVelocity.y * kVelocityBias;
	targetPosition_.z = targetWorldTransform.translation_.z + targetOffset_.z + targetVelocity.z * kVelocityBias;

	// 座標補間によりゆったり追従
	camera_.translation_ = Lerp(camera_.translation_, targetPosition_, kInterpolationRate);

	// 追従対象が画面外に出ないように補正
	camera_.translation_.x = std::clamp(camera_.translation_.x, targetWorldTransform.translation_.x + margin_.left,  targetWorldTransform.translation_.x + margin_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, targetWorldTransform.translation_.y + margin_.bottom,targetWorldTransform.translation_.y + margin_.top);

	// 移動範囲制限
	camera_.translation_.x = std::clamp(camera_.translation_.x, movableArea_.left, movableArea_.right);
	camera_.translation_.y = std::clamp(camera_.translation_.y, movableArea_.bottom, movableArea_.top);

	// 行列を更新する
	camera_.UpdateMatrix();
}

void CameraController::Reset() { 
	// 追従対象のワールドトランスフォームを参照
	const WorldTransform& targetWorldTransform = target_->GetWorldTransform(); 
	// 追従対象とオフセットからカメラの座標を計算
	camera_.translation_.x = targetWorldTransform.translation_.x + targetOffset_.x;
	camera_.translation_.y = targetWorldTransform.translation_.y + targetOffset_.y;
	camera_.translation_.z = targetWorldTransform.translation_.z + targetOffset_.z;
}

Vector3 CameraController::Lerp(const Vector3& start, const Vector3& end, float t) {
	return Vector3{
		start.x + (end.x - start.x) * t,
		start.y + (end.y - start.y) * t, 
		start.z + (end.z - start.z) * t
	};
}