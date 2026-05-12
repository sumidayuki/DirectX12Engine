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
    uint hasTexture; // テクスチャの有無 (0: なし, 1: あり)
    uint2 padding;
};

ConstantBuffer<OrthoBuffer> cOrtho : register(b0, space0);
ConstantBuffer<ObjectBuffer> cObject : register(b1, space0);

// Bindless: テクスチャ配列（インデックスでアクセス）
// Bindless textures array
Texture2D _Textures[4096] : register(t0, space1);

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
    float4 finalColor;

    if (cObject.hasTexture)
    {
        // テクスチャあり: Bindlessインデックスでテクスチャ選択
        float4 texColor = _Textures[cObject.textureIndex].Sample(_Sampler, input.uv);
        finalColor = texColor * cObject.color;
    }
    else
    {
        // テクスチャなし: 単色矩形
        finalColor = cObject.color;
    }

    if (finalColor.a < 0.001f)
        discard;

    PSOutput output;
    output.target0 = finalColor;
    return output;
}