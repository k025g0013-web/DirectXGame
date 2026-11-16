#pragma once

#include "TunaEngine.h"

#include "RenderCore/Camera/CameraManager.h"
#include "RenderCore/Lighting.h"
#include "Manager/ParticleManager.h"

#include "Object/Particle/BillboardMode.h"
#include "Object/Particle/Field/ParticleField.h"

#include <memory>
#include <vector>
#include <chrono>

class Game {
public:
    Game() = default;
    ~Game() = default;

    // 初期化
    void Initialize(Tuna::TunaEngine *engine);

    // 更新
    void Update();

    // 描画
    void Draw();

    // 終了判定
    bool IsEndRequested() const { return isEndRequested_; }

private:
    // パーティクル生成
    void EmitParticles(float deltaTime);

    // ImGui
    void DrawImGui();

private:
    // エンジン
    Tuna::TunaEngine *engine_ = nullptr;

    // エンジンから取得するマネージャ
    Tuna::PrimitiveManager *primitiveManager_ = nullptr;
    Tuna::ModelManager *modelManager_ = nullptr;
    Tuna::SpriteManager *spriteManager_ = nullptr;

    // ゲーム内マネージャ
    Tuna::ParticleManager particleManager_;
    Tuna::Lighting lighting_;
    Tuna::CameraManager camera_;

    // カメラ
    Tuna::Transform cameraTransform_{
        .scale = {1.0f, 1.0f, 1.0f},
        .rotate = {0.3f, 0.0f, 0.0f},
        .translate = {0.0f, 1.5f, -5.0f}
    };

    // パーティクルフィールド
    std::vector<Tuna::ParticleField> particleFields_;

    // エミッター設定
    Tuna::Vector3 emitterPosition_{};
    Tuna::BillboardMode emitterBillboardMode_ =
        Tuna::BillboardMode::Full;

    enum class ParticleType {
        Normal,
        Fire,
    };

    ParticleType emitterParticleType_ = ParticleType::Normal;

    static constexpr float kEmitInterval = 0.5f;
    static constexpr int kEmitCount = 10;

    float emitTimer_ = 0.0f;
    bool isEmitterActive_ = true;

    // 時間計測
    using Clock = std::chrono::steady_clock;
    Clock::time_point previousFrameTime_{};

    // 終了フラグ
    bool isEndRequested_ = false;

    // 定数
    static constexpr int kClientWidth = 1280;
    static constexpr int kClientHeight = 720;
    static constexpr UINT kLightRegisterIndex = 3;
};