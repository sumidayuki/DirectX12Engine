#include "Precompiled.h"

Scene::Scene()
	: isStart(false)
{
}

bool Scene::OnLoad()
{
	if (!m_world.Load(m_world))
	{
		return false;
	}

	return Load();
}

void Scene::OnUnload()
{
	m_world.Unload(m_world);
	m_world.Clear();
	AudioManager::GetInstance()->CleanupFinishedVoices();
	Unload();
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