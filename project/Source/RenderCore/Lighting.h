#pragma once

#include "Graphics/Resource/MeshBuffer.h"
#include "Math/Vector.h"

namespace Tuna {
    struct PhongReflectionModel {
        Vector4 ambientColor{ 1.0f, 1.0f, 1.0f, 1.0f }; // 環境光の色
        Vector4 specularColor{ 1.0f, 1.0f, 1.0f, 1.0f };// 鏡面反射の色

        float ambientIntensity = 0.1f;  // 環境光の強さ
        float specularIntensity = 0.5f; // 鏡面反射の強さ

        float shininess = 32.0f; // 光沢の鋭さ

        // ConstantBufferのアライメント調整用
        float padding = 0.0f;

        // カメラのワールド座標
        Vector3 cameraPosition{};

        // ConstantBufferのアライメント調整用
        float padding2 = 0.0f;
    };

    struct DirectionalLight {

        /// 光の色
        Vector4 color;

        /// 光の向き
        Vector3 direction;

        /// 光の強さ
        float intensity;

        /// ライティング方式
        int lightMode;

        /// ConstantBufferのアライメント調整用
        float padding[3];

        // Phong反射モデルの設定
        PhongReflectionModel phong;
    };

    /// <summary>
    /// シーン全体で使用する平行光源を管理するクラス
    /// </summary>
    /// <remarks>
    /// ライト情報を更新し、GPUへ定数バッファとして転送する。
    /// ライティング方式の切り替えも担当する。
    /// </remarks>
    class Lighting {
    public:

        /// <summary>
        /// ライティング方式
        /// </summary>
        enum class LightingMode {
            None,
            Lambert,
            HalfLambert,
            Phong,
            BlinnPhong,
        };

    public:

        /// <summary>
        /// ライトを初期化する
        /// </summary>
        /// <param name="device">Direct3Dデバイス</param>
        void Initialize(ID3D12Device *device);

        /// <summary>
        /// ライト情報を更新する
        /// </summary>
        void Update();

        /// <summary>
        /// ライト情報をGPUへ設定する
        /// </summary>
        /// <param name="rootParameterIndex">RootParameter番号</param>
        /// <param name="commandList">描画コマンドリスト</param>
        void Bind(
            UINT rootParameterIndex,
            ID3D12GraphicsCommandList *commandList
        );

        /// <summary>
        /// ライティング方式を変更する
        /// </summary>
        /// <param name="mode">設定するライティング方式</param>
        void SetLightMode(LightingMode mode);

        /// <summary>
        /// ライト情報を取得する
        /// </summary>
        DirectionalLight *GetLightingData() { return lightingData_; }

        /// <summary>
        /// 現在のライティング方式を取得する
        /// </summary>
        LightingMode GetLightMode() const { return currentLightMode_; }

        // Phong反射の設定
        void SetAmbientColor(const Vector4 &color);
        void SetAmbientIntensity(float intensity);

        void SetSpecularColor(const Vector4 &color);
        void SetSpecularIntensity(float intensity);

        void SetShininess(float shininess);

        // カメラの位置
        void SetCameraPosition(const Vector3 &position);

    private:

        /// ライト用定数バッファ
        MeshBuffer constantBuffer_;

        /// CPU側から更新するライト情報
        DirectionalLight *lightingData_ = nullptr;

        /// 現在のライティング方式
        LightingMode currentLightMode_{};
    };
}