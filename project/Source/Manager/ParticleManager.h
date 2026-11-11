#pragma once

#include <d3d12.h>
#include <cstdint>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <type_traits>
#include <cassert>

#include "Object/Particle/Particle.h"
#include "Object/Particle/BillboardMode.h"
#include "Object/Particle/Field/ParticleFieldManager.h"

#include "Graphics/Resource/Mesh.h"
#include "Graphics/Resource/StructuredBuffer.h"
#include "Graphics/Material/Material.h"
#include "Graphics/Resource/Texture.h"
#include "Graphics/Resource/TextureLoader.h"


#include <format> 
#include <Windows.h>

namespace Tuna {

    class CameraManager;
    class DescriptorHeap;
    class TextureManager;

    struct ParticleTexture {
        std::unique_ptr<Texture> texture;
        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle{};
    };

    struct ParticleInstance {
        std::unique_ptr<Particle> particle;
        std::string texturePath;
    };

    struct Emitter {
        Transform transform{};
        uint32_t count = 3;
        float frequency = 0.1f;
        float frequencyTime = 0.0f;

        BillboardMode billboardMode = BillboardMode::Full;
    };

    class ParticleManager {
    public:
        void Initialize(
            ID3D12Device *device,
            ID3D12GraphicsCommandList *commandList,
            DescriptorHeap *srvHeap,
            TextureManager *textureManager
        );

        /// <summary>
        /// 指定した種類のパーティクルを生成する
        /// </summary>
        template<typename T = Particle>
        void Emit(
            const std::string &texturePath, uint32_t count,
            const Transform &transform, const Vector3 &velocity,
            float lifeTime, const Vector4 &color, 
            BillboardMode billboardMode = BillboardMode::Full) {
            
            static_assert(std::is_base_of_v<Particle, T>, "T must be derived from Particle.");

            EmitInternal<T>(texturePath, count, transform, velocity, lifeTime, color, billboardMode);
        }

        void Update(float deltaTime, CameraManager *camera);

        void Draw(ID3D12GraphicsCommandList *commandList);

        void AddEmitter(const Emitter &emitter);

        uint32_t GetAliveCount() const;

        // Fieldを登録
        void AddField(const ParticleField &field);

        // Fieldをすべて削除
        void ClearFields();

    private:
        template<typename T>
        void EmitInternal(
            const std::string &texturePath, uint32_t count,
            const Transform &transform, const Vector3 &velocity, 
            float lifeTime, const Vector4 &color, BillboardMode billboardMode) {
            assert(textureManager_);

            if (count == 0 || lifeTime <= 0.0f) {
                return;
            }

            const uint32_t aliveCount = static_cast<uint32_t>(particles_.size());
            const uint32_t available = kMaxParticles - (std::min)(aliveCount, kMaxParticles);
            count = (std::min)(count, available);
            if (count == 0) {
                return;
            }

            // テクスチャを読み込む
            LoadParticleTexture(texturePath);
            for (uint32_t i = 0; i < count; ++i) {
                auto particle = std::make_unique<T>();
                particle->Initialize(transform, velocity, lifeTime, color, billboardMode);

                if (particle->IsAlive()) {
                    particles_.push_back({std::move(particle), texturePath});
                }
            }
        }

        D3D12_GPU_DESCRIPTOR_HANDLE LoadParticleTexture(
            const std::string &texturePath
        );

    private:
        ID3D12Device *device_ = nullptr;
        ID3D12GraphicsCommandList *commandList_ = nullptr;
        DescriptorHeap *srvHeap_ = nullptr;
        TextureManager *textureManager_ = nullptr;

        TextureLoader textureLoader_;

        std::vector<ParticleInstance> particles_;
        std::vector<Emitter> emitters_;

        std::unordered_map<std::string, ParticleTexture> particleTextures_;

        Mesh mesh_;
        std::unique_ptr<Material> material_;
        std::unique_ptr<StructuredBuffer> transformationBuffer_;

        CameraManager *camera_ = nullptr;

        ParticleFieldManager fieldManager_;

        static constexpr uint32_t kMaxParticles = 1024;
    };
}