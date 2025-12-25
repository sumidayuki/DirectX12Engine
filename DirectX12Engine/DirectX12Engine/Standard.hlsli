#ifndef STANDARD_HLSLI
#define STANDARD_HLSLI

#include "TypeMarshalling.hlsli"
#include "light.hlsli"

struct ObjectLayout
{
    matrix world;
};

struct SkinnedObjectLayout
{
    matrix world;
    matrix boneMatrices[256];
};

struct LightConstants
{
    uint lightCount;
};

#endif