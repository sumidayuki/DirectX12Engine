#pragma once

/// <summary>
/// 当たり判定を行うシステムです。
/// Colliderコンポーネントを所持している全てのエンティティ同士の当たり判定を行います。
/// </summary>
class CollisionSystem : public System
{
private:
	// 円コライダー同士が当たっているかを確認します。
	void CheckCollision(CircleCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB);

	// 円コライダーが矩形コライダーに当たっているかを確認します。
	void CheckCollision(CircleCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB);

	// 矩形コライダー同士が当たっているかを確認します。
	void CheckCollision(BoxCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB);

	// 矩形コライダーが円コライダーに当たっているかを確認します。
	void CheckCollision(BoxCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB);

public:
	void Update(ComponentManager& cm, World& world) override;
};