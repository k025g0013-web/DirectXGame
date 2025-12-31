#pragma once
#include "KamataEngine.h"
#include <vector>

#include "UpdateWorldTransform.h"
#include "MapChipField.h"
#include "CameraController.h"

#include "Player.h"
#include "Skydome.h"

class GameScene {
public:
	GameScene();  // コンストラクタ
	~GameScene(); // デストラクタ

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

	// 表示ブロックの生成
	void GenerateBlocks();

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

	// プレイヤー
	Player* player_ = nullptr;
	KamataEngine::Model* modelPlayer_ = nullptr;

	// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_;

	// カメラコントローラー
	CameraController* cameraController_;
};