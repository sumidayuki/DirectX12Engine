#include "Application.h"
#include "SpriteRendererSystem.h"
#include "MeshRendererSystem.h"
#include "TitleScene.h"
#include "MainScene.h"

void Application::Run()
{
	// ゲーム用のウィンドウを作成
	m_gameWindow = new Windows::NativeWindow();  // "DirectX12プログラミング"
	m_gameWindow->SetClientSize({ Screen::GetWidth(), Screen::GetHeight() });
	m_gameWindow->Centering();
	::ShowWindow(m_gameWindow->GetHandle(), SW_HIDE);

	// ワーカースレッドを作成
	m_isRunning = true;
	std::thread workerThread(WorkerThreadEntryPoint);

	// メッセージループ
	PumpMessage();

	// ワーカースレッドを終了させて合流
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
	// メッセージ1個分を格納する変数
	MSG msg;

	while (::GetMessage(&msg, nullptr, 0, 0))
	{
		// 仮想キーメッセージを文字メッセージに変換します。
		::TranslateMessage(&msg);

		// メッセージをウィンドウプロシージャに配送します。
		::DispatchMessage(&msg);
	}
}

void Application::AddScenesAndStartupScene()
{
	// シーンの追加
	m_sceneManager = new SceneManager();
	m_sceneManager->AddScene("Title", std::make_unique<TitleScene>());
	m_sceneManager->AddScene("Main", std::make_unique<MainScene>());
	m_sceneManager->ChangeScene("Main");
}

void Application::WorkerThreadEntryPoint()
{
    // ウィンドウハンドル
    HWND hWnd = m_gameWindow->GetHandle();

    // グラフィックシステムの初期化
    Resolution resolution;
    resolution.width = Screen::GetWidth();
    resolution.height = Screen::GetHeight();
    resolution.refreshRate = 60;
    Graphics::StaticConstructor(hWnd, resolution);
    m_gameWindow->SetFullscreen(Screen::IsFullScreen());

    // スプライトレンダラーの初期化
    SpriteRendererSystem::StaticConstructor();

    // メッシュレンダラーシステムの初期化
    MeshRendererSystem::StaticConstructor();

    SkinnedMeshRendererSystem::StaticConstructor();

    // 入力システムの初期化
    InputManager::StaticConstructor(hWnd);

    // 時間の初期化
    Time::StaticConstructor();

    // あらかじめビルド設定に全てのシーンを登録しておく
    AddScenesAndStartupScene();

    // ウィンドウを表示してコントロールを更新する
    ::ShowWindowAsync(hWnd, SW_SHOW);
    ::SetForegroundWindow(hWnd);

    Time::m_startupTimePoint = std::chrono::high_resolution_clock::now();
    Time::m_lastLevelLoadTime = Time::m_startupTimePoint;
    auto previousTimeForFixed = Time::m_startupTimePoint;
    auto previousTimeForVariable = Time::m_startupTimePoint;
    auto previousTimeForFpsCounter = Time::m_startupTimePoint;
    auto previousFrameCount = Time::m_frameCount;

    // ゲームループ
    while (m_isRunning)
    {
        // 現在の時間
        const auto currentTime = std::chrono::high_resolution_clock::now();

        // 前回のフレームからの経過時間
        const auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTimeForVariable);

        // 1フレームあたりの時間
        const std::chrono::microseconds microsecondsPerFrame = std::chrono::microseconds((Time::m_captureFramerate > 0) ? (1000000 / Time::m_captureFramerate) : 16666);

        // 経過時間 >= 1フレームあたりの時間
        if (elapsedTime >= microsecondsPerFrame)
        {
            // 入力システムの更新処理
            InputManager::Update();

            // ゲームの更新 (ゲーム内の時間を少しだけ進める)
            // ゲーム画面をフレームバッファにレンダリング
            FrameResource* currentFrameResource = Graphics::GetCurrentFrameResource();
            currentFrameResource->Update();
            currentFrameResource->Render();

            // 次のフレームの為に上書きしておく
            Time::m_unscaledDeltaTime = std::chrono::duration<float, std::ratio<1, 1>>(elapsedTime).count();
            Time::m_unscaledTime += Time::m_unscaledDeltaTime;
            Time::m_deltaTime = Time::m_unscaledDeltaTime * Time::m_timeScale;
            Time::m_time += Time::m_deltaTime;
            Time::m_frameCount++;
            previousTimeForVariable = currentTime;
        }
    }

    // 全てのフレームリソースにおいてPresent()の完了を待つ。
    Graphics::WaitForCompletionOfPresent();

    // シーンマネージャーの終了処理
    SceneManager::StaticDestructor();

    // インプットマネージャーの終了処理
    InputManager::StaticDestructor();

    // メッシュレンダラーシステムの終了処理
    MeshRendererSystem::StaticDestructor();

    SkinnedMeshRendererSystem::StaticDestructor();

    // スプライトレンダラーシステムの終了処理
    SpriteRendererSystem::StaticDestructor();

    // グラフィックシステムの終了処理
    Graphics::StaticDestructor();
}