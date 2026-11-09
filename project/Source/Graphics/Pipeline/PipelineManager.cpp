#include "PipelineManager.h"

#include "PipelineLibrary.h"

namespace Tuna {

    PipelineManager::PipelineManager() = default;
    PipelineManager::~PipelineManager() = default;


    void PipelineManager::Initialize(
        ID3D12Device *device
    ) {
        pipelineLibrary_ =
            std::make_unique<PipelineLibrary>();

        pipelineLibrary_->Initialize(
            device
        );
    }


    const PipelineState *PipelineManager::GetPipeline(PipelineMode mode) const {
        return pipelineLibrary_->GetPipeline(mode);
    }


    PipelineConfig &PipelineManager::GetConfig(PipelineMode mode) {
        return pipelineLibrary_->GetConfig(mode);
    }


    void PipelineManager::RebuildPipeline(PipelineMode mode) {
        pipelineLibrary_->RebuildPipeline(mode);
    }
}