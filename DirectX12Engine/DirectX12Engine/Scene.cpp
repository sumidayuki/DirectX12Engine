#include "Precompiled.h"

Scene::Scene()
	: isStart(false)
{
}

bool Scene::OnLoad()
{
	if (!Load())
	{
		return false;
	}

	return m_world.Load(m_world);
}

void Scene::OnUnload()
{
	Unload();
	m_world.Clear();
}

void Scene::OnStart()
{
	Start();
	m_world.Start(m_world);
}

void Scene::OnBeginFrame(UINT frameIndex)
{
	m_world.BeginFrame(frameIndex);
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