struct VSInput
{
    float2 mpos : POSITION;
    float2 uv : TEXCOORD0;
};

struct VSOutput
{
    float4 ppos : SV_POSITION;
    float2 uv : TEXCOORD0;
};

struct OrthoBuffer
{
    matrix orthoMatrix;
};

struct ObjectBuffer
{
    matrix worldMatrix;
    float4 color;
    float2 minUV;
    float2 maxUV;
    uint textureIndex;
    uint3 padding;
};

ConstantBuffer<OrthoBuffer> cOrtho : register(b0, space0);
ConstantBuffer<ObjectBuffer> cObject : register(b1, space0);

// Bindless: テクスチャ配列（インデックスでアクセス）
Texture2D _Textures[4096] : register(t0, space0);

SamplerState _Sampler : register(s0, space0);

VSOutput VSMain(VSInput input)
{
    float4 worldPos = mul(float4(input.mpos, 0, 1), cObject.worldMatrix);
    float4 clipPos = mul(worldPos, cOrtho.orthoMatrix);

    float2 uv;
    uv.x = lerp(cObject.minUV.x, cObject.maxUV.x, input.uv.x);
    uv.y = lerp(cObject.minUV.y, cObject.maxUV.y, input.uv.y);
    uv.y = 1.0f - uv.y;

    VSOutput output;
    output.ppos = clipPos;
    output.uv = uv;
    return output;
}

struct PSOutput
{
    float4 target0 : SV_TARGET0;
};

PSOutput PSMain(VSOutput input)
{
    // Bindless: インデックスでテクスチャを選択
    float4 texColor = _Textures[cObject.textureIndex].Sample(_Sampler, input.uv);
    float4 finalColor = texColor * cObject.color;

    if (finalColor.a < 0.001f)
        discard;

    PSOutput output;
    output.target0 = finalColor;
    return output;
}