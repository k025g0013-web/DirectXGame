#include "TitleScene.h"

using namespace KamataEngine;

// コンストラクタ
TitleScene::TitleScene() {};

// デストラクタ
TitleScene::~TitleScene() {
	delete titleLogo_;
	delete skydome_;
};

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
};

// 更新
void TitleScene::Update() {
	// スカイドーム
	skydome_->Update();

	// タイトルロゴ
	titleLogo_->Update();

	if (Input::GetInstance()->PushKey(DIK_SPACE)) {
		finished_ = true;
	}
};

// 描画
void TitleScene::Draw() {
	Model::PreDraw(); // 開始

	// スカイドーム
	skydome_->Draw();

	// タイトルロゴ
	titleLogo_->Draw();

	Model::PostDraw(); // 終了
};