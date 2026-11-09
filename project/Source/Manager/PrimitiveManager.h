#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <vector>

#include "Object/Primitive/Primitive.h"
#include "Graphics/Resource/Texture.h"
#include "Math/Transform.h"

namespace Tuna {

    class PrimitiveManager {
    public:
        void Initialize(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList);

        void CreateTriangle(
            const Vector3 &left,
            const Vector3 &top,
            const Vector3 &right,
            const TextureData &texture,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void CreateSphere(
            int32_t subdivision,
            const TextureData &texture,
            uint32_t color = 0xFFFFFFFF,
            bool enableLighting = true);

        void Update(
            CameraManager *camera);

        void Draw(
            ID3D12GraphicsCommandList *commandList);

        std::vector<std::unique_ptr<Primitive>> &GetObjects() {
            return objects_;
        }

        std::vector<Transform> &GetTransforms() {
            return transforms_;
        }

        size_t GetObjectCount() const {
            return objects_.size();
        }

    private:
        ID3D12Device *device_ = nullptr;
        ID3D12GraphicsCommandList *commandList_ = nullptr;

        std::vector<std::unique_ptr<Primitive>> objects_;
        std::vector<Transform> transforms_;
    };
}