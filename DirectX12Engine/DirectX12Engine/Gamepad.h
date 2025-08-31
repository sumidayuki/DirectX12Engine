#pragma once
#include "InputControls.h"

//----------------------------------------------------------------------------------------------------------------------------------------------------
// �v���C���[�C���f�b�N�X
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class PlayerIndex
{
    One,
    Two,
    Three,
    Four,
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// �Q�[���p�b�h�{�^���񋓌^
//----------------------------------------------------------------------------------------------------------------------------------------------------
enum class GamepadButton
{
    South,                  // ���A�N�V�����{�^�� (�uA�{�^���v�܂��́u�~�{�^���v)
    East,                   // �E�A�N�V�����{�^�� (�uB�{�^���v�܂��́u�Z�{�^���v)
    West,                   // ���A�N�V�����{�^�� (�uX�{�^���v�܂��́u���{�^���v)
    North,                  // ��A�N�V�����{�^�� (�uY�{�^���v�܂��́u���{�^���v)

    A = South,    // XBox�R���g���[���[�́uA�{�^���v
    B = East,     // XBox�R���g���[���[�́uB�{�^���v
    X = West,     // XBox�R���g���[���[�́uX�{�^���v
    Y = North,    // XBox�R���g���[���[�́uY�{�^���v

    Cross = South,    // PS�R���g���[���[�́u�~�{�^���v
    Circle = East,     // PS�R���g���[���[�́u�Z�{�^���v
    Square = West,     // PS�R���g���[���[�́u���{�^���v
    Triangle = North,    // PS�R���g���[���[�́u���{�^���v

    DpadDown,               // �\���L�[�̉�
    DpadRight,              // �\���L�[�̉E
    DpadLeft,               // �\���L�[�̍�
    DpadUp,                 // �\���L�[�̏�

    LeftShoulder,           // ���V�����_�[           (�uLB�{�^���v�܂��́uL1�{�^���v)
    LeftTrigger,            // ���g���K�[             (�uLT�{�^���v�܂��́uL2�{�^���v)
    LeftStickButton,        // ���X�e�B�b�N�{�^��     (�uL�{�^���v �܂��́uL3�{�^���v)
    RightShoulder,          // �E�V�����_�[           (�uRB�{�^���v�܂��́uR1�{�^���v)
    RightTrigger,           // �E�g���K�[             (�uRT�{�^���v�܂��́uR2�{�^���v)
    RightStickButton,       // �E�X�e�B�b�N�{�^��     (�uR�{�^���v �܂��́uR3�{�^���v)

    Select,                 // �Z���N�g�{�^��
    Start,                  // �X�^�[�g�{�^��

    NumberOfButtons,        // ���{�^����
};


//----------------------------------------------------------------------------------------------------------------------------------------------------
// �Q�[���p�b�h����̓��͂������N���X
// 
//  �E�Q�[���p�b�h�͕����ڑ������\��������̂Ŕ�ÓI�N���X���]�܂����B
// 
//----------------------------------------------------------------------------------------------------------------------------------------------------
class Gamepad
{
public:
    // ���{�^����
    static inline const int NumberOfButtons = (int)GamepadButton::NumberOfButtons;

    // �Ō�Ɏg�p���ꂽ/�ڑ����ꂽ�Q�[���p�b�h
    static Gamepad* m_current;

private:
    // �v���C���[�C���f�b�N�X
    PlayerIndex m_playerIndex;

    // �{�^���R���g���[���z��
    ButtonControl m_buttons[NumberOfButtons];

    // �X�e�B�b�N�R���g���[��
    StickControl m_leftStick;
    StickControl m_rightStick;

    bool m_isConnected;

public:
    // �f�t�H���g�R���X�g���N�^
    Gamepad(PlayerIndex playerIndex);

    // �Q�[���p�b�h�̓��͏�Ԃ��X�V���܂��B
    void Update();

    // �Q�[���p�b�h���ڑ�����Ă���ꍇ�� true ��Ԃ��܂��B
    bool IsConnected() const { return m_isConnected; }

    // �X�e�B�b�N
    const StickControl& LeftStick() const { return m_leftStick; }
    const StickControl& RightStick() const { return m_rightStick; }

    // �e��{�^��
    const ButtonControl& A() const { return m_buttons[(int)GamepadButton::A]; }
    const ButtonControl& B() const { return m_buttons[(int)GamepadButton::B]; }
    const ButtonControl& X() const { return m_buttons[(int)GamepadButton::X]; }
    const ButtonControl& Y() const { return m_buttons[(int)GamepadButton::Y]; }
    const ButtonControl& South() const { return m_buttons[(int)GamepadButton::South]; }
    const ButtonControl& East() const { return m_buttons[(int)GamepadButton::East]; }
    const ButtonControl& West() const { return m_buttons[(int)GamepadButton::West]; }
    const ButtonControl& North() const { return m_buttons[(int)GamepadButton::North]; }
    const ButtonControl& DpadDown() const { return m_buttons[(int)GamepadButton::DpadDown]; }
    const ButtonControl& DpadRight() const { return m_buttons[(int)GamepadButton::DpadRight]; }
    const ButtonControl& DpadLeft() const { return m_buttons[(int)GamepadButton::DpadLeft]; }
    const ButtonControl& DpadUp() const { return m_buttons[(int)GamepadButton::DpadUp]; }
    const ButtonControl& LeftShoulder() const { return m_buttons[(int)GamepadButton::LeftShoulder]; }
    const ButtonControl& LeftTrigger() const { return m_buttons[(int)GamepadButton::LeftTrigger]; }
    const ButtonControl& LeftStickButton() const { return m_buttons[(int)GamepadButton::LeftStickButton]; }
    const ButtonControl& RightShoulder() const { return m_buttons[(int)GamepadButton::RightShoulder]; }
    const ButtonControl& RightTrigger() const { return m_buttons[(int)GamepadButton::RightTrigger]; }
    const ButtonControl& RightStickButton() const { return m_buttons[(int)GamepadButton::RightStickButton]; }
    const ButtonControl& SelectButton() const { return m_buttons[(int)GamepadButton::Select]; }
    const ButtonControl& StartButton() const { return m_buttons[(int)GamepadButton::Start]; }

    // �w�肵���{�^���̏�Ԃ��擾���܂��B
    const ButtonControl& GetButton(GamepadButton gamepadButton) const;
};
