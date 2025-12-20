#pragma once
#include "KamataEngine.h"

class GameScene {
public:
	GameScene();  // コンストラクタ
	~GameScene(); // デストラクタ

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

private:
	// ImGui用float型変数
	float inputFloat3[3] = {0, 0, 0};

	// ハンドル
	uint32_t textureHandle_ = 0;   // テクスチャ
	uint32_t soundDetaHandle_ = 0; // サウンドデータ
	uint32_t voiceHandle_ = 0;

	// インスタンス生成
	KamataEngine::Sprite* sprite_ = nullptr; // スプライト
	KamataEngine::Model* model_ = nullptr;   // モデル

	// ワールドトランスフォーム
	KamataEngine::WorldTransform worldTransform_;
	// カメラ
	KamataEngine::Camera camera_;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};