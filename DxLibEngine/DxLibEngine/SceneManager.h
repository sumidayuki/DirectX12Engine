#pragma once

/// <summary>
/// クラス名：SceneManager
/// 概要：シーン全体を管理するクラスです。
/// 用途：現在のシーンの更新・描画・切り替えを行います。
/// 備考：このクラスは静的クラスです。
/// </summary>
class SceneManager
{
private:
	static inline Scene* m_currentScene = nullptr;

	static inline std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;

public:
	/// <summary>
	/// 新しいシーンを登録します。
	/// </summary>
	/// <param name="name">登録するシーン名</param>
	/// <param name="scene">登録するシーン</param>
	static void AddScene(const std::string& name, std::unique_ptr<Scene> scene);

	/// <summary>
	/// 指定したシーンに変更します。
	/// </summary>
	/// <param name="name">変更先のシーン名</param>
	static void ChangeScene(const std::string& name);

	/// <summary>
	/// 現在のシーンの開始処理です。
	/// </summary>
	static void Start();

	/// <summary>
	/// 現在のシーンの更新処理です。
	/// </summary>
	static void Update();

	/// <summary>
	/// 現在のシーンの描画処理です。
	/// </summary>
	static void Draw();
};