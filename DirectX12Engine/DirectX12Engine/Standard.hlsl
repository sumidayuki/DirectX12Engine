#include "Camera.hlsli"
#include "Standard.hlsli"

struct MaterialLayout
{
    float4 _BaseColor;
    float4 _SpecularColor;
    float _Shininess;
};

ConstantBuffer<CameraLayout>        bCamera                 : register(b0, space0);
#ifdef SKINNED
ConstantBuffer<SkinnedObjectLayout> bObject                 : register(b1, space0);
#else
ConstantBuffer<ObjectLayout>        bObject                 : register(b1, space0);
#endif
ConstantBuffer<LightConstants>      bLightConstants         : register(b2, space0);
ConstantBuffer<MaterialLayout>      bMaterialConstants      : register(b3, space0);

StructuredBuffer<LightLayout>       lights                  : register(t0);
Texture2D                           _MainTex                : register(t1);
Texture2D                           _NormalTex              : register(t2);

SamplerState linearSampler : register(s0);

// VS Input
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD0;
    float3 tangent : TANGENT;
#ifdef SKINNED
    int4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
#endif
};

// PS Input
struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

// Helper: Calculate Light
void CalculateLight(LightLayout light, MaterialLayout mat, float3 pixelPos, float3 normal, float3 viewDir,
                    out float3 outDiffuse, out float3 outSpecular)
{
    outDiffuse = float3(0, 0, 0);
    outSpecular = float3(0, 0, 0);

    float3 lightDir;
    float attenuation = 1.0;

    switch (light.type)
    {
        case 0: // Directional
        {
                lightDir = normalize(-light.direction);
                break;
            }
        case 1: // Point
        {
                float3 toLightVec = light.position - pixelPos;
                float dist = length(toLightVec);
                if (dist > light.range)
                    return;
                lightDir = normalize(toLightVec);
                float distFactor = 1.0 - smoothstep(light.range * 0.75, light.range, dist);
                attenuation = distFactor * distFactor;
                break;
            }
        case 2: // Spot
        {
                float3 toLightVec = light.position - pixelPos;
                float dist = length(toLightVec);
                if (dist > light.range)
                    return;
                lightDir = normalize(toLightVec);
                float distFactor = 1.0 - smoothstep(light.range * 0.75, light.range, dist);
                attenuation = distFactor * distFactor;
                float spotFactor = dot(-lightDir, normalize(light.direction));
                float spotAttenuation = smoothstep(light.spotAngle, light.spotAngle + 0.05, spotFactor);
                attenuation *= spotAttenuation;
                break;
            }
    }

    if (attenuation <= 0.001)
        return;

    // Diffuse
    float diff = saturate(dot(normal, lightDir));
    outDiffuse = diff * light.color.rgb * mat._BaseColor.rgb;

    // Specular
    float3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(saturate(dot(normal, halfVec)), mat._Shininess);
    outSpecular = spec * light.color.rgb * mat._SpecularColor.rgb;

    outDiffuse *= attenuation;
    outSpecular *= attenuation;
}

// Vertex Shader
PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float4x4 world = bObject.world;
    float4 localPos = float4(input.position, 1.0f);
    float3 localNormal = input.normal;

#ifdef SKINNED
    matrix skinningTransform = (matrix)0;
    skinningTransform += bObject.boneMatrices[input.boneIndices[0]] * input.boneWeights[0];
    skinningTransform += bObject.boneMatrices[input.boneIndices[1]] * input.boneWeights[1];
    skinningTransform += bObject.boneMatrices[input.boneIndices[2]] * input.boneWeights[2];
    skinningTransform += bObject.boneMatrices[input.boneIndices[3]] * input.boneWeights[3];
    
    localPos = mul(localPos, skinningTransform);
    // Transform normal by skinning matrix (rotation part)
    localNormal = mul(float4(localNormal, 0.0f), skinningTransform).xyz;
#endif

    float4 worldPos = mul(localPos, bObject.world);
    output.position = mul(worldPos, bCamera.view);
    output.position = mul(output.position, bCamera.proj);

    output.worldPos = worldPos.xyz;
    output.normal = normalize(mul(float4(input.normal, 0.0f), bObject.world).xyz);
    output.uv = input.uv;

    return output;
}

// Pixel Shader
float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = _MainTex.Sample(linearSampler, input.uv);
    input.normal = normalize(input.normal);
    float3 viewDir = normalize(bCamera.position.xyz - input.worldPos);

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);

    MaterialLayout mat;
    mat._BaseColor = bMaterialConstants._BaseColor;
    mat._SpecularColor = bMaterialConstants._SpecularColor;
    mat._Shininess = bMaterialConstants._Shininess;
    
    for (int i = 0; i < bLightConstants.lightCount; ++i)
    {
        float3 currentDiffuse, currentSpecular;
        CalculateLight(lights[i], mat, input.worldPos, input.normal, viewDir, currentDiffuse, currentSpecular);
        totalDiffuse += currentDiffuse;
        totalSpecular += currentSpecular;
    }

    float3 ambient = float3(0.02, 0.02, 0.02) * mat._BaseColor.rgb;
    float3 finalColor = (ambient + totalDiffuse) * textureColor.rgb + totalSpecular;

    return float4(finalColor, textureColor.a * mat._BaseColor.a);
}
