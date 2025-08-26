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
	}
}

void SceneManager::Update()
{
	if (m_currentScene)
	{
		if (m_currentScene->isDirty)
		{
			m_currentScene->Start();
			m_currentScene->isDirty = false;
		}

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

void SceneManager::StaticDestructor()
{
	m_currentScene = nullptr;
	m_scenes.clear();
}
