#include "GameScene.h"
#ifdef _DEBUG
#include "2D/ImGuiManager.h"
#endif

using namespace KamataEngine;

 // コンストラクタ
GameScene::GameScene() {}

// デストラクタ
GameScene::~GameScene() {
	delete sprite_;
	delete model_;
	delete debugCamera_;
}

void GameScene::Initialize() {
	// ハンドル
	textureHandle_ = TextureManager::Load("uvChecker.png");           // テクスチャ
	soundDetaHandle_ = Audio::GetInstance()->LoadWave("fanfare.wav"); // サウンドデータ

	// サウンド: ループ再生
	voiceHandle_ = Audio::GetInstance()->PlayWave(soundDetaHandle_, true);

	// インスタンス生成
	sprite_ = Sprite::Create(textureHandle_, {100, 50}); // スプライト
	model_ = Model::Create();                            // モデル

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// カメラの初期化
	// camera_.Initialize();

	// デバッグカメラの生成
	debugCamera_ = new DebugCamera(1280, 720);

	// 3Dライン描画が参照するカメラを指定する
	PrimitiveDrawer::GetInstance()->SetCamera(&debugCamera_->GetCamera());

	// 軸方向表示を有効にする
	AxisIndicator::GetInstance()->SetVisible(true);
	// 軸方向表示が参照するビュープロジェクションを指定する
	AxisIndicator::GetInstance()->SetTargetCamera(&debugCamera_->GetCamera());
}

void GameScene::Update() {
#ifdef _DEBUG
	// デバッグテキスト表示
	ImGui::Begin("Debug1");
	ImGui::Text("Kamata Tarou %d.%d.%d", 2050, 12, 31);

	ImGui::InputFloat3("InputFloat3", inputFloat3);              // 入力
	ImGui::SliderFloat3("InputFloat3", inputFloat3, 0.0f, 1.0f); // スライダー
	ImGui::End();

	// デモウィンドウの表示
	ImGui::ShowDemoWindow();
#endif

	// デバッグカメラの更新
	debugCamera_->Update();

	// サウンド: ループ再生の停止
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		Audio::GetInstance()->StopWave(soundDetaHandle_);
	}

	// サウンド: 再生
	// Audio::GetInstance()->PlayWave(soundDetaHandle_);

	/*
	// スプライトの座標を取得
	Vector2 position = sprite_->GetPosition();

	// 座標移動
	position.x += 2.0f;
	position.y += 1.0f;

	// 移動処理をスプライトに反映
	sprite_->SetPosition(position);
	*/
}

void GameScene::Draw() {
	// 3Dライン描画
	for (int i = 0; i < 25; i++) {
		// Z方向
		PrimitiveDrawer::GetInstance()->DrawLine3d({-(25 / 2) * 2.0f, 0, (i - 25 / 2) * 2.0f}, {(25 / 2) * 2.0f, 0, (i - 25 / 2) * 2.0f}, {1.0f, 0.0f, 0.0f, 1.0f});

		// X方向
		PrimitiveDrawer::GetInstance()->DrawLine3d({(i - 25 / 2) * 2.0f, 0, -(25 / 2) * 2.0f}, {(i - 25 / 2) * 2.0f, 0, (25 / 2) * 2.0f}, {0.0f, 0.0f, 1.0f, 1.0f});
	}

	// スプライト描画
	//=========================
	Sprite::PreDraw(); // 開始

	sprite_->Draw();

	Sprite::PostDraw(); // 終了

	// モデル描画
	//=========================
	Model::PreDraw(); // 開始

	model_->Draw(worldTransform_, debugCamera_->GetCamera(), textureHandle_);

	Model::PostDraw(); // 終了
}
