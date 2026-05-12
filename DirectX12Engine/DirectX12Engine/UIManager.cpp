#include "UIManager.h"

UIManager::UIManager()
{
	m_canvasEntities.clear();
	m_uiObjects.clear();
}

UIManager::~UIManager()
{
	m_canvasEntities.clear();
	m_uiObjects.clear();
}

void UIManager::AddCanvas(Entity entity, uint64_t canvasID)
{
	m_canvasEntities[canvasID] = entity;
}

Entity UIManager::GetCanvas(uint64_t canvasID)
{
	auto it = m_canvasEntities.find(canvasID);
	if (it != m_canvasEntities.end())
	{
		return it->second;
	}
	return Entity{ 0, 0 };
}

void UIManager::AddUIObject(Entity entity, uint64_t canvasID, uint64_t objectID)
{
	m_uiObjects[canvasID][objectID] = entity;
}

Entity UIManager::GetUIObject(uint64_t canvasID, uint64_t objectID)
{
	auto canvasIt = m_uiObjects.find(canvasID);
	if (canvasIt != m_uiObjects.end())
	{
		auto objectIt = canvasIt->second.find(objectID);
		if (objectIt != canvasIt->second.end())
		{
			
			return objectIt->second;
		}
	}
	return INVALID_ENTITY;
}
