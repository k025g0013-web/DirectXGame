#pragma once
#include "KamataEngine.h"
#include <vector>

#include "UpdateWorldTransform.h"
#include "MapChipField.h"
#include "CameraController.h"

#include "Player.h"
#include "Enemy.h"
#include "Skydome.h"
#include "DeathParticles.h"
#include "Fade.h"

class GameScene {
public:
	GameScene();  // コンストラクタ
	~GameScene(); // デストラクタ

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

	// 表示ブロックの生成
	void GenerateBlocks();

	// 全ての当たり判定を行う
	void CheckAllCollision();

	// ゲームのフェーズ(型)
	enum class Phase {
		kFadeIn,	// フェードイン
		kPlay,		// ゲームプレイ
		kDeath,		// デス演出
		kFadeOut,	// フェードアウト
	};

	// ゲームの現在フェーズ(変数)
	Phase phase_ = Phase::kFadeIn;

	// フェーズの切り替え
	void ChangePhase();

	// 終了フラグのgetter
	bool IsFinished() const { return finished_; };

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
	
	Player* playerAttack_ = nullptr;
	KamataEngine::Model* modelPlayerAttack_ = nullptr;

	// エネミー
	std::list<Enemy*> enemies_;
	KamataEngine::Model* modelEnemy_ = nullptr;

	// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

	// マップチップフィールド
	MapChipField* mapChipField_ = nullptr;

	// カメラコントローラー
	CameraController* cameraController_;

	// デス演出
	DeathParticles* deathParticles_ = nullptr;
	KamataEngine::Model* modelDeathParticles_ = nullptr;

	// フェード
	Fade* fade_ = nullptr;
	float duration_ = 1.0f;

	// 終了フラグ
	bool finished_ = false;
};