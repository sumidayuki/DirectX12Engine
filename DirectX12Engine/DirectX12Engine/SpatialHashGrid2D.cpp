#include "SpatialHashGrid2D.h"

std::pair<int, int> SpatialHashGrid2D::GetCellCoords(const Vector2& position) const
{
	return
	{
		static_cast<int>(std::floor(position.x / cellSize)),
		static_cast<int>(std::floor(position.y / cellSize))
	};
}

SpatialHashGrid2D::SpatialHashGrid2D(float size)
	: cellSize(size)
{
}

void SpatialHashGrid2D::Clear()
{
	grid.clear();
}

void SpatialHashGrid2D::AddEntity(const Entity& entity, const Vector2& position)
{
	grid[GetCellCoords(position)].entities.push_back(entity);
}

const std::vector<Entity>& SpatialHashGrid2D::GetNearbyEntities(const Vector2& position) const
{
	static std::vector<Entity> result;
	result.clear();
	auto [cx, cy] = GetCellCoords(position);

	for (int dx = -1; dx <= 1; ++dx)
	{
		for (int dy = -1; dy <= 1; ++dy)
		{
			auto it = grid.find({ cx + dx, cy + dy });
			if (it != grid.end())
			{
				result.insert(result.end(), it->second.entities.begin(), it->second.entities.end());
			}
		}
	}
	return result;
}
