#include "TunaEngine.h"
#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Object/Primitive/Primitive.h"
#include "Manager/ImGuiManager.h"

#include "Object/Primitive/Triangle.h"
#include "Object/Primitive/Plane.h"
#include "Object/Primitive/Sphere.h"

#include "Manager/ParticleManager.h"
#include "Object/Particle/BillboardMode.h"

#include "Utils/RandomHelper.h"

#include <cstdint>
#include <algorithm>
#include <chrono>

using namespace Tuna;

namespace {
	// クライアント領域のサイズ
	constexpr int32_t kClientWidth = 1280;
	constexpr int32_t kClientHeight = 720;

	// ライトバインド用の定数バッファレジスタ番号
	constexpr UINT kLightRegisterIndex = 3;
}

// Windowsアプリでのエントリーポイント
int WINAPI WinMain(_In_ HINSTANCE, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int) {

	//-------------------------------------------------------------------------
	// 基盤初期化
	//-------------------------------------------------------------------------
	auto engine = std::make_unique<TunaEngine>();

	engine->Initialize(L"TunaEngine", kClientWidth, kClientHeight);

	ID3D12Device *device = engine->GetDevice();

	ID3D12GraphicsCommandList *commandList = engine->GetCommandList();

	//-------------------------------------------------------------------------
	// データ生成・初期リソースのセットアップ
	//-------------------------------------------------------------------------
	PrimitiveManager *primitiveManager = engine->GetPrimitiveManager();

	ModelManager *modelManager = engine->GetModelManager();

	SpriteManager *spriteManager = engine->GetSpriteManager();
	assert(engine != nullptr);
	assert(spriteManager != nullptr);

	//-------------------------------------------------------------------------
	// パーティクル
	//-------------------------------------------------------------------------
	ParticleManager particleManager;
	particleManager.Initialize(device, commandList,
		engine->GetDirectXCommon()->GetSRVHeap(), engine->GetTextureManager());

	Vector3 emitterPosition = {};			// 生成座標
	BillboardMode emitterBillboardMode = BillboardMode::Full;	// ビルボードの種類
	constexpr float kEmitInterval = 0.5f;	// 何秒ごとに生成するか
	constexpr int kEmitCount = 10;			// いくつ生成するか
	float emitTimer = 0.0f;					// タイマー 
	bool isEmitterActive = true;			// 生成するか否か

	//-------------------------------------------------------------------------
	// 初期オブジェクト
	//-------------------------------------------------------------------------
	// スプライト
	std::vector<std::unique_ptr<Sprite>> sprites;
	std::vector<int> spriteTextureIndices;

	// 初期モデル
	modelManager->CreateModel(commandList, "fence");

	//-------------------------------------------------------------------------
	// ライト / カメラ
	//-------------------------------------------------------------------------

	Lighting lighting;
	lighting.Initialize(device);

	CameraManager camera;

	camera.Initialize(
		float(kClientWidth),
		float(kClientHeight),
		&engine->Input());

	Transform cameraTransform = {
		.scale = {1.0f, 1.0f, 1.0f},
		.rotate = {0.3f, 0.0f, 0.0f},
		.translate = {0.0f, 1.5f, -5.0f}
	};

	//-------------------------------------------------------------------------
	// オーディオ
	//-------------------------------------------------------------------------

	engine->GetAudioManager()->Load("fanfare", "Resources/fanfare.wav");
	engine->GetAudioManager()->Play("fanfare", false, 1.0f);

	using Clock = std::chrono::steady_clock;
	auto previousFrameTime = Clock::now();

	//-------------------------------------------------------------------------
	// メインループ
	//-------------------------------------------------------------------------

	while (engine->ProcessMessage()) {
		engine.get()->GetAudioManager()->ClearFinishedVoices();


		//=========================================================================
		// 入力更新
		//=========================================================================

		engine->UpdateInput();

		//=========================================================================
		// カメラ更新
		//=========================================================================

#ifdef _DEBUG

		if (engine->Input().TriggerKey(DIK_Q) ||
			engine->Input().TriggerButton(XINPUT_GAMEPAD_A)) {
			camera.ToggleCamera();
		}

#endif

		// 実際のフレーム経過時間を計算
		const auto currentFrameTime = Clock::now();

		float deltaTime =
			std::chrono::duration<float>(
				currentFrameTime - previousFrameTime).count();

		previousFrameTime = currentFrameTime;

		// デバッグ停止などによる極端な時間経過を制限
		deltaTime = (std::min)(deltaTime, 0.1f);

		//-------------------------------------------------------------------------
		// カメラ更新
		//-------------------------------------------------------------------------
		camera.Update(cameraTransform);

		//-------------------------------------------------------------------------
		// パーティクル生成
		//-------------------------------------------------------------------------
		if (isEmitterActive) {

			emitTimer += deltaTime;

			// 経過時間が0.5秒以上なら生成
			// whileにすることで、時間経過が大きくなっても生成漏れを防ぐ
			while (emitTimer >= kEmitInterval) {

				emitTimer -= kEmitInterval;

				const Vector4 colors[] = {
					{ 1,0,0,1 }, // 赤
					{ 0,1,0,1 }, // 緑
					{ 0,0,1,1 }, // 青
				};

				for (int i = 0; i < kEmitCount; ++i) {

					Transform transform{};
					transform.translate = emitterPosition;


					for (const Vector4 &color : colors) {
						Vector3 particleVelocity = {
							RandHelperFloat(-2.0f, 2.0f),
							RandHelperFloat(-2.0f, 2.0f),
							RandHelperFloat(-2.0f, 2.0f)
						};

						particleManager.Emit("resources/circle.png", 1, transform,
							particleVelocity, 3.0f, color, emitterBillboardMode
						);
					}
				}
			}
		}

		//-------------------------------------------------------------------------
		// パーティクル更新
		//-------------------------------------------------------------------------
		particleManager.Update(deltaTime, &camera);

		//-------------------------------------------------------------------------
		// ImGui
		//-------------------------------------------------------------------------
#ifdef USE_IMGUI

		ImGuiManager::GetInstance()->BeginFrame();

		ImGui::Begin("Particle Emitter");

		ImGui::Text(
			"Particle Count: %d",
			particleManager.GetAliveCount()
		);

		// エミッターの有効・無効
		ImGui::Checkbox(
			"Emitter Active",
			&isEmitterActive
		);

		// 生成位置
		ImGui::DragFloat3(
			"Emitter Position",
			&emitterPosition.x,
			0.1f
		);

		// ビルボードモード
		const char *billboardItems[] = {
			"None",
			"Full",
			"YAxis"
		};

		int billboardIndex = static_cast<int>(emitterBillboardMode);

		if (ImGui::Combo(
			"Billboard Mode",
			&billboardIndex,
			billboardItems,
			IM_ARRAYSIZE(billboardItems))) {

			emitterBillboardMode =
				static_cast<BillboardMode>(billboardIndex);
		}

		ImGui::End();

		ImGuiManager::GetInstance()->EndFrame();

#endif

		//-------------------------------------------------------------------------
		// Lighting
		//-------------------------------------------------------------------------
		lighting.Update();

		//-------------------------------------------------------------------------
		// 描画処理
		//-------------------------------------------------------------------------

		engine->BeginFrame();

		// 3Dオブジェクト
		{
			engine->SetPipeline(
				PipelineMode::Object3DOpaque);

			lighting.Bind(kLightRegisterIndex, commandList);

			// プリミティブ
			primitiveManager->Update(
				&camera);

			primitiveManager->Draw(
				commandList);


			// モデル
			modelManager->Update(&camera);
			modelManager->Draw(commandList);
		}

		// パーティクル
		{
			engine->SetPipeline(PipelineMode::Particle);

			lighting.Bind(kLightRegisterIndex, commandList);

			particleManager.Draw(commandList);
		}

		// 2Dオブジェクト
		{
			engine->SetPipeline(PipelineMode::Object2DOpaque);

			spriteManager->Update(
				kClientWidth,
				kClientHeight);

			spriteManager->Draw(commandList);
		}

		engine->EndFrame();

		// ESC
		if (engine->Input().PushKey(DIK_ESCAPE)) {
			break;
		}
	}

	//-------------------------------------------------------------------------
	// 後処理
	//-------------------------------------------------------------------------

	engine->Finalize();

	return 0;
}