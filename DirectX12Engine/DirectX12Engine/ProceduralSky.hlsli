#ifndef PROCEDURALSKY_HLSLI
#define PROCEDURALSKY_HLSLI

#include "TypeMarshalling.hlsli"

struct ProceduralSkyConstants
{
    float timeOfDay;
    float time;
    int enableClouds;
    int enableStars;
    int enableMoon;
    int enableSun;
    int enableSunGlow;
};

struct LightConstants
{
    uint lightCount;
};

#endif