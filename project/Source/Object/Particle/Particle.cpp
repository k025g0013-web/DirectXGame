#include "Particle.h"

#include <algorithm>
#include <cassert>

namespace Tuna {

    void Particle::Initialize(
        const Transform& transform, const Vector3& velocity,
        float lifeTime, const Vector4& color, BillboardMode billboardMode
    ) {
        transform_ = transform;
        velocity_ = velocity;

        lifeTime_ = lifeTime;
        initialLifeTime_ = lifeTime;

        color_ = color;
        color_.w = 1.0f;

        billboardMode_ = billboardMode;

        isAlive_ = lifeTime_ > 0.0f;
    }

    void Particle::Update(float deltaTime) {
        if (!isAlive_) {
            return;
        }

        assert(deltaTime >= 0.0f);

        if (deltaTime < 0.0f) {
            return;
        }

        // 位置を更新
        transform_.translate.x += velocity_.x * deltaTime;
        transform_.translate.y += velocity_.y * deltaTime;
        transform_.translate.z += velocity_.z * deltaTime;

        // 残り寿命を更新
        lifeTime_ = (std::max)(0.0f, lifeTime_ - deltaTime);

        // 残り寿命の割合を透明度に反映
        if (initialLifeTime_ > 0.0f) {
            color_.w = lifeTime_ / initialLifeTime_;
        }

        if (lifeTime_ <= 0.0f) {
            isAlive_ = false;
        }
    }

    // Fieldの効果を受けた更新
    void Particle::Update(
        float deltaTime,
        const ParticleFieldResult &fieldResult
    ) {
        if (!isAlive_) {
            return;
        }

        assert(deltaTime >= 0.0f);

        if (deltaTime < 0.0f) {
            return;
        }

        // 加速度を速度に反映
        velocity_ += fieldResult.acceleration * deltaTime;

        // 抵抗を適用
        const float drag = (std::max)(0.0f, fieldResult.drag);

        const float damping = (std::max)(
            0.0f,
            1.0f - drag * deltaTime
            );

        velocity_ = velocity_ * damping;

        // 回転を更新
        transform_.rotate.x += fieldResult.angularVelocity.x * deltaTime;
        transform_.rotate.y += fieldResult.angularVelocity.y * deltaTime;
        transform_.rotate.z += fieldResult.angularVelocity.z * deltaTime;

        // 通常の位置・寿命更新
        Update(deltaTime);
    }
}