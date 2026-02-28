#pragma once

/// <summary>
/// クラス名：SceneManager
/// 概要：シーン全体を管理するクラスです。
/// 用途：現在のシーンの更新・描画・切り替えを行います。
/// 備考：このクラスは静的クラスです。
/// </summary>
class SceneManager
{
	friend class Application;
	friend class FrameResource;

private:
	static inline Scene* m_currentScene = nullptr;

	static inline std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;

	// 遅延シーン切り替え用
	static inline std::string m_pendingSceneName;

public:
	/// <summary>
	/// 新しいシーンを登録します。
	/// </summary>
	static void AddScene(const std::string& name, std::unique_ptr<Scene> scene);

	/// <summary>
	/// 指定したシーンに変更します。
	/// 実際の切り替えは次のUpdateの冒頭で行われます。
	/// </summary>
	static void ChangeScene(const std::string& name);

	static Scene* GetCurrentScene() { return m_currentScene; }

private:
	static void StaticDestructor();

	/// <summary>
	/// 保留中のシーン切り替えを実行します。
	/// </summary>
	static void ProcessPendingSceneChange();

	static void Start();

	static void BeginFrame(UINT frameIndex);

	static void Update();

	static void Draw();
};