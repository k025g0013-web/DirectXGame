#include "BillboardMatrix.h"

#include "RenderCore/Camera/CameraManager.h"
#include "Math/MatrixFunctions.h"

#include <cmath>

namespace Tuna {

    Matrix4x4 BillboardMatrix::Make(
        const Transform &transform,
        const CameraManager &camera,
        BillboardMode mode
    ) {
        // カメラのTransformを取得
        const Transform &cameraTransform = camera.GetTransform();

        switch (mode) {

        case BillboardMode::None:
            // ビルボードなし：自身の回転を使用
            return Math::MakeAffineMatrix(
                transform.scale,
                transform.rotate,
                transform.translate
            );

        case BillboardMode::Full:
            // 全軸ビルボード：カメラと同じ回転を使用
            return Math::MakeAffineMatrix(
                transform.scale,
                cameraTransform.rotate,
                transform.translate
            );

        case BillboardMode::YAxis: {
            // Y軸ビルボード：Y軸方向の回転だけを計算する

            const float dx =
                cameraTransform.translate.x - transform.translate.x;

            const float dz =
                cameraTransform.translate.z - transform.translate.z;

            // 水平方向のカメラへの角度を計算
            const float rotateY = std::atan2(dx, dz);

            Vector3 rotation = {
                0.0f,
                rotateY,
                0.0f
            };

            return Math::MakeAffineMatrix(
                transform.scale,
                rotation,
                transform.translate
            );
        }

        }

        // 想定外のモードの場合
        return Math::MakeAffineMatrix(
            transform.scale,
            transform.rotate,
            transform.translate
        );
    }

}