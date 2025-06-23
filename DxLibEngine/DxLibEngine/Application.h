#pragma once

/// <summary>
/// アプリケーションクラスです。
/// アプリケーションの起動・更新・終了を行います。
/// </summary>
class Application
{
private:
	// ゲーム画面を表示するためのウィンドウ
 	static inline DxWindow* m_gameWindow;

	// ワーカースレッド実行中は true
	static inline bool m_isRunning;

	static inline SceneManager* m_sceneManager;

public:
	// アプリケーションを起動します。
	// この関数はアプリケーションが終了するまで制御を返しません。
	static void Run();

private:
	static void Update();

	static void Draw();

	// ワーカースレッドのエントリーポイント関数
	//static void WorkerThreadEntryPoint();

	// メッセージキュー内のメッセージを取得して処理します。
	//static void PumpMessage();
};