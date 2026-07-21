#include "GameScene.h"
#include "KamataEngine.h"
#include "StageManager.h"
#include "TitleScene.h"
#include <Windows.h>

using namespace KamataEngine;

// グローバル変数
StageManager* stageManager = nullptr; // ステージマネージャ
TitleScene* titleScene = nullptr;     // タイトルシーン
GameScene* gameScene = nullptr;       // ゲームシーン

// シーン (型)
enum class Scene {
	kUnknown = 0,
	kTitle,
	kGame,
};

// 現在シーン (型)
Scene scene = Scene::kUnknown;

// シーン切り替え処理
static void ChangeScene() {
	switch (scene) {
	case Scene::kTitle:
		if (titleScene->IsFinished()) {
			// シーン変更
			scene = Scene::kGame;
			// 旧シーンの解放
			delete titleScene;
			titleScene = nullptr;
			// 新シーンの生成と初期化
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}
		break;
	case Scene::kGame:
		if (gameScene->IsFinished()) {
			// シーン変更
			scene = Scene::kTitle;
			// 旧シーンの解放
			delete gameScene;
			gameScene = nullptr;
			// 新シーンの生成と初期化
			titleScene = new TitleScene;
			titleScene->Initialize();
		} else if (gameScene->hasReloadRequested()) {
			delete gameScene;
			gameScene = nullptr;
			gameScene = new GameScene;
			gameScene->Initialize(stageManager);
		}
		break;
	}
};

// シーンの更新
static void UpdateScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Update();
		break;
	case Scene::kGame:
		gameScene->Update();
		break;
	}
};

// シーンの描画
static void DrawScene() {
	switch (scene) {
	case Scene::kTitle:
		titleScene->Draw();
		break;
	case Scene::kGame:
		gameScene->Draw();
		break;
	}
};

// 設定ファイルの読み込み
static void LoadDebugSettings() {
	std::ifstream file("debugSettings.ini");
	if (!file.is_open()) {
		return;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line.empty()) continue;

		std::stringstream lineStream(line);
		std::string key, value;

		if (lineStream >> key >> value) {
			if (key == "InitialStage") {
				stageManager->SetCurrentStageIndexByName(value);
			}
		}
	}
}

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	// インスタンス生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();   // DirectXCommon
	ImGuiManager* imguiManager = ImGuiManager::GetInstance(); // ImGuiManager

	stageManager = new StageManager;
	stageManager->LoadStageData();
	LoadDebugSettings();

	// 最初のシーン初期化
	scene = Scene::kTitle;
	titleScene = new TitleScene();

	// エンジンの初期化
	Initialize(L"LC1A_16_ツカモトキズナ_AL2");

	titleScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// 更新処理
		//=========================
		imguiManager->Begin(); // ImGui:開始

		// シーンの切り替え
		ChangeScene();
		// 現在シーン更新
		UpdateScene();

		imguiManager->End(); // ImGui:終了

		// 描画処理
		//=========================
		dxCommon->PreDraw(); // DirectX:開始

		// 現在シーン描画
		DrawScene();

		// 軸方向表示
		AxisIndicator::GetInstance()->Draw();

		imguiManager->Draw();

		dxCommon->PostDraw(); // DirectX:終了

		// 強制終了コマンド(ESC)
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			break;
		}
	}

	// ステージマネージャの解放
	delete stageManager;
	// タイトルシーンの開放
	delete titleScene;
	// ゲームシーンの開放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	Finalize();
	return 0;
}
