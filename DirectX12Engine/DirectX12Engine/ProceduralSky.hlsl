#include "Camera.hlsli"
#include "Light.hlsli"
#include "ProceduralSky.hlsli"

#define PI 3.14159265359

ConstantBuffer<CameraLayout> bCamera : register(b0, space0);
ConstantBuffer<ProceduralSkyConstants> bProceduralSkyConstants : register(b1, space0);

struct VSOutput
{
    float4 Position : SV_POSITION;
    float2 uv : TEXCOORD0;
};

VSOutput VSMain(uint vertexID : SV_VertexID)
{
    VSOutput output;
    float2 position[3] =
    {
        float2(-1.0, -1.0),
        float2(-1.0, 3.0),
        float2(3.0, -1.0)
    };
    
    output.Position = float4(position[vertexID], 0.0, 1.0);
    output.uv = position[vertexID] * 0.5 + 0.5;
    
    return output;
}

float3 GetWorldDirection(float2 uv)
{
    // UV [0,1]
    // ↓
    // NDC [-1,1]
    float2 ndc = uv * 2.0 - 1.0;

    // NDC
    // ↓
    // View Space
    float4 viewPos = mul(float4(ndc, 1.0, 1.0), bCamera.invProj);

    // 透視除算
    viewPos.xyz /= viewPos.w;

    // View Space
    // ↓
    // World Space
    float3 worldDir = mul(float4(viewPos.xyz, 0.0), bCamera.invView).xyz;

    return normalize(worldDir);
}

float3 GetSunDirection(float timeOfDay)
{
    float theta = timeOfDay * 2.0 * PI - PI / 2.0;
    return normalize(float3(cos(theta), sin(theta), 0.0));
}

float3 GetSkyColor(float3 viewDir, float3 sunDirection)
{
    float height = saturate(viewDir.y * 0.5 + 0.5);
    
    float3 horizonColor = float3(0.65, 0.8, 1.0);
    float3 zenithColor = float3(0.05, 0.2, 0.65);
    
    return lerp(horizonColor, zenithColor, height);
}

float3 ApplyDayNight(float3 skyColor, float3 sunDirection)
{
    float day = smoothstep(-0.15, 0.15, sunDirection.y);
    
    float3 nightColor = float3(0.005, 0.01, 0.04);
    
    return lerp(nightColor, skyColor, day);
}

float3 ApplySunset(float3 skyColor, float3 viewDir, float3 sunDirection)
{
    float sunset = 1.0 - smoothstep(0.0, 0.5, abs(sunDirection.y));
    
    float horizon = 1.0 - smoothstep(0.0, 0.6, abs(viewDir.y));
    
    float3 sunsetColor = float3(1.0, 0.2, 0.03);
    
    float factor = sunset * horizon;
    
    return lerp(skyColor, sunsetColor, factor);
}

float3 DrawSun(float3 skyColor, float3 viewDir, float3 sunDirection)
{
    float sunDot = saturate(dot(viewDir, sunDirection));
    
    float sun = pow(sunDot, 1000.0);
    
    float3 sunColor = float3(1.0, 0.9, 0.6);
    
    return skyColor + sun * sunColor;
}

float3 DrawSunGlow(float3 skyColor, float3 viewDir, float3 sunDirection)
{
    float sunDot = saturate(dot(viewDir, sunDirection));
    
    float sunGlow = pow(sunDot, 8.0);
    
    float3 glowColor = float3(1.0, 0.35, 0.05);
    
    return skyColor + sunGlow * glowColor * 0.5;
}

float3 GetMoonDirection(float3 sunDirection)
{
    return -sunDirection;
}

float3 DrawMoon(float3 skyColor, float3 viewDir, float3 moonDirection)
{
    float moonDot = saturate(dot(viewDir, moonDirection));
    
    float moon = pow(moonDot, 2000.0);
    
    float3 moonColor = float3(0.8, 0.85, 1.0);
    
    return skyColor + moon * moonColor;
}

float Random(float3 value)
{
    return frac(sin(dot(value, float3(12.9898, 78.233, 37.719))) * 43758.5453);
}

float3 DrawStars(float3 skyColor, float3 viewDir, float3 sunDirection)
{
    float day = smoothstep(-0.1, 0.1, sunDirection.y);
    
    float night = 1.0 - day;
    
    float3 grid = floor(viewDir * 250.0);
    
    float random = Random(grid);
    
    float star = step(0.995, random);
    
    float brightness = random;
    
    float3 starColor = float3(1.0, 1.0, 0.9) * brightness;
    
    return skyColor + starColor * star * brightness * night;
}

float2 Hash22(float2 p)
{
    p = float2(dot(p, float2(127.1, 311.7)), dot(p, float2(269.5, 183.3)));
    return frac(sin(p) * 43758.5453);
}

float WorleyNoise(float2 pos)
{
    float2 cell = floor(pos);
    float2 local = frac(pos);
    
    float minDistance = 1.0;
    
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            float2 offset = float2(x, y);
            
            // ランダムな点を生成
            float2 randomPoint = Hash22(cell + offset);
            
            // ローカル座標系での距離を計算
            float2 diff = offset + randomPoint - local;
            
            float distance = length(diff);
            
            // 最小距離を更新
            minDistance = min(minDistance, distance);
        }
    }
    
    return minDistance;
}

