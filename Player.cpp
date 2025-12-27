#include "Player.h"
#include <cassert>

using namespace KamataEngine;

// コンストラクタ
Player::Player() {};

// デストラクタ
Player::~Player() {
	delete model_; 
};

void Player::Initialize(Model* model, Camera* camera) {
	// nullポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録する
	model_ = model;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;

	// 初期位置設定
	worldTransform_.translation_ = {2.0f, 2.0f, 0.0f};
};

void Player::Update() {
	// 行列を定数バッファに転送
	UpdateWorldTransform(worldTransform_);
};

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_);
};