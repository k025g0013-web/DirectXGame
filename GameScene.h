#pragma once
#include "KamataEngine.h"
#include <vector>

class GameScene {
public:
	GameScene();  // コンストラクタ
	~GameScene(); // デストラクタ

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

private:
	// カメラ
	KamataEngine::Camera camera_;

	// 3Dモデルデータ
	KamataEngine::Model* modelBlock_ = nullptr;

	// 可変個配列
	std::vector<std::vector<KamataEngine::WorldTransform*>> worldTransformBlocks_;

	// デバッグカメラ有効
	bool isDebugCameraActive_ = false;

	// デバッグカメラ
	KamataEngine::DebugCamera* debugCamera_ = nullptr;
};