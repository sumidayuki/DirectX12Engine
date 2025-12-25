#ifndef MATERIAL_HLSLI
#define MATERIAL_HLSLI

#define BIT_HAS_NORMAL_MAP          (1 << 0) 
#define BIT_HAS_METALLIC_ROUGHNESS  (1 << 1) 
#define BIT_IS_ALPHA_TESTED         (1 << 2)

#include "TypeMarshalling.hlsli"

struct MaterialLayout
{
    float4  baseColor;
    float   roughness;
    float   metalic;
    float4  emmisiveColor;
    float   alphaCutoff;
    uint    shaderFlags;
    float   padding[2];
};

#endif