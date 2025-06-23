#include "TitleScene.h"
#include "SystemList.h"

void TitleScene::Start()
{
	m_world.AddSystem(std::make_unique<InputSystem>());
}

void TitleScene::Update()
{
	m_time += Time::GetDeltaTime();

	if (Keyboard::GetKeyState(KeyCode::Space).WasPressedThisFrame())
	{
		SceneManager::ChangeScene("Main");
	}
}

void TitleScene::Draw()
{
}
