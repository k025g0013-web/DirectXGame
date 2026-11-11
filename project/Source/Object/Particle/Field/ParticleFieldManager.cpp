#include "ParticleFieldManager.h"

namespace Tuna {
    void ParticleFieldManager::AddField(const ParticleField &field) {
        fields_.push_back(field);
    }

    void ParticleFieldManager::ClearFields() {
        fields_.clear();
    }

    bool ParticleFieldManager::IsInside(const AABB &area, const Vector3 &position) const {
        return
            position.x >= area.min.x &&
            position.x <= area.max.x &&
            position.y >= area.min.y &&
            position.y <= area.max.y &&
            position.z >= area.min.z &&
            position.z <= area.max.z;
    }

    // 指定位置に作用する効果を取得
    ParticleFieldResult ParticleFieldManager::GetEffectsAt(const Vector3 &position) const {
        ParticleFieldResult result{};

        // 登録されているFieldをすべて確認
        for (const ParticleField &field : fields_) {
            // 範囲外なら次のFieldへ
            if (!IsInside(field.area, position)) {
                continue;
            }

            // 加速度を合算
            result.acceleration += field.acceleration;

            // 抵抗を合算
            result.drag += field.drag;

            // 回転速度を合算
            result.angularVelocity += field.angularVelocity;
        }

        return result;
    }

}