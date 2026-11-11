#pragma once

#include "Math/Collision/AABB.h"
#include "Math/Vector.h"

namespace Tuna {
    struct ParticleField {
        AABB area{};

        Vector3 acceleration{};
        float drag = 0.0f;
        Vector3 angularVelocity{};
    };

    struct ParticleFieldResult {
        Vector3 acceleration{};
        float drag = 0.0f;
        Vector3 angularVelocity{};
    };

    void WindField(ParticleField &field, const Vector3 &acceleration);
    void GravityField(ParticleField &field, const Vector3 &acceleration);
    void DragField(ParticleField &field, float drag);
    void RotationField(ParticleField &field, float angularVelocity);
}