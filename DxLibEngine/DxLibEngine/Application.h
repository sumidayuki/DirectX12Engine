#pragma once

/// <summary>
/// �A�v���P�[�V�����N���X�ł��B
/// �A�v���P�[�V�����̋N���E�X�V�E�I�����s���܂��B
/// </summary>
class Application
{
private:
	// �Q�[����ʂ�\�����邽�߂̃E�B���h�E
 	static inline DxWindow* m_gameWindow;

	// ���[�J�[�X���b�h���s���� true
	static inline bool m_isRunning;

	static inline SceneManager* m_sceneManager;

public:
	// �A�v���P�[�V�������N�����܂��B
	// ���̊֐��̓A�v���P�[�V�������I������܂Ő����Ԃ��܂���B
	static void Run();

private:
	static void Update();

	static void Draw();

	// ���[�J�[�X���b�h�̃G���g���[�|�C���g�֐�
	//static void WorkerThreadEntryPoint();

	// ���b�Z�[�W�L���[���̃��b�Z�[�W���擾���ď������܂��B
	//static void PumpMessage();
};