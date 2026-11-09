#pragma once

#pragma comment(lib, "d3d12.lib")

#include <d3d12.h>
#include <unordered_map>
#include <memory>

#include "RootSignature.h"
#include "GraphicsPipeline.h"
#include "PipelineConfig.h"

namespace Tuna {

	class RootSignature;
	class GraphicsPipeline;

	enum class PipelineMode {
		Object3DOpaque,
		Object3DAlpha,
		Object3DWireframe,
		Object3DThroughWall,

		Object2DOpaque,

		Model3DOpaque,

		Particle,
	};


	struct PipelineState {
		std::unique_ptr<RootSignature> rootSignature;
		std::unique_ptr<GraphicsPipeline> pipeline;
	};


	class PipelineLibrary {
	public:

		PipelineLibrary() = default;

		~PipelineLibrary() = default;

		PipelineLibrary(const PipelineLibrary &) = delete;

		PipelineLibrary &operator=(const PipelineLibrary &) = delete;


		void Initialize(
			ID3D12Device *device
		);


		const PipelineState *GetPipeline(
			PipelineMode mode
		) const;


		// Pipelineの設定を取得
		PipelineConfig &GetConfig(
			PipelineMode mode
		);


		// Pipelineを再生成
		void RebuildPipeline(
			PipelineMode mode
		);


	private:

		void CreateObject3DOpaque(
			ID3D12Device *device
		);

		void CreateObject3DAlpha(
			ID3D12Device *device
		);

		void CreateObject3DWireframe(
			ID3D12Device *device
		);

		void CreateObject3DThroughWall(
			ID3D12Device *device
		);

		void CreateObject2DOpaque(
			ID3D12Device *device
		);

		void CreateModel3DOpaque(
			ID3D12Device *device
		);

		void CreateParticle(
			ID3D12Device *device
		);

	private:

		// PipelineModeをキーとしてPipelineを管理
		std::unordered_map<PipelineMode, PipelineState> pipelines_;

		// PipelineModeをキーとして設定を管理
		std::unordered_map<PipelineMode, PipelineConfig> configs_;

		// 再生成に使用する
		ID3D12Device *device_ = nullptr;
	};

}