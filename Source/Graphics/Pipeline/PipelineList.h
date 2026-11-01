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

	enum class PipelineType {
		Object3dOpaque,
		Object3dAlpha,
		Object3dWireframe,
		Object3dThroughWall,

		Object2dOpaque,

		Model3dOpaque,

		Particle,
	};


	struct PipelineSet {
		std::unique_ptr<RootSignature> rootSignature;
		std::unique_ptr<GraphicsPipeline> pipeline;
	};


	class PipelineList {
	public:

		PipelineList() = default;

		~PipelineList() = default;

		PipelineList(const PipelineList &) = delete;

		PipelineList &operator=(const PipelineList &) = delete;


		void Initialize(
			ID3D12Device *device
		);


		const PipelineSet *GetPipeline(
			PipelineType type
		) const;


		// Pipelineの設定を取得
		PipelineConfig &GetConfig(
			PipelineType type
		);


		// Pipelineを再生成
		void RebuildPipeline(
			PipelineType type
		);


	private:

		void CreateObject3dOpaque(
			ID3D12Device *device
		);

		void CreateObject3dAlpha(
			ID3D12Device *device
		);

		void CreateObject3dWireframe(
			ID3D12Device *device
		);

		void CreateObject3dThroughWall(
			ID3D12Device *device
		);

		void CreateObject2dOpaque(
			ID3D12Device *device
		);

		void CreateModel3dOpaque(
			ID3D12Device *device
		);

		void CreateParticle(
			ID3D12Device *device
		);

	private:

		// PipelineTypeをキーとしてPipelineを管理
		std::unordered_map<PipelineType, PipelineSet> pipelines_;

		// PipelineTypeをキーとして設定を管理
		std::unordered_map<PipelineType, PipelineConfig> configs_;

		// 再生成に使用する
		ID3D12Device *device_ = nullptr;
	};

}