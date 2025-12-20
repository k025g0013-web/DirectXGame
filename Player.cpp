#include "Player.h"
#include <cassert>

using namespace KamataEngine;

Player::Player() {};

// デストラクタ
Player::~Player() { delete model_; };

void Player::Initialize(Model* model, uint32_t textureHandle, Camera* camera) {
	// nullポインタチェック
	assert(model);

	// 引数の内容をメンバ変数に記録する
	model_ = model;
	textureHandle_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();

	// 引数の内容をメンバ変数に記録
	camera_ = camera;
};

void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
};

void Player::Draw() {
	// 3Dモデルを描画
	model_->Draw(worldTransform_, *camera_, textureHandle_);
};