#pragma once
#include "KamataEngine.h"
#include "UpdateWorldTransform.h"
#include "TitleLogo.h"
#include "Skydome.h"

class TitleScene {
public:
	TitleScene();	// コンストラクタ
	~TitleScene();	// デストラクタ

	void Initialize(); // 初期化
	void Update();     // 更新
	void Draw();       // 描画

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
	
	// 終了フラグ
	bool finished_ = false;
};