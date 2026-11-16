#include "FireParticle.h"
#include "Utils/RandomHelper.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <numbers>

namespace Tuna {

    namespace {
        // 0～1に収める
        float Saturate(float value) {
            return std::clamp(value, 0.0f, 1.0f);
        }

        // 滑らかな補間
        float SmoothStep(float start, float end, float value) {
            float t = Saturate((value - start) / (end - start));
            return t * t * (3.0f - 2.0f * t);
        }

        // 寿命に応じた炎の色
        Vector3 GetFireColor(float life) {
            if (life > 0.6f) {
                float t = (life - 0.6f) / 0.4f;
                return {
                    1.0f,
                    0.4f + 0.55f * t,
                    0.05f + 0.65f * t
                };
            }

            float t = life / 0.6f;
            return {
                0.6f + 0.4f * t,
                0.1f + 0.3f * t,
                0.02f + 0.03f * t
            };
        }

    }

    void FireParticle::Initialize(
        const Transform &transform,
        const Vector3 &velocity,
        float lifeTime,
        const Vector4 &color,
        BillboardMode billboardMode
    ) {
        settings_ = DefaultSettings();

        // 初速に個体差を付ける
        Vector3 randomVelocity = velocity;
        randomVelocity.x += RandHelperFloat(-settings_.horizontalJitter, settings_.horizontalJitter);
        randomVelocity.z += RandHelperFloat(-settings_.horizontalJitter, settings_.horizontalJitter);
        randomVelocity.y *= RandHelperFloat(
            1.0f - settings_.speedJitter, 1.0f + settings_.speedJitter
        );

        float randomLife = lifeTime * RandHelperFloat(
            1.0f - settings_.lifeJitter, 1.0f + settings_.lifeJitter
        );

        Particle::Initialize(
            transform,
            randomVelocity,
            randomLife,
            color,
            billboardMode
        );

        // 発生位置を少し散らす
        float angle = RandHelperFloat(0.0f, 2.0f * std::numbers::pi_v<float>);
        float radius = RandHelperFloat(0.0f, settings_.spawnRadius);

        transform_.translate.x += std::cos(angle) * radius;
        transform_.translate.z += std::sin(angle) * radius;

        spawnPosition_ = transform_.translate;

        // 初期サイズ
        float size = RandHelperFloat(
            1.0f - settings_.sizeJitter,
            1.0f + settings_.sizeJitter
        );

        baseScale_ = {
            transform.scale.x * size,
            transform.scale.y * size,
            transform.scale.z * size
        };

        // 回転と揺れに個体差を付ける
        transform_.rotate.z = RandHelperFloat(0.0f, 2.0f * std::numbers::pi_v<float>);
        phase_ = RandHelperFloat(0.0f, 2.0f * std::numbers::pi_v<float>);
        spin_ = RandHelperFloat(-settings_.spinSpeed, settings_.spinSpeed);

        baseColor_ = color;
        elapsedTime_ = 0.0f;
    }

    void FireParticle::Update(
        float deltaTime,
        const ParticleFieldResult &fieldResult
    ) {
        if (!IsAlive()) {
            return;
        }

        //================================
        // 移動
        //================================

        velocity_.y += settings_.buoyancy * deltaTime;

        Particle::Update(deltaTime, fieldResult);

        if (!IsAlive()) {
            return;
        }

        elapsedTime_ += deltaTime;

        float life = Saturate(1.0f - lifeTime_ / initialLifeTime_);

        //================================
        // 横揺れ
        //================================

        float time = elapsedTime_ * settings_.swayFrequency;

        float swayX = std::sin(time + phase_);
        float swayZ = std::cos(time + phase_);

        transform_.translate.x += swayX * settings_.swayStrength * deltaTime;
        transform_.translate.z += swayZ * settings_.swayStrength * deltaTime;

        // 中心に向かって収束
        transform_.translate.x +=
            (spawnPosition_.x - transform_.translate.x)
            * settings_.centerPull * life * deltaTime;

        transform_.translate.z +=
            (spawnPosition_.z - transform_.translate.z)
            * settings_.centerPull * life * deltaTime;

        // 回転
        transform_.rotate.z += spin_ * deltaTime;

        //================================
        // 拡縮
        //================================

        float grow = SmoothStep(0.0f, 0.15f, life);
        float shrink = std::pow(1.0f - life, 0.7f);

        float flicker = 1.0f +
            std::sin(elapsedTime_ * settings_.flickerFrequency + phase_)
            * settings_.flickerAmplitude;

        float scale = grow * shrink * flicker;

        transform_.scale.x = baseScale_.x * scale;
        transform_.scale.y = baseScale_.y * scale * settings_.stretchY;
        transform_.scale.z = baseScale_.z * scale;

        //================================
        // 色・透明度
        //================================

        Vector3 fireColor = GetFireColor(life);

        color_.x = fireColor.x * baseColor_.x;
        color_.y = fireColor.y * baseColor_.y;
        color_.z = fireColor.z * baseColor_.z;

        color_.w =
            SmoothStep(0.0f, 0.08f, life)
            * std::pow(1.0f - life, 1.3f)
            * baseColor_.w;
    }

}