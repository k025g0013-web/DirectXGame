#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"
#include "TitleLogo.h"
#include "Skydome.h"
#include "Fade.h"

class TitleScene {
public:
	enum class Phase {
		kFadeIn,
		kMain,
		kFadeOut,
	};

public:
	TitleScene();
	~TitleScene();

	void Initialize();
	void Update();
	void Draw();

	// 終了フラグのgetter
	bool IsFinished() const { return finished_; };

private:
	// カメラ
	KamataEngine::Camera camera_;

	// タイトルロゴ
	TitleLogo* titleLogo_ = nullptr;
	KamataEngine::Model* modelTitleLogo_ = nullptr;

	// スカイドーム
	Skydome* skydome_ = nullptr;
	KamataEngine::Model* modelSkydome_ = nullptr;

	// フェード
	Fade* fade_ = nullptr;
	float duration_ = 1.0f;
	Phase phase_ = Phase::kFadeIn;
	
	// 終了フラグ
	bool finished_ = false;
};