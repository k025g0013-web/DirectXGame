#include "object3D.hlsli"

struct TransformationMatrix
{
    float32_t4x4 WVP;
    float32_t4x4 World;
    float32_t4 color;
    float32_t4x4 WorldInverseTranspose;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;

    // クリップ座標
    output.position = mul(input.position, gTransformationMatrix.WVP);

    // UV
    output.texcoord = input.texcoord;

    // 法線
    output.normal = normalize(mul(input.normal, (float32_t3x3) gTransformationMatrix.WorldInverseTranspose));

    // ワールド座標
    output.worldPosition = mul(input.position, gTransformationMatrix.World).xyz;

    return output;
}