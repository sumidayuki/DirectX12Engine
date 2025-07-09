#pragma once

/// <summary>
/// アプリケーションクラスです。
/// アプリケーションの起動・更新・終了を行います。
/// </summary>
class Application
{
private:
	// ゲーム画面を表示するためのウィンドウ
 	static inline Windows::NativeWindow* m_gameWindow;

	// ワーカースレッド実行中は true
	static inline bool m_isRunning;

	static inline SceneManager* m_sceneManager;

public:
	// アプリケーションを起動します。
	// この関数はアプリケーションが終了するまで制御を返しません。
	static void Run();

	// ゲームウィンドウを取得します。
	static Windows::NativeWindow* GetGameWindow() { return m_gameWindow; }

private:
	//static void Update();

	//static void Draw();

	// シーンの追加と起動シーンの設定
	static void AddScenesAndStartupScene();

	// ワーカースレッドのエントリーポイント関数
	static void WorkerThreadEntryPoint();

	// メッセージキュー内のメッセージを取得して処理します。
	static void PumpMessage();
};