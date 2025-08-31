#pragma once
#include "ButtonControl.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
// �}�E�X�{�^����\���񋓌^
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class MouseButton
{
    Left,       // ���{�^��
    Right,      // �E�{�^��
    Middle,     // �����{�^�� (�`���g�z�C�[���̉�������)
    X1,         // �g���{�^������1
    X2,         // �g���{�^������2
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// �}�E�X����̓��͂������ÓI�N���X
//----------------------------------------------------------------------------------------------------------------------------------------------------
class Mouse
{
    friend class InputManager;
    friend class Windows::NativeWindow;

private:
    // �E�B���h�E�n���h�� (�u�ǂ̃E�B���h�E�H�v����\�������l)
    static inline HWND m_hWnd;

    // �J�[�\���̓�����\���\����
    struct CursorMotion
    {
        Vector2Int  position[2];    // �ʒu ([0]:����  [1]:�O��)
        Vector2     velocity[2];    // ���x ([0]:����  [1]:�O��)
        Vector2     acceleration;   // �����x
    };

    // �J�[�\���̓���
    static inline CursorMotion  m_motionInScreen;
    static inline CursorMotion  m_motionInClient;

    // �z�C�[�����x
    static inline float m_wheelVelocity;

    // �e�{�^���̏��
    static inline ButtonControl m_buttonStates[5];

private:
    // �}�E�X���̓V�X�e�������������܂��B
    static void StaticConstructor(HWND hWnd);

    // �}�E�X���̓V�X�e�����I�����܂��B
    static void StaticDestructor();

    // �}�E�X�̓��͏����X�V���܂��B(���t���[���Ăяo�����I)
    static void Update();

    // �z�C�[�����ړ������ۂ̃C�x���g�֐��ł��B
    // ���̊֐��̓E�B���h�E�v���V�[�W������Ăяo����܂��B
    // 
    // ������1�P�ʈړ������ +1.0�A
    // ��O����1�P�ʈړ������ -1.0 �̂悤�Ȓl���n����܂��B
    static void OnWheelMove(float wheelVelocity) { m_wheelVelocity = wheelVelocity; }

public:
    // �}�E�X�J�[�\���̈ʒu���擾���܂��B(�X�N���[�����W)
    static Vector2Int GetPositionInScreen() { return m_motionInScreen.position[0]; }

    // �}�E�X�J�[�\���̈ʒu���擾���܂��B(�N���C�A���g���W)
    static Vector2Int GetPosition() { return m_motionInClient.position[0]; }

    // �}�E�X�J�[�\���̑��x���擾���܂��B
    static Vector2 GetVelocity() { return m_motionInClient.velocity[0]; }

    // �}�E�X�J�[�\���̉����x���擾���܂��B
    static Vector2 GetAcceleration() { return m_motionInClient.acceleration; }

    // �z�C�[�����x���擾���܂��B
    static float GetWheelVelocity() { return m_wheelVelocity; }

    // �w�肵���{�^���̏�Ԃ��擾���܂��B
    static ButtonControl GetButtonState(MouseButton button);
};

