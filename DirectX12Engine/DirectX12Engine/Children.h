#pragma once

struct Children : public IComponentData
{
	std::vector<Entity> children;
};