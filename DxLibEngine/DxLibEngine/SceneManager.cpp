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
		Start();
	}
}

void SceneManager::Start()
{
	if (m_currentScene)
	{
		m_currentScene->Start();
	}
}

void SceneManager::Update()
{
	if (m_currentScene)
	{
		m_currentScene->Update();
	}
}

void SceneManager::Draw()
{
	if (m_currentScene)
	{
		m_currentScene->Draw();
	}
}
