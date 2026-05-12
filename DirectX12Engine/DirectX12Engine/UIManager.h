#pragma once

class UIManager : public Singleton<UIManager>
{
	friend class Singleton<UIManager>;

private:
	std::unordered_map<uint64_t, Entity> m_canvasEntities;
	std::unordered_map<uint64_t, std::unordered_map<uint64_t, Entity>> m_uiObjects;

private:
	UIManager();
	~UIManager();

public:
	void AddCanvas(Entity entity, uint64_t canvasID);
	Entity GetCanvas(uint64_t canvasID);
	void AddUIObject(Entity entity, uint64_t canvasID, uint64_t objectID);
	Entity GetUIObject(uint64_t canvasID, uint64_t objectID);
};