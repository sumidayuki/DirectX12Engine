#ifndef PROCEDURALSKY_HLSLI
#define PROCEDURALSKY_HLSLI

#include "TypeMarshalling.hlsli"

struct ProceduralSkyConstants
{
    float timeOfDay;
    float time;
    uint enableClouds;
    uint enableStars;
    uint enableMoon;
    uint enableSun;
    uint enableSunGlow;
};

struct LightConstants
{
    uint lightCount;
};

#endif