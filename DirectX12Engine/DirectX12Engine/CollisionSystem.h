#pragma once
#include "CollisionPair.h"

/// <summary>
/// コリジョンシステムクラスです。全ペアのコリジョンテスト、狭義フェーズ（Sphere、AABB、OBB via SAT）、
/// ペアごとのEnter/Stay/Exit状態の追跡、レイヤーベースのコリジョンフィルタリング（CollisionMatrix + Collider::collisionMask）、
/// ECSフレンドリーなイベント消費のためのCollisionEventsコンポーネント、レガシーCollisionInfoの互換性を提供します。
/// </summary>
class CollisionSystem : public System
{
private:
	// 前フレームのアクティブなコリジョンペア
    std::unordered_map<CollisionPairKey, CollisionPairData, CollisionPairKeyHash> m_previousPairs;

	// 現フレームのアクティブなコリジョンペア
    std::unordered_map<CollisionPairKey, CollisionPairData, CollisionPairKeyHash> m_currentPairs;

private:
    // Sphere vs Sphere
    bool TestSphereSphere(
        const Vector3& posA, float radiusA,
        const Vector3& posB, float radiusB,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

    // AABB vs AABB
    bool TestAABBAABB(
        const Vector3& posA, const Vector3& halfExtA,
        const Vector3& posB, const Vector3& halfExtB,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

    // OBB vs OBB
    bool TestOBBOBB(
        const Vector3& posA, const Vector3& halfExtA, const Matrix4x4& rotA,
        const Vector3& posB, const Vector3& halfExtB, const Matrix4x4& rotB,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

    // Sphere vs AABB
    bool TestSphereAABB(
        const Vector3& spherePos, float sphereRadius,
        const Vector3& boxPos, const Vector3& boxHalfExt,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

    // Sphere vs OBB
    bool TestSphereOBB(
        const Vector3& spherePos, float sphereRadius,
        const Vector3& boxPos, const Vector3& boxHalfExt, const Matrix4x4& boxRot,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

    // AABB vs OBB
    bool TestAABBOBB(
        const Vector3& aabbPos, const Vector3& aabbHalfExt,
        const Vector3& obbPos, const Vector3& obbHalfExt, const Matrix4x4& obbRot,
        Vector3& outNormal, float& outPenetration, Vector3& outPoint);

	// 汎用的なペアテスト関数。コライダの種類に応じて適切なテスト関数を呼び出します。
    bool TestPair(
        const Collider& colA, const Transform& transA,
        const Collider& colB, const Transform& transB,
        ContactInfo& outContactA, ContactInfo& outContactB);

	// レイヤーベースのコリジョンフィルタリング関数。CollisionMatrixとCollider::collisionMaskを使用して、2つのコライダが衝突すべきかを判断します。
    Vector3 GetScaledHalfExtents(const Collider& col, const Transform& trans) const;
    float GetScaledRadius(const Collider& col, const Transform& trans) const;

	// コリジョンペアの状態を更新し、CollisionEventsコンポーネントを生成/更新します。
    void DrawDebugColliders(World& world);

public:
    void Update(World& world) override;
};