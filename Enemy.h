#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>
#include "UpdateWorldTransform.h"
#include "MapChipField.h"
#include "AABB.h"

class Player;

class Enemy {
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

	// AABBを取得
	AABB GetAABB();

	// 衝突応答
	void OnCollision(const Player* player);

private:
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
};
