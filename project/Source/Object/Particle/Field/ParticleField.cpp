#include "ParticleField.h"

namespace Tuna {

    // 風の効果を追加
    void WindField(ParticleField &field, const Vector3 &acceleration) {
        field.acceleration += acceleration;
    }

    // 重力の効果を追加
    void GravityField(ParticleField &field, const Vector3 &acceleration) {
        field.acceleration += acceleration;
    }

    // 抵抗の効果を追加
    void DragField(ParticleField &field, float drag) {
        field.drag += drag;
    }

    // 回転の効果を追加
    void RotationField(ParticleField &field, Vector3 angularVelocity) {
        field.angularVelocity += angularVelocity;
    }

}