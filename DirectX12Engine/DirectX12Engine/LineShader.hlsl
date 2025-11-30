
cbuffer CameraConstant : register(b0)
{
    float4x4 viewMatrix;
    float4x4 projectionMatrix;
}

struct VSInput
{
    float3 mpos : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 ppos : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;
    float4 viewPos = mul(float4(input.mpos, 1.0f), viewMatrix);
    output.ppos = mul(viewPos, projectionMatrix);
    output.color = input.color;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}