float GetCloudDensity(float2 uv)
{
    // 大きな雲の形
    float largeNoise = WorleyNoise(uv * 1.8);
    
    // 中くらいの雲の形
    float mediumNoise = WorleyNoise(uv * 4.0);
    
    // 小さな雲の形
    float smallNoise = WorleyNoise(uv * 9.0);
    
    // 大きな雲の塊を作る
    float cloud = 1.0 - largeNoise;
    
    // 中間の形で輪郭を崩す
    cloud += (1.0 - mediumNoise) * 0.35;
    
    // 細かい凹凸
    cloud += (1.0 - smallNoise) * 0.12;
    
    // 雲の密度を調整
    cloud = smoothstep(0.60, 0.88, cloud);
    
    return cloud;
}

float GetCloudLight(float2 cloudUV, float3 sunDirection)
{
    float light = 1.0;
    
    const int sampleCount = 6;
    const float sampleDistance = 0.12;
    
    for (int i = 1; i <= sampleCount; i++)
    {
        // 太陽方向へ向かって雲をサンプリング
        float2 sampleUV = cloudUV - sunDirection.xz * sampleDistance * i;
        
        float density = GetCloudDensity(sampleUV);
        
        // 雲が厚いほど光を遮る
        light -= density * 0.10;
    }
    
    return saturate(light);
}

float3 DrawClouds(float3 skyColor, float3 viewDir, float3 sunDirection)
{
    // 雲の高さ
    const float cloudHeight = 20.0;
    
    // 地平線より下には雲を描かない
    if (viewDir.y <= 0.01)
    {
        return skyColor;
    }
    
    // 視線と雲平面の交点を求める
    float t = cloudHeight / max(viewDir.y, 0.001);
    float3 cloudPosition = viewDir * t;
    
    // ワールド座標から雲のuvを作る
    float2 cloudUV = cloudPosition.xz;
    cloudUV *= 0.05;
    
    // 雲を風向きに沿って動かす
    float2 windDirection = normalize(float2(1.0, 0.25));
    
    cloudUV += windDirection * bProceduralSkyConstants.time * 0.01;
    
    // 雲の密度を取得
    float cloud = GetCloudDensity(cloudUV);
    
    // 地平線付近では雲を薄くする
    float horizonMask = smoothstep(0.08, 0.30, viewDir.y);
    
    cloud *= horizonMask;
    
    // 太陽からの光を計算
    float cloudLight = GetCloudLight(cloudUV, sunDirection);
    
    // 太陽の高さ
    float sunHeight = saturate(sunDirection.y);
    
    // 昼の雲の色
    float3 dayCloudColor = float3(1.0, 1.0, 1.0);
    
    // 夕方の雲の色
    float3 sunsetCloudColor = float3(1.0, 0.55, 0.30);
    
    // 夕方になるほど夕焼け色にする
    float sunsetFactor = 1.0 - smoothstep(0.05, 0.35, sunHeight);
    
    float3 cloudColor = lerp(
        dayCloudColor,
        sunsetCloudColor,
        sunsetFactor
    );
    
    // 光が当たっている場所を明るくする
    cloudColor *= lerp(0.45, 1.0, cloudLight);
    
    // 太陽に近い部分を明るくする
    float sunDot = saturate(dot(viewDir, sunDirection));
    
    float silverLining = pow(sunDot, 8.0);
    
    silverLining *= cloud;
    
    cloudColor += silverLining * float3(1.0, 0.8, 0.5) * 0.5;
    
    // 昼夜によって雲の色を変える
    float day = smoothstep(-0.15, 0.15, sunDirection.y);
    
    float3 nightCloudColor = float3(0.08, 0.10, 0.16);
    
    cloudColor = lerp(
        nightCloudColor,
        cloudColor,
        day
    );
    
    // 雲の透明度を調整
    float cloudAlpha = cloud * 0.95;
    
    return lerp(
        skyColor,
        cloudColor,
        cloudAlpha
    );
}

float4 PSMain(VSOutput input) : SV_TARGET
{
    float3 viewDir = GetWorldDirection(input.uv);

    float3 sunDir = GetSunDirection(bProceduralSkyConstants.timeOfDay);
    
    float3 moonDir = GetMoonDirection(sunDir);
    
    // 空
    float3 color = GetSkyColor(viewDir, sunDir);
    
    // 昼夜
    color = ApplyDayNight(color, sunDir);
    
    // 夕暮れ
    color = ApplySunset(color, viewDir, sunDir);
    
    // 太陽
    if(bProceduralSkyConstants.enableSun)
    {
        color = DrawSun(color, viewDir, sunDir);
    }
    
    // 太陽の光
    if(bProceduralSkyConstants.enableSunGlow)
    {
        color = DrawSunGlow(color, viewDir, sunDir);
    }
    
    // 月
    if(bProceduralSkyConstants.enableMoon)
    {
        color = DrawMoon(color, viewDir, moonDir);
    }
    
    // 星
    if(bProceduralSkyConstants.enableStars)
    {
        color = DrawStars(color, viewDir, sunDir);
    }
    
    // 雲
    if(bProceduralSkyConstants.enableClouds)
    {
        color = DrawClouds(color, viewDir, sunDir);
    }
    
    return float4(color, 1.0);
}