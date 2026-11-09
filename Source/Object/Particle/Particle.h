#pragma once

#include "Math/Transform.h"
#include "BillboardMode.h"

namespace Tuna {

    /// <summary>
    /// パーティクル1個分の状態と更新処理を管理するクラス
    /// </summary>
    class Particle {
    public:
        virtual ~Particle() = default;

        /// <summary>
        /// パーティクルを初期化する
        /// </summary>
        virtual void Initialize(
            const Transform &transform, const Vector3 &velocity,
            float lifeTime, const Vector4 &color, BillboardMode billboardMode
        );

        /// <summary>
        /// パーティクルを更新する
        /// </summary>
        virtual void Update(float deltaTime);

        /// <summary>
        /// 生存しているか取得する
        /// </summary>
        bool IsAlive() const {
            return isAlive_;
        }

        /// <summary>
        /// Transformを取得する
        /// </summary>
        const Transform &GetTransform() const {
            return transform_;
        }

        /// <summary>
        /// Transformを変更する
        /// </summary>
        Transform &GetTransform() {
            return transform_;
        }

        /// <summary>
        /// 速度を取得する
        /// </summary>
        const Vector3 &GetVelocity() const {
            return velocity_;
        }

        /// <summary>
        /// 残り寿命を取得する
        /// </summary>
        float GetLifeTime() const {
            return lifeTime_;
        }

        /// <summary>
        /// 色を取得する
        /// </summary>
        Vector4 GetColor() const {
            return color_;
        }

        BillboardMode GetBillboardMode() const {
            return billboardMode_;
        }

        void SetBillboardMode(BillboardMode mode) {
            billboardMode_ = mode;
        }

    protected:

        // パーティクルのTransform
        Transform transform_{
            {1.0f, 1.0f, 1.0f},
            {0.0f, 0.0f, 0.0f},
            {0.0f, 0.0f, 0.0f}
        };

        // 移動速度
        Vector3 velocity_{};

        // 残り寿命と初期寿命
        float lifeTime_ = 0.0f;
        float initialLifeTime_ = 0.0f;

        // 色
        Vector4 color_{ 1.0f, 1.0f, 1.0f, 1.0f };

        // ビルボードの種類
        BillboardMode billboardMode_ = BillboardMode::kFull;

        // 生存フラグ
        bool isAlive_ = false;
    };
}