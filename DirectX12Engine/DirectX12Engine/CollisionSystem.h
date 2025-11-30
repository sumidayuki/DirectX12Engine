#pragma once

/// <summary>
/// 当たり判定を行うシステムです。
/// Colliderコンポーネントを所持している全てのエンティティ同士の当たり判定を行います。
/// </summary>
class CollisionSystem : public System
{
private:
    // 衝突状態(Enter/Stay/Exit)を更新する共通関数
    void UpdateComponentState(CollisionInfo& info, Entity otherEntity, bool isHitNow);

    // 2つのエンティティのコンポーネント型を確認し、適切な判定関数を呼び出す
    bool CheckPair(Entity entityA, Entity entityB, World& world);

    // Sphere vs Sphere
    bool IsColliding(const SphereCollider& a, const Vector3& posA, const SphereCollider& b, const Vector3& posB);

    // AABB vs AABB
    bool IsColliding(const AABBCollider& a, const Vector3& posA, const AABBCollider& b, const Vector3& posB);

    // Sphere vs AABB
    bool IsColliding(const SphereCollider& sphere, const Vector3& spherePos, const AABBCollider& aabb, const Vector3& aabbPos);



public:
	void Update(World& world) override;
};