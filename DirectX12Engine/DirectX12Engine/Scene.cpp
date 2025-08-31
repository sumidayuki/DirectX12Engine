#include "Precompiled.h"

Scene::Scene()
	: isStart(false)
{
}

void Scene::OnStart()
{
	Start();
	m_world.Start(m_world);
}

void Scene::OnUpdate()
{
	Update();
	m_world.Update(m_world);
}

void Scene::OnDraw()
{
	Draw();
	m_world.Draw(m_world);
}
