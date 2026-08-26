#include "SpatialHashBroadPhase.h"

CellCoords3D SpatialHashBroadPhase::GetCellCoords(const Vector3& position) const
{
	return CellCoords3D(
		static_cast<int>(std::floor(position.x / m_cellSize)),
		static_cast<int>(std::floor(position.y / m_cellSize)),
		static_cast<int>(std::floor(position.z / m_cellSize))
	);
}

SpatialHashBroadPhase::SpatialHashBroadPhase(float cellSize)
	: m_cellSize(cellSize)
{
}

void SpatialHashBroadPhase::Clear()
{
	m_cells.clear();
}

void SpatialHashBroadPhase::Insert(Entity entity, const BroadPhaseBounds& bounds)
{
	CellCoords3D minCell = GetCellCoords(bounds.min);
	CellCoords3D maxCell = GetCellCoords(bounds.max);

	for (int x = minCell.x; x <= maxCell.x; ++x)
	{
		for (int y = minCell.y; y <= maxCell.y; ++y)
		{
			for (int z = minCell.z; z <= maxCell.z; ++z)
			{
				CellCoords3D cellCoords{ x, y, z };
				m_cells[cellCoords].entities.push_back(entity);
			}
		}
	}
}

void SpatialHashBroadPhase::GeneratePairs(std::vector<CollisionPairKey>& outPairs) const
{
	outPairs.clear();

	// 同じ組み合わせのペアを重複して追加しないように、unordered_setを使用して一意のペアを追跡します。
	std::unordered_set<CollisionPairKey, CollisionPairKeyHash> uniquePairs;

	for (const auto& [coords, cell] : m_cells)
	{
		const auto& entities = cell.entities;

		for (size_t i = 0; i < entities.size(); ++i)
		{
			for (size_t j = i + 1; j < entities.size(); ++j)
			{
				CollisionPairKey pair(entities[i], entities[j]);
				if (uniquePairs.insert(pair).second)
				{
					outPairs.push_back(pair);
				}
			}
		}
	}
}
