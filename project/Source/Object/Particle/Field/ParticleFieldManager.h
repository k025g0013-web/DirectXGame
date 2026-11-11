#pragma once

#include "ParticleField.h"

#include <vector>

namespace Tuna {
    class ParticleFieldManager {
    public:
        // Fieldを登録
        void AddField(const ParticleField &field);

        // 登録したFieldをすべて削除
        void ClearFields();

        // 指定位置に作用する効果を取得
        ParticleFieldResult GetEffectsAt(const Vector3 &position) const;

    private:
        // 指定位置がFieldの範囲内か判定
        bool IsInside(const AABB &area, const Vector3 &position) const;

    private:
        std::vector<ParticleField> fields_;
    };
}