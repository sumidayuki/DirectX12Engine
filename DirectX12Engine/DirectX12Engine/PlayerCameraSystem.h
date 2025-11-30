#pragma once

class PlayerCameraSystem : public System
{
private:
	Transform* m_playerTransform;

	float m_yaw, m_pitch;

private:
	void Start(World& world) override;

	void Draw(World& world) override;
};