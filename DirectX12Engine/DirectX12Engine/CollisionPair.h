#pragma once

/// <summary>
/// コリジョンペアを一意に識別するためのキー構造体です。Entity IDの組み合わせで衝突ペアを表現します。
/// </summary>
struct CollisionPairKey
{
	Entity entityA;
	Entity entityB;

	CollisionPairKey() : entityA(INVALID_ENTITY), entityB(INVALID_ENTITY) {}

	CollisionPairKey(Entity a, Entity b)
	{
		// Always store in sorted order to avoid (A,B) != (B,A)
		if (a.id < b.id)
		{
			entityA = a;
			entityB = b;
		}
		else
		{
			entityA = b;
			entityB = a;
		}
	}

	bool operator==(const CollisionPairKey& other) const
	{
		return entityA.id == other.entityA.id && entityB.id == other.entityB.id;
	}
};

struct CollisionPairKeyHash
{
	size_t operator()(const CollisionPairKey& key) const
	{
		size_t h1 = std::hash<EntitySize>{}(key.entityA.id);
		size_t h2 = std::hash<EntitySize>{}(key.entityB.id);
		return h1 ^ (h2 << 1);
	}
};

/// <summary>
/// コリジョンペアのデータを格納する構造体です。各エンティティの接触情報を保持します。
/// </summary>
struct CollisionPairData
{
	ContactInfo contactA;   // Entity A に対する接触情報 
	ContactInfo contactB;   // Entity B に対する接触情報
};