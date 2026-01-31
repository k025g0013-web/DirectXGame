#include "Enemy.h"
#include <numbers>
#include <cmath>

using namespace KamataEngine;

// コンストラクタ
Enemy::Enemy() {};

// デストラクタ
Enemy::~Enemy() { delete model_; };

// 初期化処理
void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// nullポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// 初期回転
	worldTransform_.rotation_.y = 3.0f * std::numbers::pi_v<float> / 2.0f;

	// 速度を設定する
	velocity_ = { -kWalkSpeed, 0.0f, 0.0f };

	// 経過時間
	walkTimer_ = 0.0f;
};

// 更新処理
void Enemy::Update() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;

	// 経過時間
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(walkTimer_ / kWalkMotionTime * (2.0f * std::numbers::pi_v<float>));
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * std::numbers::pi_v<float> / 180.0f;

	// 行列更新
	UpdateWorldTransform(worldTransform_);
};

// 描画処理
void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
};