#include "Game.h"

#include "Object/Particle/FireParticle.h"
#include "Manager/ImGuiManager.h"
#include "Utils/RandomHelper.h"

#include <cassert>
#include <algorithm>
#include <cstdint>

using namespace Tuna;

void Game::Initialize(TunaEngine *engine) {

    engine_ = engine;
    assert(engine_ != nullptr);

    // エンジンからマネージャを取得
    primitiveManager_ = engine_->GetPrimitiveManager();
    modelManager_ = engine_->GetModelManager();
    spriteManager_ = engine_->GetSpriteManager();

    assert(primitiveManager_ != nullptr);
    assert(modelManager_ != nullptr);
    assert(spriteManager_ != nullptr);

    //=========================================
    // パーティクル
    //=========================================

    particleManager_.Initialize(
        engine_->GetDevice(),
        engine_->GetCommandList(),
        engine_->GetDirectXCommon()->GetSRVHeap(),
        engine_->GetTextureManager()
    );

    //=========================================
    // モデル
    //=========================================

    TextureData textures = engine_->GetTextureManager()->LoadTexture(
        engine_->GetCommandList(), "resources/monsterBall.png");

    primitiveManager_->CreateSphere(32, textures, 0xFFFFFFFF);

    //=========================================
    // ライティング
    //=========================================

    lighting_.Initialize(engine_->GetDevice());

    //=========================================
    // カメラ
    //=========================================

    camera_.Initialize(
        static_cast<float>(kClientWidth),
        static_cast<float>(kClientHeight),
        &engine_->Input()
    );

    //=========================================
    // オーディオ
    //=========================================

    engine_->GetAudioManager()->Load(
        "fanfare",
        "Resources/fanfare.wav"
    );

    engine_->GetAudioManager()->Play(
        "fanfare",
        false,
        1.0f
    );

    // フレーム時間の初期化
    previousFrameTime_ = Clock::now();
}

void Game::Update() {

    //=========================================
    // 入力更新
    //=========================================

    engine_->UpdateInput();

    engine_->GetAudioManager()->ClearFinishedVoices();

    //=========================================
    // デバッグカメラ切り替え
    //=========================================

#ifdef _DEBUG

    if (engine_->Input().TriggerKey(DIK_Q) ||
        engine_->Input().TriggerButton(XINPUT_GAMEPAD_A)) {

        camera_.ToggleCamera();
    }

#endif

    //=========================================
    // DeltaTime
    //=========================================

    const auto currentFrameTime = Clock::now();

    float deltaTime =
        std::chrono::duration<float>(
            currentFrameTime - previousFrameTime_
        ).count();

    previousFrameTime_ = currentFrameTime;

    // 極端な時間経過を制限
    deltaTime = (std::min)(deltaTime, 0.1f);

    //=========================================
    // カメラ更新
    //=========================================

    camera_.Update(cameraTransform_);

    //=========================================
    // パーティクル生成
    //=========================================

    EmitParticles(deltaTime);

    //=========================================
    // ImGui
    //=========================================

#ifdef USE_IMGUI

    DrawImGui();

    // ParticleFieldManagerへの反映
    particleManager_.ClearFields();

    for (const ParticleField &field : particleFields_) {
        particleManager_.AddField(field);
    }

#endif

    //=========================================
    // パーティクル更新
    //=========================================

    particleManager_.Update(deltaTime, &camera_);

    //=========================================
    // Lighting
    //=========================================

    lighting_.SetCameraPosition(camera_.GetTransform().translate);
    lighting_.Update();

    //=========================================
    // 終了判定
    //=========================================

    if (engine_->Input().PushKey(DIK_ESCAPE)) {
        isEndRequested_ = true;
    }
}

void Game::EmitParticles(float deltaTime) {

    if (!isEmitterActive_) {
        return;
    }

    emitTimer_ += deltaTime;

    while (emitTimer_ >= kEmitInterval) {

        emitTimer_ -= kEmitInterval;

        const Vector4 normalColors[] = {
            { 1, 0, 0, 1 },
            { 0, 1, 0, 1 },
            { 0, 0, 1, 1 },
        };

        const Vector4 fireColors[] = {
            { 1, 0, 0, 1 },
            { 1, 1, 0, 1 },
        };

        for (int i = 0; i < kEmitCount; ++i) {

            Transform transform{};
            transform.translate = emitterPosition_;

            Vector3 particleVelocity = {
                RandHelperFloat(-1.0f, 1.0f),
                RandHelperFloat(-1.0f, 1.0f),
                RandHelperFloat(-1.0f, 1.0f)
            };

            switch (emitterParticleType_) {

            case ParticleType::Normal:

                particleManager_.Emit(
                    "resources/circle.png",
                    1,
                    transform,
                    particleVelocity,
                    3.0f,
                    normalColors[i % 3],
                    emitterBillboardMode_
                );

                break;

            case ParticleType::Fire:

                particleManager_.Emit<FireParticle>(
                    "resources/circle.png",
                    10,
                    {},
                    particleVelocity,
                    3.0f,
                    fireColors[i % 2],
                    emitterBillboardMode_
                );

                break;
            }
        }
    }
}

#ifdef USE_IMGUI

