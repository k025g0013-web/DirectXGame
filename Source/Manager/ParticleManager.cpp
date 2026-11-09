#include "ParticleManager.h"

#include "Manager/CameraManager.h"
#include "Graphics/Core/DescriptorHeap.h"
#include "Graphics/Resource/TransformationMatrix.h"
#include "Math/FunctionMatrix.h"
#include "Graphics/Renderer/BillboardMatrix.h"

#include "Manager/TextureManager.h"

#include <algorithm>
#include <cassert>
#include <unordered_set>

namespace Tuna {

    void ParticleManager::Initialize(
        ID3D12Device *device,
        ID3D12GraphicsCommandList *commandList, DescriptorHeap *srvHeap,
        TextureManager *textureManager) {

        assert(device);
        assert(commandList);
        assert(srvHeap);
        assert(textureManager);

        device_ = device;
        commandList_ = commandList;
        srvHeap_ = srvHeap;
        textureManager_ = textureManager;

        // パーティクル用テクスチャローダーを初期化
        textureLoader_.Initialize(device_, srvHeap_);

        // パーティクル用の四角形メッシュを作成する。
        std::vector<VertexData> vertices(4);

        vertices[0].position = { -0.5f,  0.5f, 0.0f, 1.0f };
        vertices[0].texcoord = { 0.0f, 0.0f };
        vertices[0].normal = { 0.0f, 0.0f, -1.0f };

        vertices[1].position = { 0.5f,  0.5f, 0.0f, 1.0f };
        vertices[1].texcoord = { 1.0f, 0.0f };
        vertices[1].normal = { 0.0f, 0.0f, -1.0f };

        vertices[2].position = { -0.5f, -0.5f, 0.0f, 1.0f };
        vertices[2].texcoord = { 0.0f, 1.0f };
        vertices[2].normal = { 0.0f, 0.0f, -1.0f };

        vertices[3].position = { 0.5f, -0.5f, 0.0f, 1.0f };
        vertices[3].texcoord = { 1.0f, 1.0f };
        vertices[3].normal = { 0.0f, 0.0f, -1.0f };

        std::vector<uint32_t> indices = {
            0, 1, 2,
            2, 1, 3
        };

        mesh_.Create(device_, vertices, indices);

        // マテリアルを作成する。
        material_ = std::make_unique<Material>();
        material_->Initialize(device_, 0xFFFFFFFF, false);

        // インスタンスごとの変換行列を格納する構造化バッファを作成する。
        transformationBuffer_ = std::make_unique<StructuredBuffer>();

        const uint32_t descriptorIndex = srvHeap_->AllocateDescriptor();

        transformationBuffer_->Initialize(
            device_,
            srvHeap_,
            descriptorIndex,
            sizeof(TransformationMatrix),
            kMaxParticles
        );
    }

    void ParticleManager::Update(
        float deltaTime,
        CameraManager *camera
    ) {
        assert(camera);

        camera_ = camera;

        // パーティクル更新・削除
        for (size_t i = 0; i < particles_.size();) {
            Particle *particle = particles_[i].particle.get();

            particle->Update(deltaTime);

            if (!particle->IsAlive()) {
                particles_.erase(particles_.begin() + i);
            } else {
                ++i;
            }
        }
    }

    void ParticleManager::Draw(
        ID3D12GraphicsCommandList *commandList
    ) {
        assert(commandList);

        if (particles_.empty() || !camera_) {
            return;
        }

        assert(material_);
        assert(transformationBuffer_);

        commandList->SetGraphicsRootConstantBufferView(
            0,
            material_->GetGPUVirtualAddress()
        );

        mesh_.BindBuffers(commandList);

        // テクスチャとビルボードモードの組み合わせごとに描画する
        for (size_t first = 0; first < particles_.size(); ++first) {

            const std::string &texturePath =
                particles_[first].texturePath;

            const BillboardMode billboardMode =
                particles_[first].particle->GetBillboardMode();

            // 同じテクスチャ・同じモードのグループが
            // すでに描画されているか確認
            bool alreadyDrawn = false;

            for (size_t j = 0; j < first; ++j) {
                if (particles_[j].texturePath == texturePath &&
                    particles_[j].particle->GetBillboardMode() == billboardMode) {
                    alreadyDrawn = true;
                    break;
                }
            }

            if (alreadyDrawn) {
                continue;
            }

            const auto textureIt = particleTextures_.find(texturePath);
            assert(textureIt != particleTextures_.end());

            const D3D12_GPU_DESCRIPTOR_HANDLE textureHandle =
                textureIt->second.gpuHandle;

            std::vector<TransformationMatrix> matrices;
            matrices.reserve(particles_.size());

            // 同じテクスチャ・同じビルボードモードのパーティクルを集める
            for (const auto &instance : particles_) {

                Particle *particle = instance.particle.get();

                if (instance.texturePath != texturePath ||
                    particle->GetBillboardMode() != billboardMode) {
                    continue;
                }

                TransformationMatrix matrix{};

                // ビルボード方式に応じたワールド行列を生成
                matrix.World = BillboardMatrix::Make(
                    particle->GetTransform(),
                    *camera_,
                    particle->GetBillboardMode()
                );

                matrix.WVP = Math::Multiply(
                    matrix.World, camera_->GetViewProjectionMatrix());

                matrix.color = particle->GetColor();

                matrices.push_back(matrix);
            }

            if (matrices.empty()) {
                continue;
            }

            transformationBuffer_->Update(matrices.data(), matrices.size() * sizeof(TransformationMatrix));

            commandList->SetGraphicsRootDescriptorTable(1, transformationBuffer_->GetGPUHandle());

            commandList->SetGraphicsRootDescriptorTable(2, textureHandle);

            commandList->DrawIndexedInstanced(
                mesh_.GetIndexCount(), static_cast<UINT>(matrices.size()), 0, 0, 0);
        }
    }

    uint32_t ParticleManager::GetAliveCount() const {
        return static_cast<uint32_t>(particles_.size());
    }

    D3D12_GPU_DESCRIPTOR_HANDLE ParticleManager::LoadParticleTexture(
        const std::string &texturePath) {

        // すでに読み込まれている場合は再利用する
        auto it = particleTextures_.find(texturePath);

        if (it != particleTextures_.end()) {
            return it->second.gpuHandle;
        }

        // 新しいテクスチャを読み込む
        ParticleTexture particleTexture;
        particleTexture.texture = std::make_unique<Texture>();

        TextureData textureData = textureLoader_.LoadTexture(
            commandList_,
            texturePath,
            *particleTexture.texture
        );

        particleTexture.gpuHandle = textureData.gpuHandle;

        assert(particleTexture.gpuHandle.ptr != 0);

        // 読み込み済みテクスチャとして登録
        auto [insertedIt, inserted] = particleTextures_.emplace(
            texturePath,
            std::move(particleTexture)
        );

        assert(inserted);

        return insertedIt->second.gpuHandle;
    }

    void ParticleManager::AddEmitter(const Emitter &emitter) {
        emitters_.push_back(emitter);
    }
}