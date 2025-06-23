#pragma once

/// <summary>
/// �N���X���FSceneManager
/// �T�v�F�V�[���S�̂��Ǘ�����N���X�ł��B
/// �p�r�F���݂̃V�[���̍X�V�E�`��E�؂�ւ����s���܂��B
/// ���l�F���̃N���X�͐ÓI�N���X�ł��B
/// </summary>
class SceneManager
{
private:
	static inline Scene* m_currentScene = nullptr;

	static inline std::unordered_map<std::string, std::unique_ptr<Scene>> m_scenes;

public:
	/// <summary>
	/// �V�����V�[����o�^���܂��B
	/// </summary>
	/// <param name="name">�o�^����V�[����</param>
	/// <param name="scene">�o�^����V�[��</param>
	static void AddScene(const std::string& name, std::unique_ptr<Scene> scene);

	/// <summary>
	/// �w�肵���V�[���ɕύX���܂��B
	/// </summary>
	/// <param name="name">�ύX��̃V�[����</param>
	static void ChangeScene(const std::string& name);

	/// <summary>
	/// ���݂̃V�[���̊J�n�����ł��B
	/// </summary>
	static void Start();

	/// <summary>
	/// ���݂̃V�[���̍X�V�����ł��B
	/// </summary>
	static void Update();

	/// <summary>
	/// ���݂̃V�[���̕`�揈���ł��B
	/// </summary>
	static void Draw();
};