#pragma once

class PlayerCameraSystem : public System
{
private:
	Transform* m_playerTransform;

	float m_yaw, m_pitch;

private:
	void Start(ComponentManager& cm, World& world) override;

	void Draw(ComponentManager& cm, World& world) override;
};