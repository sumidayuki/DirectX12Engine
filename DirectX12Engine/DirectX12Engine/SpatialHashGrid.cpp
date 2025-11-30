#include "SpatialHashGrid.h"
#include <algorithm> // std::sort, std::unique, std::insert
#include <cmath>     // std::floor

SpatialHashGrid::SpatialHashGrid(float size)
	: m_cellSize(size) // メンバー名 m_cellSize に統一
{
}

// CellCoords3D を戻り値として使用
CellCoords3D SpatialHashGrid::GetCellCoords(const Vector3& position) const
{
	return
	{
		static_cast<int>(std::floor(position.x / m_cellSize)),
		static_cast<int>(std::floor(position.y / m_cellSize)),
		static_cast<int>(std::floor(position.z / m_cellSize))
	};
}

void SpatialHashGrid::Clear()
{
	m_grid.clear(); // メンバー名 m_grid に統一
}

void SpatialHashGrid::AddEntity(const Entity& entity, const Vector3& position)
{
	m_grid[GetCellCoords(position)].entities.push_back(entity);
}

const std::vector<Entity>& SpatialHashGrid::GetNearbyEntities(const Vector3& position) const
{
	static std::vector<Entity> result;
	result.clear();
	auto [cx, cy, cz] = GetCellCoords(position); // 3D座標を取得

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			for (int dz = -1; dz <= 1; ++dz)
			{
				CellCoords3D cellKey = { cx + dx, cy + dy, cz + dz };
				// メンバー名 m_grid に統一
				auto it = m_grid.find(cellKey);
				if (it != m_grid.end())
				{
					result.insert(result.end(), it->second.entities.begin(), it->second.entities.end());
				}
			}
		}
	}
	// 重複エンティティの削除 (Entity::operator< が必要)
	std::sort(result.begin(), result.end());
	// std::unique には Entity::operator== が必要
	result.erase(std::unique(result.begin(), result.end()), result.end());
	return result;
}