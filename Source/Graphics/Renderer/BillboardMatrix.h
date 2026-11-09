#pragma once

#include "Math/Transform.h"
#include "BillboardMode.h"

namespace Tuna {

    class CameraManager;

    class BillboardMatrix {
    public:

        /// <summary>
        /// ビルボード方式に応じたワールド行列を生成する
        /// </summary>
        static Matrix4x4 Make(
            const Transform &transform,
            const CameraManager &camera,
            BillboardMode mode
        );
    };

}