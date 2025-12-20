#include "GameScene.h"
#include "KamataEngine.h"
#include <Windows.h>

using namespace KamataEngine;

// Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {
	// インスタンス生成
	DirectXCommon* dxCommon = DirectXCommon::GetInstance();   // DirectXCommon
	ImGuiManager* imguiManager = ImGuiManager::GetInstance(); // ImGuiManager
	GameScene* gameScene = new GameScene();                   // ゲームシーン

	// エンジンの初期化
	Initialize(L"LC1A_16_ツカモトキズナ_AL2");

	// ゲームシーンの初期化
	gameScene->Initialize();

	// メインループ
	while (true) {
		// エンジンの更新
		if (Update()) {
			break;
		}

		// 更新処理
		//=========================
		imguiManager->Begin(); // ImGui:開始

		// ゲームシーン
		gameScene->Update();

		imguiManager->End(); // ImGui:終了

		// 描画処理
		//=========================
		dxCommon->PreDraw(); // DirectX:開始

		// ゲームシーン
		gameScene->Draw();

		// ImGui
		imguiManager->Draw();

		// 軸方向表示
		AxisIndicator::GetInstance()->Draw();

		dxCommon->PostDraw(); // DirectX:終了

		// 強制終了コマンド(ESC)
		if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
			break;
		}
	}

	// ゲームシーンの開放
	delete gameScene;
	// nullptrの代入
	gameScene = nullptr;

	// エンジンの終了処理
	Finalize();
	return 0;
}