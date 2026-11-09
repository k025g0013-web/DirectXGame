#include "TunaEngine.h"

#include "Manager/ImGuiManager.h"
#include "Graphics/Pipeline/RootSignature.h"
#include "Graphics/Pipeline/GraphicsPipeline.h"

namespace Tuna {

    void TunaEngine::Initialize(
        const std::wstring &title,
        int32_t width,
        int32_t height
    ) {
        // 最優先システムの構築
        debugManager_ = std::make_unique<DebugManager>();
        
        // ログ管理を初期化する
        Logger::Initialize();

        // アプリケーションウィンドウの生成
        winApp_ = std::make_unique<WinApp>();
        winApp_->Initialize(
            title.c_str(),
            width,
            height
        );

#ifdef _DEBUG
        // DirectX12のデバッグレイヤーを有効化
        debugManager_->EnableDebugLayer();
#endif

        // DirectX12 コアグラフィックス環境の構築
        directXCommon_ = std::make_unique<DirectXCommon>();

        directXCommon_->Initialize(
            winApp_.get(),
            width,
            height
        );

        ID3D12Device *device =
            directXCommon_->GetDevice();
      
        // 各種リソース・情報マネージャの初期化
        // テクスチャ
        textureManager_ = std::make_unique<TextureManager>();

        textureManager_->Initialize(
            device,
            directXCommon_->GetSRVHeap()
        );

        // オーディオ
        audioManager_ = std::make_unique<AudioManager>();
        audioManager_->Initialize();

        // 頂点生成オブジェクト
        primitiveManager_ = std::make_unique<PrimitiveManager>();
        primitiveManager_->Initialize(device, GetCommandList());

        // モデルオブジェクト
        modelManager_ = std::make_unique<ModelManager>();
        modelManager_->Initialize(device, textureManager_.get());

        // スプライトマネージャー
        spriteManager_ = std::make_unique<SpriteManager>();
        spriteManager_->Initialize(device);

        // 入力デバイスの初期化
        inputManager_ = std::make_unique<InputManager>();
        inputManager_->Initialize(winApp_.get());

        winApp_->SetInputMouse(
            inputManager_.get()->GetMouse()
        );

        // レンダリング環境・パイプラインの構築
        pipelineManager_ =
            std::make_unique<PipelineManager>();

        pipelineManager_->Initialize(
            device
        );

        // グローバルライト
        lighting_ = std::make_unique<Lighting>();

        lighting_->Initialize(device);

#ifdef USE_IMGUI
        // ImGui
        ImGuiManager::GetInstance()->Initialize(
            winApp_.get()->GetHWND(),
            device,
            directXCommon_->GetRenderOutput()->GetBufferCount(),
            directXCommon_->GetRenderOutput()->GetRTVDesc().Format,
            directXCommon_->GetSRVHeap()->GetDescriptorHeap(),
            directXCommon_->GetSRVHeap()->GetCPUDescriptorHandle(0),
            directXCommon_->GetSRVHeap()->GetGPUDescriptorHandle(0)
        );

        ImGuiIO &io = ImGui::GetIO();

        io.ConfigFlags |=
            ImGuiConfigFlags_DockingEnable;

#endif
    }


    void TunaEngine::Finalize() {

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->Finalize();

#endif

        // 入力
        if (inputManager_) {
            inputManager_->Finalize();
            inputManager_.reset();
        }

        // テクスチャ
        if (textureManager_) {
            textureManager_->Finalize();
        }

        // オーディオ
        if (audioManager_) {
            audioManager_->Finalize();
        }

        // ウィンドウ
        if (winApp_) {
            winApp_->Finalize();
        }
    }


    bool TunaEngine::ProcessMessage() {

        return winApp_->ProcessMessage();
    }


    void TunaEngine::BeginFrame() {

        directXCommon_->BeginFrame();
    }


    void TunaEngine::EndFrame() {

#ifdef USE_IMGUI

        ImGuiManager::GetInstance()->Draw(
            directXCommon_->GetCommandList()
        );

#endif

        directXCommon_->EndFrame();

        if (inputManager_) {
            inputManager_->EndFrame();
        }
    }


    void TunaEngine::UpdateInput() {
        if (inputManager_) {
            inputManager_->Update();
        }
    }


    void TunaEngine::SetPipeline(
        PipelineMode mode
    ) {
        // PipelineManagerにパイプライン取得を任せる
        const PipelineState *pipeline =
            pipelineManager_->GetPipeline(mode);

        // コマンドリストを取得
        ID3D12GraphicsCommandList *commandList =
            directXCommon_->GetCommandList();

        // RootSignature
        commandList->SetGraphicsRootSignature(
            pipeline->rootSignature->GetRootSignature()
        );

        // Pipeline State
        commandList->SetPipelineState(
            pipeline->pipeline->GetPipelineState()
        );

        // Primitive Topology
        commandList->IASetPrimitiveTopology(
            D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST
        );
    }

    PipelineConfig &TunaEngine::GetPipelineConfig(
        PipelineMode mode
    ) {
        return pipelineManager_->GetConfig(mode);
    }


    void TunaEngine::RebuildPipeline(
        PipelineMode mode
    ) {
        pipelineManager_->RebuildPipeline(mode);
    }
}