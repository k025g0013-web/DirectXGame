#pragma once

#include "Math/Matrix.h"

namespace Tuna {
	/// <summary>
	/// シェーダーへ渡す変換行列を保持する構造体
	/// </summary>
	struct TransformationMatrix {

		/// ワールド・ビュー・プロジェクション行列
		Matrix4x4 WVP;

		/// ワールド行列
		Matrix4x4 World;

		// 色と透明度
		Vector4 color;

		Matrix4x4 WorldInverseTranspose;
	};
}