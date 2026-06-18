#include "TitleScene.h"

using namespace KamataEngine;

TitleScene::TitleScene() {}
TitleScene::~TitleScene() {
	delete titleLogo_;
	delete skydome_;
	delete fade_;
}

// 初期化
void TitleScene::Initialize() {
	// カメラの初期化
	camera_.farZ = 2000.0f;
	camera_.Initialize();

	// タイトルロゴ
	modelTitleLogo_ = Model::CreateFromOBJ("titleLogo", true);
	titleLogo_ = new TitleLogo;
	titleLogo_->Initialize(modelTitleLogo_, &camera_);

	// スカイドーム
	modelSkydome_ = Model::CreateFromOBJ("skydome", true);
	skydome_ = new Skydome;
	skydome_->Initialize(modelSkydome_, &camera_);

	// フェード
	fade_ = new Fade;
	fade_->Initialize();
	fade_->Start(Fade::Status::FadeIn, duration_);
}

// 更新
void TitleScene::Update() {
	skydome_->Update();   // スカイドーム
	titleLogo_->Update(); // タイトルロゴ

	// フェード
	fade_->Update();

	switch (phase_) {
	case Phase::kFadeIn:
		// メイン状態へ
		if (fade_->IsFinished()) {
			phase_ = Phase::kMain;
		}
		break;

	case Phase::kMain:
		// スペースキーが押されたらフェードアウト
		if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
			phase_ = Phase::kFadeOut;
			fade_->Start(Fade::Status::FadeOut, duration_);
		}
		break;

	case Phase::kFadeOut:
		// シーン終了フラグ
		if (fade_->IsFinished()) {
			finished_ = true;
		}
		break;
	}
};

// 描画
void TitleScene::Draw() {
	Model::PreDraw(); // 開始

	// スカイドーム
	skydome_->Draw();

	// タイトルロゴ
	titleLogo_->Draw();

	// フェード
	fade_->Draw();

	Model::PostDraw(); // 終了
};