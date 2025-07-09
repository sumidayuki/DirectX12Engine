#pragma once
#include "TransformSystem.h"

class PlayerSystem : public System
{
private:
	TransformSystem* m_transformSystem;

	float m_timer;
	const float m_coolDown = 0.1;

	Texture2D* m_bulletTex;

	// 体力を表すハートのオブジェクト
	std::queue<Entity> m_lifeObjects;

private:
	void MovePlayer(Input& input, Transform& transform, Velocity& velocity);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};