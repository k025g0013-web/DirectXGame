#include "GameScene.h"
#ifdef _DEBUG
#include "2D/ImGuiManager.h"
#endif

using namespace KamataEngine;

 // コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() { delete model_; }

void GameScene::Initialize() {
	// ファイルを指定してファイルを読み込む
	textureHandle_ = TextureManager::Load("uvChecker.png");

	// カメラの初期化
	camera_.Initialize();

	// 3Dモデルデータの生成
	model_ = Model::Create();

	// プレイヤー
	player_ = new Player();
	player_->Initialize(model_, textureHandle_, &camera_);
}

void GameScene::Update() { player_->Update(); }

void GameScene::Draw() {
	Model::PreDraw(); // 開始

	player_->Draw();

	Model::PostDraw(); // 終了
}
