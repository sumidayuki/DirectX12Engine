#include "TypeMarshalling.hlsli"

struct LightLayout
{
    float4 color;
    float3 position;
    float range;
    float3 direction;
    float spotAngle;
    uint type;
};