void Game::DrawImGui() {

    ImGuiManager::GetInstance()->BeginFrame();

    ImGui::Begin("Particle Emitter");

    //=========================================
    // パーティクルの種類
    //=========================================

    const char *particleItems[] = {
        "Normal",
        "Fire",
    };

    int particleIndex = static_cast<int>(emitterParticleType_);

    if (ImGui::Combo(
        "Particle Type",
        &particleIndex,
        particleItems,
        IM_ARRAYSIZE(particleItems))) {

        emitterParticleType_ =
            static_cast<ParticleType>(particleIndex);
    }

    //=========================================
    // エミッター設定
    //=========================================

    ImGui::Text(
        "Particle Count: %d",
        particleManager_.GetAliveCount()
    );

    ImGui::Checkbox(
        "Emitter Active",
        &isEmitterActive_
    );

    ImGui::DragFloat3(
        "Emitter Position",
        &emitterPosition_.x,
        0.1f
    );

    const char *billboardItems[] = {
        "None",
        "Full",
        "YAxis"
    };

    int billboardIndex =
        static_cast<int>(emitterBillboardMode_);

    if (ImGui::Combo(
        "Billboard Mode",
        &billboardIndex,
        billboardItems,
        IM_ARRAYSIZE(billboardItems))) {

        emitterBillboardMode_ =
            static_cast<BillboardMode>(billboardIndex);
    }

    //=========================================
    // Particle Field
    //=========================================

    ImGui::Separator();
    ImGui::Text("Particle Fields");

    if (ImGui::Button("Add Field")) {

        ParticleField field{};

        field.area.min = {
            emitterPosition_.x - 1.0f,
            emitterPosition_.y - 1.0f,
            emitterPosition_.z - 1.0f
        };

        field.area.max = {
            emitterPosition_.x + 1.0f,
            emitterPosition_.y + 1.0f,
            emitterPosition_.z + 1.0f
        };

        particleFields_.push_back(field);
    }

    //=========================================
    // 登録済みフィールド
    //=========================================

    for (size_t i = 0; i < particleFields_.size();) {

        ParticleField &field = particleFields_[i];

        ImGui::PushID(static_cast<int>(i));

        char fieldName[32];
        sprintf_s(fieldName, "Field %zu", i);

        bool isOpen = ImGui::TreeNode(fieldName);

        ImGui::SameLine();

        bool remove = ImGui::SmallButton("Remove");

        if (remove) {

            if (isOpen) {
                ImGui::TreePop();
            }

            ImGui::PopID();

            particleFields_.erase(
                particleFields_.begin() + i
            );

            continue;
        }

        if (isOpen) {

            // 範囲
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

            // 加速度
            ImGui::Separator();
            ImGui::Text("Acceleration");

            ImGui::DragFloat3(
                "Acceleration",
                &field.acceleration.x,
                0.1f
            );

            // 抵抗
            ImGui::Separator();

            ImGui::DragFloat(
                "Drag",
                &field.drag,
                0.01f,
                0.0f,
                100.0f
            );

            // 回転
            ImGui::Separator();

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

    const char *lightingModes[] = {
        "None",
        "Lambert",
        "Half Lambert",
        "Phong",
        "Blinn-Phong"
    };

    int currentMode = static_cast<int>(lighting_.GetLightMode());

    if (ImGui::Combo("Lighting Mode", &currentMode, lightingModes, IM_ARRAYSIZE(lightingModes))){
        lighting_.SetLightMode(
            static_cast<Lighting::LightingMode>(currentMode)
        );
    }

    if (ImGui::TreeNode("Phong Reflection")) {

        PhongReflectionModel &phong =
            lighting_.GetLightingData()->phong;

        ImGui::ColorEdit4(
            "Ambient Color",
            &phong.ambientColor.x
        );

        ImGui::DragFloat(
            "Ambient Intensity",
            &phong.ambientIntensity,
            0.01f,
            0.0f,
            2.0f
        );

        ImGui::ColorEdit4(
            "Specular Color",
            &phong.specularColor.x
        );

        ImGui::DragFloat(
            "Specular Intensity",
            &phong.specularIntensity,
            0.01f,
            0.0f,
            5.0f
        );

        ImGui::DragFloat(
            "Shininess",
            &phong.shininess,
            1.0f,
            1.0f,
            256.0f
        );

        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Sphere Settings")) {

        auto &transforms = primitiveManager_->GetTransforms();

        if (!transforms.empty()) {
            Transform &sphereTransform = transforms[0];

            ImGui::DragFloat3(
                "Sphere Scale",
                &sphereTransform.scale.x,
                0.01f,
                0.01f,
                10.0f
            );

            if (ImGui::Button("Reset Scale")) {
                sphereTransform.scale = { 1.0f, 1.0f, 1.0f };
            }
        }

        ImGui::TreePop();
    }
    ImGui::End();

    ImGuiManager::GetInstance()->EndFrame();
}

#endif

void Game::Draw() {

    engine_->BeginFrame();

    //=========================================
    // 3Dオブジェクト
    //=========================================

    engine_->SetPipeline(
        PipelineMode::Object3DOpaque
    );

    lighting_.Bind(
        kLightRegisterIndex,
        engine_->GetCommandList()
    );

    // プリミティブ
    primitiveManager_->Update(&camera_);
    primitiveManager_->Draw(engine_->GetCommandList());

    // モデル
    modelManager_->Update(&camera_);
    modelManager_->Draw(engine_->GetCommandList());

    //=========================================
    // パーティクル
    //=========================================

    engine_->SetPipeline(
        PipelineMode::Particle
    );

    lighting_.Bind(
        kLightRegisterIndex,
        engine_->GetCommandList()
    );

    particleManager_.Draw(engine_->GetCommandList());

    //=========================================
    // 2Dオブジェクト
    //=========================================

    engine_->SetPipeline(
        PipelineMode::Object2DOpaque
    );

    spriteManager_->Update(
        kClientWidth,
        kClientHeight
    );

    spriteManager_->Draw(engine_->GetCommandList());

    engine_->EndFrame();
}