#include "Camera.hlsli"
#include "Standard.hlsli"

struct MaterialLayout
{
    float4 _BaseColor;
    float _Metallic;
    float _Roughness;
    float _Occlusion;
};

ConstantBuffer<CameraLayout> bCamera : register(b0, space0);
#ifdef SKINNED
ConstantBuffer<SkinnedObjectLayout> bObject : register(b1, space0);
#else
ConstantBuffer<ObjectLayout> bObject : register(b1, space0);
#endif

ConstantBuffer<LightConstants> bLightConstants : register(b2, space0);
ConstantBuffer<MaterialLayout> bMaterialConstants : register(b3, space0);

StructuredBuffer<LightLayout> lights : register(t0);
Texture2D _MainTex : register(t1);
Texture2D _NormalTex : register(t2);
Texture2D _MetallicRoughnessTex : register(t3);
Texture2D _AOTex : register(t4);
Texture2D _EnvMap : register(t5);
SamplerState linearSampler : register(s0);

static const float PI = 3.14159265359;

float DistributionGGX(float3 N, float3 H, float r)
{
    float a = r * r;
    float a2 = a * a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH * NdotH;
    float nom = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    return nom / max(PI * denom * denom, 0.0000001);
}

float GeometrySchlickGGX(float NdotV, float r)
{
    float k = ((r + 1.0) * (r + 1.0)) / 8.0;
    return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float r)
{
    return GeometrySchlickGGX(max(dot(N, V), 0.0), r) *
           GeometrySchlickGGX(max(dot(N, L), 0.0), r);
}

float3 FresnelSchlick(float c, float3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - c, 0.0, 1.0), 5.0);
}

float2 SampleEquirectangular(float3 v)
{
    float2 uv = float2(atan2(v.z, v.x), asin(v.y));
    uv *= float2(0.1591, 0.3183);
    uv += 0.5;
    return uv;
}

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
    PS_INPUT o = (PS_INPUT) 0;

    float3 pos = input.position;
    float3 nrm = input.normal;
    float3 tan = input.tangent;

#ifdef SKINNED
    matrix skin = (matrix)0;
    skin += bObject.boneMatrices[input.boneIndices[0]] * input.boneWeights[0];
    skin += bObject.boneMatrices[input.boneIndices[1]] * input.boneWeights[1];
    skin += bObject.boneMatrices[input.boneIndices[2]] * input.boneWeights[2];
    skin += bObject.boneMatrices[input.boneIndices[3]] * input.boneWeights[3];

    pos = mul(float4(pos, 1), skin).xyz;
    nrm = mul(float4(nrm, 0), skin).xyz;
    tan = mul(float4(tan, 0), skin).xyz;
#endif

    float4 worldPos = mul(float4(pos, 1), bObject.world);
    o.worldPos = worldPos.xyz;
    o.position = mul(worldPos, bCamera.view);
    o.position = mul(o.position, bCamera.proj);

    o.normal = normalize(mul(float4(nrm, 0), bObject.world).xyz);
    o.tangent = normalize(mul(float4(tan, 0), bObject.world).xyz);
    o.uv = input.uv;

    return o;
}

float4 PSMain(PS_INPUT input) : SV_TARGET
{
    float4 texColor = _MainTex.Sample(linearSampler, input.uv);
    float4 albedo = texColor * bMaterialConstants._BaseColor;

    float3 mr = _MetallicRoughnessTex.Sample(linearSampler, input.uv).rgb;
    float metallic = mr.b * bMaterialConstants._Metallic;
    float roughness = saturate(mr.g * bMaterialConstants._Roughness);

    float ao = _AOTex.Sample(linearSampler, input.uv).r * bMaterialConstants._Occlusion;

    float3 N = normalize(input.normal);

    float3 normalSample = _NormalTex.Sample(linearSampler, input.uv).rgb;

    if (abs(normalSample.r - 0.5) > 0.001 ||
        abs(normalSample.g - 0.5) > 0.001 ||
        abs(normalSample.b - 1.0) > 0.001)
    {
        float3 T = normalize(input.tangent);

        T = normalize(T - N * dot(N, T));

        float3 B = normalize(cross(N, T));

        float3x3 TBN = float3x3(T, B, N);

        float3 nm = normalSample * 2.0 - 1.0;
        N = normalize(mul(nm, TBN));
    }

    float3 V = normalize(bCamera.position.xyz - input.worldPos);
    float3 R = reflect(-V, N);

    float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo.rgb, metallic);
    float3 Lo = 0;

    for (int i = 0; i < (int) bLightConstants.lightCount; ++i)
    {
        LightLayout light = lights[i];
        float3 L;
        float attenuation = 1.0;

        if (light.type == 0)
        {
            L = normalize(-light.direction);
        }
        else
        {
            float3 toL = light.position - input.worldPos;
            float dist = length(toL);
            if (dist > light.range)
                continue;

            L = normalize(toL);
            float fall = 1.0 - pow(dist / light.range, 4.0);
            attenuation = max(fall, 0.0) / (dist + 1.0);

            if (light.type == 2)
            {
                float theta = dot(L, normalize(-light.direction));
                attenuation *= smoothstep(light.spotAngle,
                                          light.spotAngle + 0.1,
                                          theta);
            }
        }

        float3 H = normalize(V + L);

        float D = DistributionGGX(N, H, roughness);
        float G = GeometrySmith(N, V, L, roughness);
        float3 F = FresnelSchlick(max(dot(H, V), 0.0), F0);

        float3 kS = F;
        float3 kD = (1.0 - kS) * (1.0 - metallic);

        float3 numerator = D * G * F;
        float denom = 4.0 * max(dot(N, V), 0.0) *
                      max(dot(N, L), 0.0) + 0.0001;

        float3 spec = numerator / denom;

        Lo += (kD * albedo.rgb / PI + spec) *
              light.color.rgb * attenuation *
              max(dot(N, L), 0.0);
    }

    float2 envUV = SampleEquirectangular(R);
    float3 envReflection =
        _EnvMap.SampleLevel(linearSampler, envUV, roughness * 8.0).rgb;

    float3 envDiffuse = albedo.rgb * ao * 0.3;

    float3 Fibl = FresnelSchlick(max(dot(N, V), 0.0), F0);

    float3 color = envDiffuse + Fibl * envReflection + Lo;

    color = color / (color + 1.0);
    color = pow(color, 1.0 / 2.2);

    return float4(color, albedo.a);
}
