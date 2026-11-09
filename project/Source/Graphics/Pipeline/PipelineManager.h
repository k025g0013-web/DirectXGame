#pragma once
#include "PipelineLibrary.h"

#pragma comment(lib, "d3d12.lib")
#include <d3d12.h>

namespace Tuna {
    class PipelineManager {
    public:
        PipelineManager();
        ~PipelineManager();

        PipelineManager(const PipelineManager &) = delete;
        PipelineManager &operator=(const PipelineManager &) = delete;

        void Initialize(
            ID3D12Device *device
        );

        const PipelineState *GetPipeline(
            PipelineMode mode
        ) const;

        PipelineConfig &GetConfig(
            PipelineMode mode
        );

        void RebuildPipeline(
            PipelineMode mode
        );

    private:
        std::unique_ptr<PipelineLibrary> pipelineLibrary_;
    };

}