#include "Enemy.h"
#include "Player.h"
#include "Scene/GameScene.h"
#include <numbers>
#include <cmath>

using namespace KamataEngine;

Enemy::Enemy() {};
Enemy::~Enemy() {};

void Enemy::Initialize(Model* model, Camera* camera, const Vector3& position) {
	// nullポインタチェック
	assert(model);
	model_ = model;		// 引数の内容をメンバ変数に記録する
	camera_ = camera;	// 引数の内容をメンバ変数に記録

	// ワールド変換の初期化
	worldTransform_.Initialize();
	worldTransform_.translation_ = position;

	// 初期回転
	worldTransform_.rotation_.y = 3.0f * std::numbers::pi_v<float> / 2.0f;

	// 初期状態を設定
	behavior_ = Behavior::kWalk;
	behaviorRequest_ = Behavior::kUnknown;

	// 歩行状態初期化
	BehaviorWalkInitialize();

};

void Enemy::Update() {
	if (behaviorRequest_ != Behavior::kUnknown) {
		// 振る舞いを変更
		behavior_ = behaviorRequest_;
		// 各振る舞いごとの初期化を実行
		switch (behavior_) {
		case Behavior::kWalk:
			BehaviorWalkInitialize();
			break;
		case Behavior::kDeath:
			BehaviorDeathInitialize();
			break;
		}
		// 振る舞いリクエストをリセット
		behaviorRequest_ = Behavior::kUnknown;
	}

	// Behaviorの実行
	switch (behavior_) {
	case Behavior::kWalk:
		BehaviorWalkUpdate();
		break;
	case Behavior::kDeath:
		BehaviorDeathUpdate();
		break;
	}

	// 行列更新
	UpdateWorldTransform(worldTransform_);
};

void Enemy::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
};

#pragma region 各Behaviorの処理
void Enemy::BehaviorWalkInitialize() {
	// 経過時間
	walkTimer_ = 0.0f;

	// 速度を設定する
	velocity_ = {-kWalkSpeed, 0.0f, 0.0f};
}

void Enemy::BehaviorWalkUpdate() {
	// 移動
	worldTransform_.translation_.x += velocity_.x;

	// 経過時間
	walkTimer_ += 1.0f / 60.0f;

	// 回転アニメーション
	float param = std::sin(walkTimer_ / kWalkMotionTime * (2.0f * std::numbers::pi_v<float>));
	float degree = kWalkMotionAngleStart + kWalkMotionAngleEnd * (param + 1.0f) / 2.0f;
	worldTransform_.rotation_.x = degree * std::numbers::pi_v<float> / 180.0f;
}

void Enemy::BehaviorDeathInitialize() {
	deathTimer_ = 0;

	velocity_.x = 0.1f;
	velocity_.y = 0.3f;
}

void Enemy::BehaviorDeathUpdate() {
	deathTimer_++;

	// 簡易的な重力をかけて放物線を描いて落ちる
	velocity_.y -= 0.01f;

	// 座標に反映
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// くるくる回転しながら吹き飛ぶ演出
	worldTransform_.rotation_.z += 0.2f;
	worldTransform_.rotation_.x += 0.1f;

	// 一定時間経過したら完全に消滅フラグを立てる
	if (deathTimer_ >= kDeathTime) {
		isDead_ = true;
	}
}
#pragma endregion

Vector3 Enemy::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos;
	// ワールド行列の並行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];

	return worldPos;
};

AABB Enemy::GetAABB() {
	Vector3 worldPos = worldTransform_.translation_;

	AABB aabb;

	aabb.min = {worldPos.x - 1.9f / 2.0f, worldPos.y - 1.9f / 2.0f, worldPos.z - 1.9f / 2.0f};
	aabb.max = {worldPos.x + 1.9f / 2.0f, worldPos.y + 1.9f / 2.0f, worldPos.z + 1.9f / 2.0f};

	return aabb;
};

void Enemy::OnCollision(const Player *player) {
	if (GetIsCollisionDisabled()) {
		return;
	}

	if (player->IsAttack() || player->GetWorldTransform().scale_.z > 1.0f) {
		behaviorRequest_ = Behavior::kDeath;

		Vector3 effectPos{
		    (worldTransform_.translation_.x + player->GetWorldTransform().translation_.x) / 2.0f,
		    (worldTransform_.translation_.y + player->GetWorldTransform().translation_.y) / 2.0f,
		    (worldTransform_.translation_.z + player->GetWorldTransform().translation_.z) / 2.0f,
		};

		gameScene_->CreateHitEffect(effectPos);
		return;
	}
};