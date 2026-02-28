#pragma once

class BattleCameraSystem : public System
{
private:
	Entity m_currentCameraEntity;
	Transform* m_characterTransform1 = nullptr;
	Transform* m_characterTransform2 = nullptr;

private:
	void Start(World& world) override;
	void Update(World& world) override;

public:
	Entity GetCurrentCameraEntity() const { return m_currentCameraEntity; }
};