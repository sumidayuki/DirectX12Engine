#pragma once

/// <summary>
/// �N���X���FScene
/// �T�v�F�V�[���̊J�n�����E�X�V�����E�`�揈�����s�����N���X�ł��B
/// �p�r�F���̃N���X���p�����邱�ƂŃV�[�����V�[�����쐬�ł��܂��B
/// ���l�F�g�p����ɂ́A���O��Application�ɃV�[����o�^���Ă����K�v������܂��B
/// </summary>
class Scene
{
public:
	bool isStart;

protected:
	World m_world;

	virtual void Start() = 0;	// �V�[�����n�܂�����
	virtual void Update() = 0;	// �V�[���̍X�V����
	virtual void Draw() = 0;	// �V�[���̕`�揈��

public:
	Scene();

	// �f�X�g���N�^
	virtual ~Scene() {}

	void OnStart();

	void OnUpdate();

	void OnDraw();
};