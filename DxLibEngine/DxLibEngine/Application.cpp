#include "Application.h"
#include "TitleScene.h"
#include "MainScene.h"

void Application::Run()
{
	m_gameWindow = new DxWindow();
	m_gameWindow->SetClientSize(Vector2Int(Screen::GetWidth(), Screen::GetHeight()));
	m_gameWindow->Centering();
	::ShowWindow(m_gameWindow->GetHandle(), SW_HIDE);

	// �E�B���h�E�n���h��
	HWND hWnd = GetMainWindowHandle();

	// ���̓V�X�e���̏�����
	InputManager::Initialize(hWnd);

	// ���Ԃ̏�����
	Time::StaticConstructor();

	// �V�[���̒ǉ�
	m_sceneManager = new SceneManager();
	m_sceneManager->AddScene("Title", std::make_unique<TitleScene>());
	m_sceneManager->AddScene("Main", std::make_unique<MainScene>());
	m_sceneManager->ChangeScene("Title");

	// �E�B���h�E��\�����ăR���g���[�����X�V����
	::ShowWindowAsync(hWnd, SW_SHOW);
	::SetForegroundWindow(hWnd);

	m_isRunning = true;

	Time::m_startupTimePoint = std::chrono::high_resolution_clock::now();
	Time::m_lastLevelLoadTime = Time::m_startupTimePoint;
	auto previousTimeForFixed = Time::m_startupTimePoint;
	auto previousTimeForVariable = Time::m_startupTimePoint;
	auto previousTimeForFpsCouter = Time::m_startupTimePoint;
	auto previousFrameCount = Time::m_frameCount;

	m_sceneManager->Start();

	while (m_isRunning)
	{
		if (ProcessMessage() != 0) break;
		// ���݂̎���
		const auto currentTime = std::chrono::high_resolution_clock::now();

		// �O��̃t���[������̌o�ߎ���
		const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTimeForVariable);

		// 1�t���[��������̎���
		const std::chrono::microseconds microsecondsPerFrame = std::chrono::microseconds((Time::m_captureFramerate > 0) ? (1000000 / Time::m_captureFramerate) : 16666);

		// �o�ߎ��� >= 1�t���[��������̎���
		if (elapsedTime >= microsecondsPerFrame)
		{
			Update();

			Draw();

			// ���̃t���[���̂��߂ɏ㏑�����Ă���
			Time::m_unscaledDeltaTime = std::chrono::duration<float, std::ratio<1, 1>>(elapsedTime).count();
			Time::m_unscaledTime += Time::m_unscaledDeltaTime;
			Time::m_deltaTime = Time::m_unscaledDeltaTime * Time::m_timeScale;
			Time::m_time += Time::m_deltaTime;
			Time::m_frameCount++;
			previousTimeForVariable = currentTime;

			ScreenFlip();
		}
	}

	m_isRunning = false;
}

void Application::Update()
{
	// ���̓V�X�e���̍X�V����
	InputManager::Update();

	if (Keyboard::GetKeyState(KeyCode::Escape).IsPressed())
	{
		m_isRunning = false;
	}

	m_sceneManager->Update();
}

void Application::Draw()
{
	ClearDrawScreen();

	m_sceneManager->Draw();
}