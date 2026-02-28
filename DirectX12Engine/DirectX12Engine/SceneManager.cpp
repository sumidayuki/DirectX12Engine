#include "SceneManager.h"

void SceneManager::AddScene(const std::string& name, std::unique_ptr<Scene> scene)
{
	m_scenes[name] = std::move(scene);
}

void SceneManager::ChangeScene(const std::string& name)
{
	m_pendingSceneName = name;
}

void SceneManager::ProcessPendingSceneChange()
{
	if (m_pendingSceneName.empty()) return;

	std::string name = m_pendingSceneName;
	m_pendingSceneName.clear();

	if (m_scenes.contains(name))
	{
		if (m_currentScene)
		{
			m_currentScene->OnUnload();
			m_currentScene->isStart = false;
		}

		m_currentScene = m_scenes[name].get();
		if (!m_currentScene->OnLoad())
		{
			assert(0);
		}

		// シーン読み込みに費やした時間をスキップする
		Time::ResetDeltaTimeOnSceneLoad();
	}
}

void SceneManager::BeginFrame(UINT frameIndex)
{
	ProcessPendingSceneChange();

	if (m_currentScene)
	{
		m_currentScene->OnBeginFrame(frameIndex);
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
	if (m_currentScene && m_currentScene->isStart)
	{
		m_currentScene->OnDraw();
	}
}

void SceneManager::StaticDestructor()
{
	m_currentScene = nullptr;
	m_pendingSceneName.clear();
	m_scenes.clear();
}