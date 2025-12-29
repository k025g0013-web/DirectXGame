#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>
#include "UpdateWorldTransform.h"

enum class LRDirection {
	kRight,
	kLift,
};

class Player {
public:
	Player();  // コンストラクタ
	~Player(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();																									  // 更新
	void Draw();																									  // 描画

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 慣性
	static inline const float kAcceleration = 0.05f;

	// 速度減衰
	static inline const float kAttenuation = 0.2f;

	// 最大速度制限
	static inline const float kLimitRunSpeed = 2.0f;

	// 左右
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度 (下方向)
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度 (下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速 (上方向)
	static inline const float kJumpAcceleration = 1.0f;
};