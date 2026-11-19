#include "Lighting.h"

#include "Math/VectorFunctions.h"

namespace Tuna {
    void Lighting::Initialize(ID3D12Device *device) {

        constantBuffer_.InitializeAsConstant(
            device,
            sizeof(DirectionalLight)
        );

        lightingData_ = static_cast<DirectionalLight *>(
            constantBuffer_.Map()
            );

        lightingData_->color = {
            1.0f, 1.0f, 1.0f, 1.0f
        };

        lightingData_->direction = {
            0.0f, -1.0f, 0.0f
        };

        lightingData_->intensity = 1.0f;

        // 初期ライティング方式
        currentLightMode_ = LightingMode::None;
        SetLightMode(LightingMode::HalfLambert);

        // Phong反射モデルの初期設定
        lightingData_->phong.ambientColor = {
            1.0f, 1.0f, 1.0f, 1.0f
        };

        lightingData_->phong.specularColor = {
            1.0f, 1.0f, 1.0f, 1.0f
        };

        lightingData_->phong.ambientIntensity = 0.1f;
        lightingData_->phong.specularIntensity = 0.5f;
        lightingData_->phong.shininess = 32.0f;

        lightingData_->phong.padding = 0.0f;
        lightingData_->phong.cameraPosition = {};
        lightingData_->phong.padding2 = 0.0f;

        // Padding
        lightingData_->padding[0] = 0.0f;
        lightingData_->padding[1] = 0.0f;
        lightingData_->padding[2] = 0.0f;
    }

    void Lighting::Update() {

        // ライティング計算が正しく行えるよう光の方向を正規化する
        lightingData_->direction = Math::Normalize(lightingData_->direction);
    }

    void Lighting::Bind(
        UINT rootParameterIndex,
        ID3D12GraphicsCommandList *commandList
    ) {
        // ライト用定数バッファをシェーダへ設定する
        commandList->SetGraphicsRootConstantBufferView(
            rootParameterIndex,
            constantBuffer_.GetGPUVirtualAddress()
        );
    }

    void Lighting::SetLightMode(LightingMode mode) {

        // ライト情報が生成済みの場合のみ設定を反映する
        if (lightingData_) {
            currentLightMode_ = mode;
            lightingData_->lightMode = static_cast<int>(mode);
        }
    }

    void Lighting::SetAmbientColor(const Vector4 &color) {
        lightingData_->phong.ambientColor = color;
    }

    void Lighting::SetAmbientIntensity(float intensity) {
        lightingData_->phong.ambientIntensity =
            (std::max)(0.0f, intensity);
    }

    void Lighting::SetSpecularColor(const Vector4 &color) {
        lightingData_->phong.specularColor = color;
    }

    void Lighting::SetSpecularIntensity(float intensity) {
        lightingData_->phong.specularIntensity =
            (std::max)(0.0f, intensity);
    }

    void Lighting::SetShininess(float shininess) {
        lightingData_->phong.shininess =
            (std::max)(1.0f, shininess);
    }

    void Lighting::SetCameraPosition(const Vector3 &position) {
        lightingData_->phong.cameraPosition = position;
    }
}