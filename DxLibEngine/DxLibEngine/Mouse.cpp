#include "Mouse.h"


void Mouse::StaticConstructor(HWND hWnd)
{
    m_hWnd = hWnd;
    m_motionInScreen.position[0] = Vector2Int::zero;
    m_motionInScreen.position[1] = Vector2Int::zero;
    m_motionInScreen.velocity[0] = Vector2::zero;
    m_motionInScreen.velocity[1] = Vector2::zero;
    m_motionInScreen.acceleration = Vector2::zero;
    m_motionInClient.position[0] = Vector2Int::zero;
    m_motionInClient.position[1] = Vector2Int::zero;
    m_motionInClient.velocity[0] = Vector2::zero;
    m_motionInClient.velocity[1] = Vector2::zero;
    m_motionInClient.acceleration = Vector2::zero;
    m_wheelVelocity = 0.0f;
}


void Mouse::StaticDestructor()
{

}


//---------------------------------------------------------------------------------------------------------------------------------
// 
// ���x = �ʒu�̕ω��� �� �o�ߎ���
//      = (���݂̈ʒu - �O��̈ʒu) �� (���݂̎��� - �O��̎���)
//
// 
// �����x = ���x�̕ω��� �� �o�ߎ���
//        = (���݂̑��x - �O��̑��x) �� (���݂̎��� - �O��̎���)
// 
//---------------------------------------------------------------------------------------------------------------------------------
void Mouse::Update()
{
    // Windows API�𗘗p���ă}�E�X�J�[�\���̈ʒu���擾����
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // �X�N���[����Ԃł̈ʒu�A���x�A�����x���v�Z����
    m_motionInScreen.position[1] = m_motionInScreen.position[0];
    m_motionInScreen.position[0] = { cursorPos.x, cursorPos.y };
    m_motionInScreen.velocity[1] = m_motionInScreen.velocity[0];
    m_motionInScreen.velocity[0].x = (float)(m_motionInScreen.position[0].x - m_motionInScreen.position[1].x);
    m_motionInScreen.velocity[0].y = (float)(m_motionInScreen.position[0].y - m_motionInScreen.position[1].y);
    m_motionInScreen.acceleration = m_motionInScreen.velocity[0] - m_motionInScreen.velocity[1];

    // �X�N���[����Ԃł̈ʒu���N���C�A���g��Ԃł̈ʒu�ɕϊ�����
    ScreenToClient(m_hWnd, &cursorPos);

    // �N���C�A���g��Ԃł̈ʒu�A���x�A�����x���v�Z����
    m_motionInClient.position[1] = m_motionInClient.position[0];
    m_motionInClient.position[0] = { cursorPos.x, cursorPos.y };
    m_motionInClient.velocity[1] = m_motionInClient.velocity[0];
    m_motionInClient.velocity[0].x = (float)(m_motionInClient.position[0].x - m_motionInClient.position[1].x);
    m_motionInClient.velocity[0].y = (float)(m_motionInClient.position[0].y - m_motionInClient.position[1].y);
    m_motionInClient.acceleration = m_motionInClient.velocity[0] - m_motionInClient.velocity[1];

    // Windows API�𗘗p���ă}�E�X�{�^���̏�Ԃ��擾����
    m_buttonStates[(int)MouseButton::Left].Update((GetAsyncKeyState(VK_LBUTTON) & 0x8000) != 0);
    m_buttonStates[(int)MouseButton::Right].Update((GetAsyncKeyState(VK_RBUTTON) & 0x8000) != 0);
    m_buttonStates[(int)MouseButton::Middle].Update((GetAsyncKeyState(VK_MBUTTON) & 0x8000) != 0);
    m_buttonStates[(int)MouseButton::X1].Update((GetAsyncKeyState(VK_XBUTTON1) & 0x8000) != 0);
    m_buttonStates[(int)MouseButton::X2].Update((GetAsyncKeyState(VK_XBUTTON2) & 0x8000) != 0);
}


ButtonControl Mouse::GetButtonState(MouseButton button)
{
    return m_buttonStates[(int)button];
}
