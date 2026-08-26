#pragma once
#include "CollisionPair.h"
#include "SpatialHashBroadPhase.h"

class CollisionSolver
{
private:
	// 空間ハッシュを使用したBroadPhase
	SpatialHashBroadPhase m_broadPhase{500.0f};

	// 前フレームに衝突していたペア
	std::unordered_map<CollisionPairKey, CollisionPairData, CollisionPairKeyHash> m_previousPairs;

	// 現在のSubstepで衝突しているペア
	std::unordered_map<CollisionPairKey, CollisionPairData, CollisionPairKeyHash> m_stepPairs;

	// このフレーム中に一度でも接触したペア
	std::unordered_map<CollisionPairKey, CollisionPairData, CollisionPairKeyHash> m_framePairs;

	// BroadPhaseで検出された衝突候補ペア
	std::vector<CollisionPairKey> m_candidatePairs;

private:
	// コライダーのサイズをTransformのスケールに基づいてスケーリングした半径
	Vector3 GetScaledHalfExtents(const Collider& col, const Transform& trans) const;

	// コライダーの半径をTransformのスケールに基づいてスケーリングした半径
	float GetScaledRadius(const Collider& col, const Transform& trans) const;

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

	void Detect(World& world);

	void Solve(World& world);

	void SolvePair(World& world, Entity entityA, Entity entityB, const CollisionPairData& pair);

	void AddContact(CollisionEvents* events, const ContactInfo& contact, CollisionState state);

	// コライダーの境界ボックスを計算します。コライダーの種類に応じて適切な境界ボックスを計算します。
	BroadPhaseBounds CalculateBounds(const Collider& col, const Transform& trans) const;

	// BroadPhaseを構築します。空間ハッシュを使用して、コライダーの境界ボックスをセルに挿入し、衝突ペアを生成します。
	void BuildBroadPhase(World& world);

public:
	void BeginFrame(World& world);
	void Step(World& world);
	void EndFrame(World& world);

	// コライダーのデバッグ描画を行います。デバッグ用のラインやワイヤーフレームを描画するための関数です。
	void DrawDebugColliders(World& world);
};