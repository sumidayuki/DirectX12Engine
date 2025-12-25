#ifndef SKINNEDMESHRENDERER_HLSLI
#define SKINNEDMESHRENDERER_HLSLI

#include "TypeMarshalling.hlsli"
#include "Light.hlsli"
#include "Material.hlsli"

// オブジェクト1個分の情報を表す構造体
struct ObjectLayout
{
    matrix world; // ワールド変換行列
    int MAX_BONES = 256; // シェーダーでサポートするボーンの最大数
    matrix boneMatrices[MAX_BONES];
};

// ライトに関する定数
struct LightConstants
{
    uint padding[3];
};

// マテリアルに関する定数
struct MaterialConstants
{
    uint materialIndex; // マテリアルインデックス
    uint lightCount; // ライト数
};

#endif // MESH_RENDERER_HLSLI