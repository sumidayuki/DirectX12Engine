#ifndef LIGHT_HLSLI
#define LIGHT_HLSLI

#include "TypeMarshalling.hlsli"

struct LightLayout
{
    float4 color;
    float3 position;
    float range;
    float3 direction;
    float spotAngle;
    uint type; // LightType
    uint padding[3];
};

#endif