#ifndef MESH_RENDERER_HLSLI
#define MESH_RENDERER_HLSLI

#include "TypeMarshalling.hlsli"
#include "Light.hlsli"
#include "Material.hlsli"

// オブジェクト1個分の情報を表す構造体
struct ObjectLayout
{
    matrix world; // ワールド変換行列
};

// ライトに関する定数
struct LightConstants
{
    uint lightCount; // ライト数
};

// マテリアルに関する定数
struct MaterialConstants
{
    uint materialIndex; // マテリアルインデックス
};

#endif // MESH_RENDERER_HLSLI