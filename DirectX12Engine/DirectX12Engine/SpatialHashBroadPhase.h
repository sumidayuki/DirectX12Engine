#pragma once

#include "CollisionPair.h"

// この構造体は、3D空間内のセルの座標を表します。
struct CellCoords3D
{
	int x;
	int y;
	int z;

	bool operator==(const CellCoords3D& other) const
	{
		return
			x == other.x &&
			y == other.y &&
			z == other.z;
	}
};

// この構造体は、CellCoords3Dをハッシュ化するための関数オブジェクトです。
struct CellCoords3DHash
{
	size_t operator()(const CellCoords3D& key) const
	{
		size_t h1 = std::hash<int>{}(key.x);
		size_t h2 = std::hash<int>{}(key.y);
		size_t h3 = std::hash<int>{}(key.z);

		return
			h1 ^
			(h2 << 1) ^
			(h3 << 2);
	}
};

// この構造体は、3D空間内の境界ボックスを表します。
struct BroadPhaseBounds
{
	Vector3 min;
	Vector3 max;
};

class SpatialHashBroadPhase
{
private:
	struct Cell
	{
		std::vector<Entity> entities;
	};

private:
	float m_cellSize;

	std::unordered_map<CellCoords3D, Cell, CellCoords3DHash> m_cells;

private:
	// 指定された位置に対応するセルの座標を取得します。
	CellCoords3D GetCellCoords(const Vector3& position) const;

public:
	// コンストラクタ。セルのサイズを指定できます。デフォルトは4.0fです。
	explicit SpatialHashBroadPhase(float cellSize = 4.0f);

	void Clear();

	// 指定されたエンティティとその境界ボックスを空間ハッシュに挿入します。
	void Insert(Entity entity, const BroadPhaseBounds& bounds);

	// 指定されたエンティティとその境界ボックスを空間ハッシュから削除します。
	void GeneratePairs(std::vector<CollisionPairKey>& outPairs) const;
};