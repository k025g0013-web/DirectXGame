#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>

#include "MapChipField.h"

#include "Utils/AABB.h"
#include "Utils/UpdateWorldTransform.h"

class GameScene;

class Player;

class Enemy {
public:
	enum class Behavior {
		kUnknown,
		kWalk,
		kDeath,
	};

public:
	Enemy();  // コンストラクタ
	~Enemy(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();                                                                                                    // 更新
	void Draw();                                                                                                      // 描画

	// マップチップの配置データを取得
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// ワールド座標を取得
	KamataEngine::Vector3 GetWorldPosition();

	void BehaviorWalkInitialize();
	void BehaviorWalkUpdate();
	void BehaviorDeathInitialize();
	void BehaviorDeathUpdate();

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player);

	const bool GetIsDead() const { return isDead_; }
	
	const bool GetIsCollisionDisabled() const { return (behavior_ == Behavior::kDeath || isDead_); }

	// ゲームシーンポインタを取得
	void SetGameScene(GameScene* gameScene) { gameScene_ = gameScene; }

private:
	// 振る舞い
	Behavior behavior_ = Behavior::kWalk;
	Behavior behaviorRequest_ = Behavior::kUnknown;

	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// 歩行の速さ
	static inline const float kWalkSpeed = 0.02f;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 最初の角度[度]
	static inline const float kWalkMotionAngleStart = -15.0f;
	// 最後の角度[度]
	static inline const float kWalkMotionAngleEnd = 30.0f;
	// アニメーションの周期となる時間[秒]
	static inline const float kWalkMotionTime = 1.0f;

	// 経過時間
	float walkTimer_ = 0.0f;

	// 死亡（Death）用の演出パラメータ
	uint32_t deathTimer_ = 0;
	static inline const uint32_t kDeathTime = 60; // 60フレームで消滅

	// デスフラグ
	bool isDead_ = false;

	// 衝突判定スキップ
	bool IsCollisionDisabled_ = false;

	GameScene *gameScene_ = nullptr;
};
