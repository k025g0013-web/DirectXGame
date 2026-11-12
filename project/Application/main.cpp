#include "TunaEngine.h"
#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Object/Primitive/Primitive.h"
#include "Manager/ImGuiManager.h"

#include "Object/Primitive/TrianglePrimitive.h"
#include "Object/Primitive/PlanePrimitive.h"
#include "Object/Primitive/SpherePrimitive.h"

#include "Manager/ParticleManager.h"
#include "Object/Particle/BillboardMode.h"
#include "Object/Particle/Field/ParticleField.h"
#include <vector>

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

	// エリア効果の管理
	std::vector<ParticleField> particleFields;

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
		// ImGui
		//-------------------------------------------------------------------------
#ifdef USE_IMGUI

		ImGuiManager::GetInstance()->BeginFrame();

		ImGui::Begin("Particle Emitter");

		//=========================================================
		// エミッター設定
		//=========================================================

		ImGui::Text(
			"Particle Count: %d",
			particleManager.GetAliveCount()
		);

		ImGui::Checkbox(
			"Emitter Active",
			&isEmitterActive
		);

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

		//=========================================================
		// Particle Field設定
		//=========================================================

		ImGui::Separator();
		ImGui::Text("Particle Fields");

		// エリア追加
		if (ImGui::Button("Add Field")) {

			ParticleField field{};

			// エミッター位置を中心とした初期範囲
			field.area.min = {
				emitterPosition.x - 1.0f,
				emitterPosition.y - 1.0f,
				emitterPosition.z - 1.0f
			};

			field.area.max = {
				emitterPosition.x + 1.0f,
				emitterPosition.y + 1.0f,
				emitterPosition.z + 1.0f
			};

			particleFields.push_back(field);
		}

		// 登録済みエリアの設定
		for (size_t i = 0; i < particleFields.size();) {

			ParticleField &field = particleFields[i];

			ImGui::PushID(static_cast<int>(i));

			// エリアごとの設定を折りたたみ表示
			char fieldName[32];
			sprintf_s(fieldName, "Field %zu", i);

			bool isOpen = ImGui::TreeNode(fieldName);

			// 削除ボタン
			ImGui::SameLine();

			bool remove = ImGui::SmallButton("Remove");

			if (remove) {
				if (isOpen) {
					ImGui::TreePop();
				}

				ImGui::PopID();

				particleFields.erase(particleFields.begin() + i);
				continue;
			}

			if (isOpen) {

				//=================================================
				// エリアの範囲
				//=================================================

				ImGui::Text("Area");

				ImGui::DragFloat3(
					"Min",
					&field.area.min.x,
					0.1f
				);

				ImGui::DragFloat3(
					"Max",
					&field.area.max.x,
					0.1f
				);

				//=================================================
				// 加速度
				//=================================================

				ImGui::Separator();
				ImGui::Text("Acceleration");

				ImGui::DragFloat3(
					"Acceleration",
					&field.acceleration.x,
					0.1f
				);

				//=================================================
				// 抵抗
				//=================================================

				ImGui::Separator();

				ImGui::DragFloat(
					"Drag",
					&field.drag,
					0.01f,
					0.0f,
					100.0f
				);

				//=================================================
				// 回転
				//=================================================

				ImGui::DragFloat3(
					"Angular Velocity",
					&field.angularVelocity.x,
					0.01f
				);

				ImGui::TreePop();
			}

			ImGui::PopID();

			++i;
		}

		ImGui::End();

		ImGuiManager::GetInstance()->EndFrame();

		//=========================================================
		// ParticleFieldManagerへの反映
		//=========================================================

		// 現在の登録内容を一度クリア
		particleManager.ClearFields();

		// ImGuiで設定したエリアを再登録
		for (const ParticleField &field : particleFields) {
			particleManager.AddField(field);
		}

#endif

		//-------------------------------------------------------------------------
		// パーティクル更新
		//-------------------------------------------------------------------------
		particleManager.Update(deltaTime, &camera);

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