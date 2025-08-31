#include "Application.h"
#include "SpriteRendererSystem.h"
#include "MeshRendererSystem.h"
#include "TitleScene.h"
#include "MainScene.h"

void Application::Run()
{
	// �Q�[���p�̃E�B���h�E���쐬
	m_gameWindow = new Windows::NativeWindow();  // "DirectX12�v���O���~���O"
	m_gameWindow->SetClientSize({ Screen::GetWidth(), Screen::GetHeight() });
	m_gameWindow->Centering();
	::ShowWindow(m_gameWindow->GetHandle(), SW_HIDE);

	// ���[�J�[�X���b�h���쐬
	m_isRunning = true;
	std::thread workerThread(WorkerThreadEntryPoint);

	// ���b�Z�[�W���[�v
	PumpMessage();

	// ���[�J�[�X���b�h���I�������č���
	m_isRunning = false;
	workerThread.join();

    Shutdown();
}

void Application::Shutdown()
{
    m_sceneManager = nullptr;
    m_gameWindow = nullptr;
}

void Application::PumpMessage()
{
	// ���b�Z�[�W1�����i�[����ϐ�
	MSG msg;

	while (::GetMessage(&msg, nullptr, 0, 0))
	{
		// ���z�L�[���b�Z�[�W�𕶎����b�Z�[�W�ɕϊ����܂��B
		::TranslateMessage(&msg);

		// ���b�Z�[�W���E�B���h�E�v���V�[�W���ɔz�����܂��B
		::DispatchMessage(&msg);
	}
}

void Application::AddScenesAndStartupScene()
{
	// �V�[���̒ǉ�
	m_sceneManager = new SceneManager();
	m_sceneManager->AddScene("Title", std::make_unique<TitleScene>());
	m_sceneManager->AddScene("Main", std::make_unique<MainScene>());
	m_sceneManager->ChangeScene("Main");
}

void Application::WorkerThreadEntryPoint()
{
    // �E�B���h�E�n���h��
    HWND hWnd = m_gameWindow->GetHandle();

    // �O���t�B�b�N�V�X�e���̏�����
    Resolution resolution;
    resolution.width = Screen::GetWidth();
    resolution.height = Screen::GetHeight();
    resolution.refreshRate = 60;
    Graphics::StaticConstructor(hWnd, resolution);
    m_gameWindow->SetFullscreen(Screen::IsFullScreen());

    // �X�v���C�g�����_���[�̏�����
    SpriteRendererSystem::StaticConstructor();

    // ���b�V�������_���[�V�X�e���̏�����
    MeshRendererSystem::StaticConstructor();

    SkinnedMeshRendererSystem::StaticConstructor();

    // ���̓V�X�e���̏�����
    InputManager::StaticConstructor(hWnd);

    // ���Ԃ̏�����
    Time::StaticConstructor();

    // ���炩���߃r���h�ݒ�ɑS�ẴV�[����o�^���Ă���
    AddScenesAndStartupScene();

    // �E�B���h�E��\�����ăR���g���[�����X�V����
    ::ShowWindowAsync(hWnd, SW_SHOW);
    ::SetForegroundWindow(hWnd);

    Time::m_startupTimePoint = std::chrono::high_resolution_clock::now();
    Time::m_lastLevelLoadTime = Time::m_startupTimePoint;
    auto previousTimeForFixed = Time::m_startupTimePoint;
    auto previousTimeForVariable = Time::m_startupTimePoint;
    auto previousTimeForFpsCounter = Time::m_startupTimePoint;
    auto previousFrameCount = Time::m_frameCount;

    // �Q�[�����[�v
    while (m_isRunning)
    {
        // ���݂̎���
        const auto currentTime = std::chrono::high_resolution_clock::now();

        // �O��̃t���[������̌o�ߎ���
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTimeForVariable);

        // 1�t���[��������̎���
        const std::chrono::microseconds microsecondsPerFrame = std::chrono::microseconds((Time::m_captureFramerate > 0) ? (1000000 / Time::m_captureFramerate) : 16666);

        // �o�ߎ��� >= 1�t���[��������̎���
        if (elapsedTime >= microsecondsPerFrame)
        {
            // ���̓V�X�e���̍X�V����
            InputManager::Update();

            // �Q�[���̍X�V (�Q�[�����̎��Ԃ����������i�߂�)
            // �Q�[����ʂ��t���[���o�b�t�@�Ƀ����_�����O
            FrameResource* currentFrameResource = Graphics::GetCurrentFrameResource();
            currentFrameResource->Update();
            currentFrameResource->Render();

            // ���̃t���[���ׂ̈ɏ㏑�����Ă���
            Time::m_unscaledDeltaTime = std::chrono::duration<float, std::ratio<1, 1>>(elapsedTime).count();
            Time::m_unscaledTime += Time::m_unscaledDeltaTime;
            Time::m_deltaTime = Time::m_unscaledDeltaTime * Time::m_timeScale;
            Time::m_time += Time::m_deltaTime;
            Time::m_frameCount++;
            previousTimeForVariable = currentTime;
        }
    }

    // �S�Ẵt���[�����\�[�X�ɂ�����Present()�̊�����҂B
    Graphics::WaitForCompletionOfPresent();

    // �V�[���}�l�[�W���[�̏I������
    SceneManager::StaticDestructor();

    // �C���v�b�g�}�l�[�W���[�̏I������
    InputManager::StaticDestructor();

    // ���b�V�������_���[�V�X�e���̏I������
    MeshRendererSystem::StaticDestructor();

    SkinnedMeshRendererSystem::StaticDestructor();

    // �X�v���C�g�����_���[�V�X�e���̏I������
    SpriteRendererSystem::StaticDestructor();

    // �O���t�B�b�N�V�X�e���̏I������
    Graphics::StaticDestructor();
}