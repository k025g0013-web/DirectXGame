#define NOMINMAX
#include "Player.h"
#include <algorithm>
#include <cassert>
#include <numbers>

using namespace KamataEngine;

// コンストラクタ
Player::Player() {};

// デストラクタ
Player::~Player() { delete model_; };

void Player::Initialize(Model* model, Camera* camera, const Vector3& position) {
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
	worldTransform_.rotation_.y = std::numbers::pi_v<float> / 2.0f;
};

void Player::Update() {
	// 着地フラグ
	bool landing = false;

	// 地面との当たり判定
	// 下降中？
	if (velocity_.y < 0.0f) {
		// y座標が地面以下になったら着地
		if (worldTransform_.translation_.y <= 2.0f) {
			landing = true;
		}
	}
	
	// 移動入力
	// 接地状態
	if (onGround_) {
		// 左右移動操作
		if (Input::GetInstance()->PushKey(DIK_RIGHT) ||
			Input::GetInstance()->PushKey(DIK_LEFT)) {

			// 左右加速
			Vector3 acceleration = {};
			if (Input::GetInstance()->PushKey(DIK_RIGHT)) {
				// 左移動中の右入力
				if (velocity_.x < 0.0f) {
					// 速度と逆方向に入力時は急ブレーキ
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x += kAcceleration;

				if (lrDirection_ != LRDirection::kRight) {
					lrDirection_ = LRDirection::kRight;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = std::numbers::pi_v<float> * 3.0f / 2.0f;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			} else if (Input::GetInstance()->PushKey(DIK_LEFT)) {
				// 左移動中の右入力
				if (velocity_.x > 0.0f) {
					// 速度と逆方向に入力時は急ブレーキ
					velocity_.x *= (1.0f - kAcceleration);
				}
				acceleration.x -= kAcceleration;

				if (lrDirection_ != LRDirection::kLift) {
					lrDirection_ = LRDirection::kLift;

					// 旋回開始時の角度を記録する
					turnFirstRotationY_ = std::numbers::pi_v<float> / 2.0f;
					// 旋回タイマーに時間を設定する
					turnTimer_ = kTimeTurn;
				}
			}
			// 加速 / 減速
			velocity_.x += acceleration.x;

			// 最大速度制限
			velocity_.x = std::clamp(velocity_.x, -kLimitRunSpeed, kLimitRunSpeed);
		} else {
			// 非入力時は速度減衰をかける
			velocity_.x *= (1.0f - kAttenuation);
		}
		if (Input::GetInstance()->PushKey(DIK_UP)) {
			// ジャンプ初速
			velocity_.y += kJumpAcceleration;
		}

		// ジャンプ開始
		if (velocity_.y > 0.0f) {
			// 空中状態に移行
			onGround_ = false;
		}
	// 空中
	} else {
		// 落下状態
		velocity_.y += -kGravityAcceleration;
		// 落下速度制限
		velocity_.y = std::max(velocity_.y, -kLimitFallSpeed);

		// 着地
		if (landing) {
			// めり込み排斥
			worldTransform_.translation_.y = 2.0f;
			// 摩擦で横方向が減速する
			velocity_.x *= (1.0f - kAttenuation);
			// 下方向速度をリセット
			velocity_.y = 0.0f;
			// 設置状態に移行
			onGround_ = true;
		}
	}

	// 旋回制御
	if (turnTimer_ > 0.0f) {
		// 旋回タイマーを1/60秒だけカウントダウンする
		turnTimer_ -= 1.0f / 60.0f;

		// 左右の自キャラ角度テーブル
		float destinationRotationYTable[] = {
			std::numbers::pi_v<float> / 2.0f,
			std::numbers::pi_v<float> * 3.0f / 2.0f
		};
		// 状態に応じた角度を取得する
		float destinationRotation = destinationRotationYTable[static_cast<uint32_t>(lrDirection_)];

		// 自キャラの角度を設定する
		float t = 1.0f - std::clamp(turnTimer_ / kTimeTurn, 0.0f, 1.0f);
		worldTransform_.rotation_.y = turnFirstRotationY_ + (destinationRotation - turnFirstRotationY_) * t;
	}

	// 移動
	worldTransform_.translation_.x += velocity_.x;
	worldTransform_.translation_.y += velocity_.y;

	// 行列更新
	UpdateWorldTransform(worldTransform_);
};

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
};