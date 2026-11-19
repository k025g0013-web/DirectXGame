#include "object3D.hlsli"
    
struct Material
{
    float32_t4 color;
    int32_t enableLighting;
    float32_t4x4 uvTransform;
};

struct PhongReflectionModel
{
    float32_t4 ambientColor;
    float32_t4 specularColor;

    float32_t ambientIntensity;
    float32_t specularIntensity;
    float32_t shininess;
    float32_t padding;

    float32_t3 cameraPosition;
    float32_t padding2;
};

struct DirectionalLight
{
    float32_t4 color;

    float32_t3 direction;
    float32_t intensity;

    int32_t lightMode;
    float32_t3 padding;

    PhongReflectionModel phong;
};

ConstantBuffer<Material> gMaterial : register(b0);
Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);
ConstantBuffer<DirectionalLight> gDirectionalLight : register(b1);

struct PixelShaderOutput
{
    float32_t4 color : SV_TARGET0;
};

float32_t3 CalculatePhongReflection(
    float32_t3 baseColor, float32_t3 normal, float32_t3 worldPosition)
{
    
    float32_t3 N = normalize(normal);
    float32_t3 L = normalize(-gDirectionalLight.direction);
    float32_t3 V = normalize(gDirectionalLight.phong.cameraPosition - worldPosition);
    
    // 環境光
    float32_t3 ambient = baseColor * gDirectionalLight.phong.ambientColor.rgb * gDirectionalLight.phong.ambientIntensity;

    // 拡散反射
    float32_t NdotL = saturate(dot(N, L));
    float32_t3 diffuse = baseColor * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

    // 鏡面反射
    float32_t3 R = reflect(-L, N);
    float32_t RdotV = saturate(dot(R, V));
    float32_t specularFactor = 0.0f;

    if (NdotL > 0.0f)
    {
        specularFactor = pow(RdotV, gDirectionalLight.phong.shininess);
    }

    float32_t3 specular =
        gDirectionalLight.phong.specularColor.rgb * gDirectionalLight.color.rgb * specularFactor *
        gDirectionalLight.phong.specularIntensity * gDirectionalLight.intensity;
    
    return ambient + diffuse + specular;
}

//==================================================
// Blinn-Phong反射モデル
//==================================================
float32_t3 CalculateBlinnPhongReflection(
    float32_t3 baseColor, float32_t3 normal, float32_t3 worldPosition)
{
    
    float32_t3 N = normalize(normal);
    float32_t3 L = normalize(-gDirectionalLight.direction);
    float32_t3 V = normalize(gDirectionalLight.phong.cameraPosition - worldPosition);

    // 環境光
    float32_t3 ambient = baseColor * gDirectionalLight.phong.ambientColor.rgb * gDirectionalLight.phong.ambientIntensity;

    // 拡散反射
    float32_t NdotL = saturate(dot(N, L));
    float32_t3 diffuse = baseColor * gDirectionalLight.color.rgb * NdotL * gDirectionalLight.intensity;

    // 鏡面反射
    float32_t3 H = normalize(L + V);
    float32_t NdotH = saturate(dot(N, H));
    float32_t specularFactor = 0.0f;

    if (NdotL > 0.0f)
    {
        specularFactor = pow(NdotH, gDirectionalLight.phong.shininess);
    }

    float32_t3 specular =
        gDirectionalLight.phong.specularColor.rgb *
        gDirectionalLight.color.rgb *
        specularFactor *
        gDirectionalLight.phong.specularIntensity *
        gDirectionalLight.intensity;
    
    return ambient + diffuse + specular;
}

PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output;

    // UV変換
    float32_t4 transformedUV = mul(
        float32_t4(input.texcoord, 0.0f, 1.0f),
        gMaterial.uvTransform
    );

    // テクスチャ取得
    float32_t4 textureColor =
        gTexture.Sample(gSampler, transformedUV.xy);
    
    //========================================
    // 2値抜き
    //========================================

    if (textureColor.a < 0.5f)
    {
        discard;
    }

    float32_t4 baseColor = gMaterial.color * textureColor;
    
    if (gMaterial.enableLighting == 0 ||
        gDirectionalLight.lightMode == 0)
    {
        output.color = baseColor;
    }
    else
    {
        float32_t3 N = normalize(input.normal);
        float32_t3 L =
            normalize(-gDirectionalLight.direction);

        float32_t NdotL = dot(N, L);

        float32_t diffuseFactor = 1.0f;

         // Lambert
        if (gDirectionalLight.lightMode == 1)
        {
            diffuseFactor = saturate(NdotL);
        }
        
        // Half-Lambert
        else if (gDirectionalLight.lightMode == 2)
        {
            diffuseFactor = pow(NdotL * 0.5f + 0.5f, 2.0f);
        }

        // Phong
        else if (gDirectionalLight.lightMode == 3)
        {
            output.color.rgb = CalculatePhongReflection(
                baseColor.rgb, input.normal, input.worldPosition);

            output.color.a = baseColor.a;

            return output;
        }
        
        // BlinnPhong
        else if (gDirectionalLight.lightMode == 4)
        {
            output.color.rgb = CalculateBlinnPhongReflection(
                baseColor.rgb, input.normal, input.worldPosition);

            output.color.a = baseColor.a;

            return output;
        }

        output.color.rgb =
            baseColor.rgb *
            gDirectionalLight.color.rgb *
            diffuseFactor *
            gDirectionalLight.intensity;

        output.color.a = baseColor.a;
    }

    return output;
}