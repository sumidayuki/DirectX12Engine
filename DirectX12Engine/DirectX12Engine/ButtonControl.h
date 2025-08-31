#pragma once

class ButtonControl
{
private:
	static inline const float DeltaTime = 1.0f / 60;

private:
	// ���s�[�g�̐i����\���񋓌^
	enum class Progress
	{
		Released,			// ������Ă��Ȃ�
		FirstPressed,		// �ŏ��̌��o
		FirstInterval,		// �ŏ��̌��o��̖����o���Ԓ�
		RepeatPressed,		// 2��ڈȍ~�̌��o
		RepeatInterval,		// 2��ڈȍ~�̌��o��̖����o���Ԓ�
	};

private:
	bool m_current;				// ���݂̃t���[���ŉ�����Ă��邩�H
	bool m_previous;			// 1�t���[���O����͉�����Ă������H	
	float m_firstInterval;		// ���s�[�g�̍ŏ��̖����o���ԁi�P�ʂ͕b�j
	float m_repeatInterval;		// ���s�[�g��2��ڈȍ~�̖����o���ԁi�P�ʂ͕b�j
	float m_elapsedTime;		// ���s�[�g�̑O��̌��o����̌o�ߎ��ԁi�P�ʂ͕b�j
	Progress m_progress;		// ���s�[�g�̐i����

public:
	// �R���X�g���N�^
	ButtonControl();

	// ���͏�Ԃ��X�V���܂��B
	void Update(bool current);

	// ������Ă���ꍇ�� true ��Ԃ��܂��B
	bool IsPressed() const { return m_current; }

	// ���݂̃t���[���ŉ����ꂽ�ꍇ�� true ��Ԃ��܂��B
	bool WasPressedThisFrame() const { return m_current && !m_previous; }

	// ���݂̃t���[���ŗ����ꂽ�ꍇ�� true ��Ԃ��܂��B
	bool WasReleasedThisFrame() const { return !m_current && m_previous; }

	// ���s�[�g���̏ꍇ�� true ��Ԃ��܂��B
	bool IsRepeated() const;
};