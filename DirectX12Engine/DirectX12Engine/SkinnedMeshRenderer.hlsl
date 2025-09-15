// Light構造体
struct Light
{
    float4 color;
    float3 position;
    float range;
    float3 direction;
    float spotAngle;
    uint type; // LightType
    int enabled;
    float2 padding;
};

struct SkinnedObjectConstantsLayout
{
    float4x4 worldMatrix;
    float4 diffuseColor;
    float4 specularColor;
    float shininess;
    float3 padding;
    float4x4 boneMatrices[256];
};

// カメラ情報
cbuffer CameraConstants : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

// スキンメッシュオブジェクトごとの情報
cbuffer ObjectConstants : register(b1)
{
    SkinnedObjectConstantsLayout objectData;
};

// シーン全体の追加情報
cbuffer SceneConstants : register(b2)
{
    int activeLightCount;
    float3 pad1;
    float4 cameraWorldPosition;
};

// ライト情報
StructuredBuffer<Light> lights : register(t3);

// テクスチャとサンプラー
Texture2D diffuseMap : register(t0);
SamplerState linearSampler : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
    int4 boneIndices : BONEINDICES;
    float4 boneWeights : BONEWEIGHTS;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 worldPos : TEXCOORD1;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;
    
    matrix skinningTransform = (matrix) 0;
    
    // 各ボーンの影響を計算
    skinningTransform = objectData.boneMatrices[input.boneIndices[0]] * input.boneWeights[0];
    skinningTransform += objectData.boneMatrices[input.boneIndices[1]] * input.boneWeights[1];
    skinningTransform += objectData.boneMatrices[input.boneIndices[2]] * input.boneWeights[2];
    skinningTransform += objectData.boneMatrices[input.boneIndices[3]] * input.boneWeights[3];
    
    // スキニング変換を適用
    float4 skinnedPosition = mul(float4(input.position, 1.0f), skinningTransform);
    float4 worldPos = mul(skinnedPosition, objectData.worldMatrix);
    
    output.position = mul(worldPos, viewMatrix);
    output.position = mul(output.position, projMatrix);

    // 法線とテクスチャ座標
    float4 skinnedNormal = mul(float4(input.normal, 0.0f), skinningTransform);
    output.normal = normalize(mul(skinnedNormal, (float3x3) objectData.worldMatrix));
    output.worldPos = worldPos.xyz;
    output.uv = input.uv;
    
    return output;
}

void CalculateLight(Light light, SkinnedObjectConstantsLayout obj, float3 pixelPos, float3 normal, float3 viewDir,
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
    outDiffuse = diff * light.color.rgb * obj.diffuseColor.rgb;

    // Specular (Blinn-Phong)
    float3 halfVec = normalize(lightDir + viewDir);
    float spec = pow(saturate(dot(normal, halfVec)), obj.shininess);
    outSpecular = spec * light.color.rgb * obj.specularColor.rgb;

    outDiffuse *= attenuation;
    outSpecular *= attenuation;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 textureColor = diffuseMap.Sample(linearSampler, input.uv);
    input.normal = normalize(input.normal);
    float3 viewDir = normalize(cameraWorldPosition.xyz - input.worldPos);

    float3 totalDiffuse = float3(0, 0, 0);
    float3 totalSpecular = float3(0, 0, 0);

    for (int i = 0; i < activeLightCount; ++i)
    {
        float3 currentDiffuse, currentSpecular;
        CalculateLight(lights[i], objectData, input.worldPos, input.normal, viewDir, currentDiffuse, currentSpecular);
        totalDiffuse += currentDiffuse;
        totalSpecular += currentSpecular;
    }

    float3 ambient = float3(0.1, 0.1, 0.1) * objectData.diffuseColor.rgb;
    float3 finalColor = (ambient + totalDiffuse) * textureColor.rgb + totalSpecular;

    return float4(finalColor, textureColor.a * objectData.diffuseColor.a);
}