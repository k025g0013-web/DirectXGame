#pragma once
#include "KamataEngine.h"

class Player {
public:
	Player();  // コンストラクタ
	~Player(); // デストラクタ

	void Initialize(KamataEngine::Model* model, uint32_t textureHandle, KamataEngine::Camera* camera); // 初期化
	void Update();                                                                                     // 更新
	void Draw();                                                                                       // 描画

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// テクスチャハンドル
	uint32_t textureHandle_ = 0u;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;
};