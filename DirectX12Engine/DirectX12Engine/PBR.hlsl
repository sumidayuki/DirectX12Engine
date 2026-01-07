#include "Camera.hlsli"
#include "Standard.hlsli"

// PBR用マテリアル定義
struct MaterialLayout
{
    float4 _BaseColor;
    float _Metallic;
    float _Roughness;
    float _Occlusion;
};

ConstantBuffer<CameraLayout> bCamera : register(b0, space0);
#ifdef SKINNED
ConstantBuffer<SkinnedObjectLayout> bObject  : register(b1, space0);
#else
ConstantBuffer<ObjectLayout> bObject : register(b1, space0);
#endif
ConstantBuffer<LightConstants> bLightConstants : register(b2, space0);
ConstantBuffer<MaterialLayout> bMaterialConstants : register(b3, space0);

// リソース
StructuredBuffer<LightLayout> lights : register(t0);
Texture2D _MainTex : register(t1);
Texture2D _NormalTex : register(t2);
Texture2D _MetallicRoughnessTex : register(t3);
Texture2D _AOTex : register(t4);
Texture2D _EnvMap : register(t5);
SamplerState linearSampler : register(s0);

static const float PI = 3.14159265359;

// --- PBR Functions ---

float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness * roughness;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return nom / max(PI * denom * denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    return GeometrySchlickGGX(NdotV, roughness) * GeometrySchlickGGX(NdotL, roughness);
}

float3 FresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float2 SampleEquirectangular(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= float2(0.1591, 0.3183);
    uv += 0.5;
    return uv;
}

// --- Shader Logic ---

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

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TEXCOORD2;
    float3 worldPos : TEXCOORD1;
};

PS_INPUT VSMain(VS_INPUT input)
{
    PS_INPUT output = (PS_INPUT) 0;

    float3 localPos = input.position;
    float3 localNormal = input.normal;
    float3 localTangent = input.tangent;

#ifdef SKINNED
    matrix skinningTransform = (matrix)0;
    skinningTransform += bObject.boneMatrices[input.boneIndices[0]] * input.boneWeights[0];
    skinningTransform += bObject.boneMatrices[input.boneIndices[1]] * input.boneWeights[1];
    skinningTransform += bObject.boneMatrices[input.boneIndices[2]] * input.boneWeights[2];
    skinningTransform += bObject.boneMatrices[input.boneIndices[3]] * input.boneWeights[3];
    
    localPos = mul(float4(localPos, 1.0f), skinningTransform).xyz;
    localNormal = mul(float4(localNormal, 0.0f), skinningTransform).xyz;
    localTangent = mul(float4(localTangent, 0.0f), skinningTransform).xyz;
#endif

    float4 worldPos = mul(float4(localPos, 1.0f), bObject.world);
    output.worldPos = worldPos.xyz;
    output.position = mul(worldPos, bCamera.view);
    output.position = mul(output.position, bCamera.proj);

    output.normal = normalize(mul(float4(localNormal, 0.0f), bObject.world).xyz);
    output.tangent = normalize(mul(float4(localTangent, 0.0f), bObject.world).xyz);
    output.uv = input.uv;

    return output;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    // 1. テクスチャサンプリング
    // テクスチャ未設定時に white.png (1,1,1,1) が来る前提
    float4 texColor = _MainTex.Sample(linearSampler, input.uv);
    float4 albedo = texColor * bMaterialConstants._BaseColor;
    
    float3 mrSample = _MetallicRoughnessTex.Sample(linearSampler, input.uv).rgb;
    // Unity流：テクスチャの各チャンネルに係数をかける
    float metallic = mrSample.b * bMaterialConstants._Metallic;
    float roughness = max(mrSample.g * bMaterialConstants._Roughness, 0.05);
    
    float ao = _AOTex.Sample(linearSampler, input.uv).r * bMaterialConstants._Occlusion;

    // 2. 法線マッピング
    float3 N = normalize(input.normal);
    // 接線が存在し、かつ法線マップが「白(1,1,1)」でない場合にのみ適用
    // C++側でデフォルトを「薄水色(0.5, 0.5, 1.0)」にしている場合はこのチェックなしでも正常動作する
    float3 normalSample = _NormalTex.Sample(linearSampler, input.uv).rgb;
    
    if (length(input.tangent) > 0.01)
    {
        float3 normalMap = normalSample * 2.0 - 1.0;
        float3 T = normalize(input.tangent);
        float3 B = normalize(cross(N, T));
        float3x3 TBN = float3x3(T, B, N);
        N = normalize(mul(normalMap, TBN));
    }
    
    float3 V = normalize(bCamera.position.xyz - input.worldPos);
    float3 R = reflect(-V, N);
    
    // 3. 反射率 F0
    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo.rgb, metallic);

    float3 Lo = float3(0, 0, 0);

    // 4. ダイレクトライティング
    for (int i = 0; i < (int) bLightConstants.lightCount; ++i)
    {
        LightLayout light = lights[i];
        float3 L;
        float attenuation = 1.0;

        if (light.type == 0) // Directional
        {
            L = normalize(-light.direction);
        }
        else // Point/Spot
        {
            float3 toLight = light.position - input.worldPos;
            float dist = length(toLight);
            if (dist > light.range)
                continue;

            L = normalize(toLight);
            float distFactor = 1.0 - pow(dist / light.range, 4.0);
            attenuation = max(distFactor, 0.0) / (dist + 1.0);

            if (light.type == 2) // Spot
            {
                float theta = dot(L, normalize(-light.direction));
                attenuation *= smoothstep(light.spotAngle, light.spotAngle + 0.1, theta);
            }
        }

        float3 H = normalize(V + L);
        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 kS = F;
        float3 kD = (1.0 - kS) * (1.0 - metallic);

        float3 numerator = D * G * F;
        float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
        float3 specular = numerator / denominator;

        float NdotL = max(dot(N, L), 0.0);
        Lo += (kD * albedo.rgb / PI + specular) * light.color.rgb * attenuation * NdotL;
    }

    // 5. 環境マップ (IBL)
    float2 envUV = SampleEquirectangular(R);
    float3 envReflection = _EnvMap.SampleLevel(linearSampler, envUV, roughness * 8.0).rgb;
    float3 envDiffuse = albedo.rgb * 0.03 * ao;
    float3 F_ibl = FresnelSchlick(max(dot(N, V), 0.0), F0);
    
    float3 color = envDiffuse + (F_ibl * envReflection) + Lo;
    
    // 6. HDR トーンマッピング & ガンマ補正
    color = color / (color + float3(1.0, 1.0, 1.0));
    color = pow(color, float3(1.0 / 2.2, 1.0 / 2.2, 1.0 / 2.2));

    return float4(color, albedo.a);
}