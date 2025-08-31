#pragma once

class IComponentStorage
{
public:
	virtual ~IComponentStorage() = default;

	virtual void Remove(Entity entity) = 0;
};