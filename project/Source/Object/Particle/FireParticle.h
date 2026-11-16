#pragma once

#include "Object/Particle/Particle.h"

namespace Tuna {

    struct FireParticleSettings {
        // 発生
        float spawnRadius = 0.15f;
        float horizontalJitter = 0.25f;
        float speedJitter = 0.3f;
        float lifeJitter = 0.1f;
        float sizeJitter = 0.3f;

        // 動き
        float buoyancy = 0.5f;
        float swayStrength = 1.2f;
        float swayFrequency = 7.0f;
        float centerPull = 1.5f;
        float spinSpeed = 1.5f;

        // 見た目
        float flickerAmplitude = 0.15f;
        float flickerFrequency = 18.0f;
        float stretchY = 1.4f;
    };

    class FireParticle : public Particle {
    public:
        using Particle::Update;

        void Initialize(
            const Transform &transform,
            const Vector3 &velocity,
            float lifeTime,
            const Vector4 &color,
            BillboardMode billboardMode
        ) override;

        void Update(
            float deltaTime,
            const ParticleFieldResult &fieldResult
        ) override;

        // 全FireParticle共通の設定
        static void SetDefaultSettings(const FireParticleSettings &settings) {
            DefaultSettings() = settings;
        }

        static FireParticleSettings &DefaultSettings() {
            static FireParticleSettings settings;
            return settings;
        }

    private:
        // 個体ごとの設定
        FireParticleSettings settings_{};

        // 経過時間
        float elapsedTime_ = 0.0f;

        // 初期状態
        Vector3 spawnPosition_{};
        Vector3 baseScale_{ 1.0f, 1.0f, 1.0f };
        Vector4 baseColor_{ 1.0f, 1.0f, 1.0f, 1.0f };

        // 個体ごとのランダム値
        float phase_ = 0.0f;
        float spin_ = 0.0f;
    };

}