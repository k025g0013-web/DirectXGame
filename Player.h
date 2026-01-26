#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>
#include "UpdateWorldTransform.h"
#include "MapChipField.h"

enum class LRDirection {
	kRight,
	kLift,
};

// マップとの当たり判定情報
struct CollisionMapInfo {
	bool ceiling = false;
	bool landing = false;
	bool wall = false;
	KamataEngine::Vector3 move;
};

// 角
enum Corner {
	kRightBottom,	// 右下
	kLeftBottom,	// 左下
	kRightTop,		// 右上
	kLeftTop,		// 左上

	kNumCorner		// 要素数
};

class MapChipField;

class Player {
public:
	Player();  // コンストラクタ
	~Player(); // デストラクタ

	void Initialize(KamataEngine::Model* model, KamataEngine::Camera* camera, const KamataEngine::Vector3& position); // 初期化
	void Update();																									  // 更新
	void Draw();																									  // 描画

	void MoveInput();																								  // 移動入力
	void MoveOnResult(const CollisionMapInfo& info);																  // 判定結果を反映して移動させる 
	void IsHitCeilingChecker(const CollisionMapInfo& info);															  // 天井に接触している場合の処理
	void IsHitWallChecker(const CollisionMapInfo& info);															  // 壁に接触している場合の処理

	// ワールド変換データを取得
	const KamataEngine::WorldTransform& GetWorldTransform() const { return worldTransform_; }

	// 速度を取得
	const KamataEngine::Vector3& GetVelocity() const { return velocity_; }

	// マップチップの配置データを取得
	void SetMapChipField(MapChipField* mapChipField) { mapChipField_ = mapChipField; }

	// マップ衝突判定
	void CollisionMap(CollisionMapInfo& info);

	void CollisionMapTop(CollisionMapInfo& info);
	void CollisionMapBottom(CollisionMapInfo& info);
	void CollisionMapRight(CollisionMapInfo& info);
	void CollisionMapLeft(CollisionMapInfo& info);

	// 設置状態の切り替え処理
	void InstallationStateSwitching(CollisionMapInfo& info);

	// 指定した角の座標計算
	KamataEngine::Vector3 CornerPosition(const KamataEngine::Vector3& center, Corner corner);

private:
	// ワールド変換データ
	KamataEngine::WorldTransform worldTransform_;

	// モデル
	KamataEngine::Model* model_ = nullptr;

	// カメラ
	KamataEngine::Camera* camera_ = nullptr;

	// 速度
	KamataEngine::Vector3 velocity_ = {};

	// 慣性
	static inline const float kAcceleration = 0.05f;

	// 速度減衰
	static inline const float kAttenuation = 0.2f;

	// 最大速度制限
	static inline const float kLimitRunSpeed = 2.0f;

	// 左右
	LRDirection lrDirection_ = LRDirection::kRight;

	// 旋回開始時の角度
	float turnFirstRotationY_ = 0.0f;
	// 旋回タイマー
	float turnTimer_ = 0.0f;

	// 旋回時間<秒>
	static inline const float kTimeTurn = 0.3f;

	// 接地状態フラグ
	bool onGround_ = true;

	// 重力加速度 (下方向)
	static inline const float kGravityAcceleration = 0.05f;
	// 最大落下速度 (下方向)
	static inline const float kLimitFallSpeed = 1.0f;
	// ジャンプ初速 (上方向)
	static inline const float kJumpAcceleration = 1.0f;

	// マップチップによるフィールド
	MapChipField* mapChipField_ = nullptr;

	// キャラクターの当たり判定のサイズ
	static inline const float kWidth = 1.9f;
	static inline const float kHeight = 1.9f;

	// 微小な余白
	static inline const float kBlank = 0.0001f;

	// 着地時の速度減衰率
	static inline const float kAttenuationLanding = 0.1f;

	// 着地時の速度減衰率
	static inline const float kAttenuationWall = 0.75f;
};