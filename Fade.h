#pragma once
#include "KamataEngine.h"
#include <3d\Model.h>

class Fade {
public:
	enum class Status {
		None,    // フェードなし
		FadeIn,  // フェードイン中
		FadeOut, // フェードアウト中
	};

	Fade();  // コンストラクタ
	~Fade(); // デストラクタ

	void Initialize();  // 初期化
	void Update();		// 更新
	void Draw();		// 描画

	void Start(Status status, float duration) {	// 開始
		status_ = status;
		duration_ = duration;
		counter_ = 0.0f;
	}
	void Stop() { status_ = Status::None; }		// 終了

	// フェード終了判定
	bool IsFinished() const;

private:
	uint32_t textureHandle_ = 0;

	KamataEngine::Sprite* sprite_ = nullptr;

	// 現在のフェードの状態
	Status status_ = Status::None;

	// フェードの持続時間
	float duration_ = 0.0f;
	// 経過時間カウンター
	float counter_ = 0.0f;
};