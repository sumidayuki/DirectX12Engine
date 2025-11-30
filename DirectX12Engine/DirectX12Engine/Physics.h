#pragma once

/// <summary>
/// レイと衝突した結果を格納する構造体です。
/// </summary>
struct RaycastHit
{
    Entity entity = INVALID_ENTITY;      // 衝突したエンティティ
    float distance = -1.0f;        // レイの原点から衝突点までの距離 (t)
    Vector3 point = Vector3::zero; // ワールド座標での衝突点
    Vector3 normal = Vector3::zero;// 衝突面における法線ベクトル (オプションだが重要)
};

class Physics
{
public:
    static bool Raycast(World& world, const Ray& ray, RaycastHit& hit, float maxDistance = Mathf::Infinity, LayerMask targetMask = Layers::Default);
};