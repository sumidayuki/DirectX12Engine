#include "SceneManager.h"

void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene)
{
	m_scenes[name] = std::move(scene);
}

void SceneManager::ChangeScene(const std::string& name)
{
	if (m_scenes.contains(name))
	{
		m_currentScene = m_scenes[name].get();
	}
}

void SceneManager::Update()
{
	if (m_currentScene)
	{
		if (!m_currentScene->isStart)
		{
			m_currentScene->OnStart();
			m_currentScene->isStart = true;
		}

		m_currentScene->OnUpdate();
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->OnDraw();
	}
}

void SceneManager::StaticDestructor()
{
	m_currentScene = nullptr;
	m_scenes.clear();
}
