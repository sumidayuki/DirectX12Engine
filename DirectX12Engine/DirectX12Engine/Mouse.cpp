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
// 速度 = 位置の変化量 ÷ 経過時間
//      = (現在の位置 - 前回の位置) ÷ (現在の時間 - 前回の時間)
//
// 
// 加速度 = 速度の変化量 ÷ 経過時間
//        = (現在の速度 - 前回の速度) ÷ (現在の時間 - 前回の時間)
// 
//---------------------------------------------------------------------------------------------------------------------------------
void Mouse::Update()
{
    // Windows APIを利用してマウスカーソルの位置を取得する
    POINT cursorPos;
    GetCursorPos(&cursorPos);

    // スクリーン空間での位置、速度、加速度を計算する
    m_motionInScreen.position[1] = m_motionInScreen.position[0];
    m_motionInScreen.position[0] = { cursorPos.x, cursorPos.y };
    m_motionInScreen.velocity[1] = m_motionInScreen.velocity[0];
    m_motionInScreen.velocity[0].x = (float)(m_motionInScreen.position[0].x - m_motionInScreen.position[1].x);
    m_motionInScreen.velocity[0].y = (float)(m_motionInScreen.position[0].y - m_motionInScreen.position[1].y);
    m_motionInScreen.acceleration = m_motionInScreen.velocity[0] - m_motionInScreen.velocity[1];

    // スクリーン空間での位置をクライアント空間での位置に変換する
    ScreenToClient(m_hWnd, &cursorPos);

    // クライアント空間での位置、速度、加速度を計算する
    m_motionInClient.position[1] = m_motionInClient.position[0];
    m_motionInClient.position[0] = { cursorPos.x, cursorPos.y };
    m_motionInClient.velocity[1] = m_motionInClient.velocity[0];
    m_motionInClient.velocity[0].x = (float)(m_motionInClient.position[0].x - m_motionInClient.position[1].x);
    m_motionInClient.velocity[0].y = (float)(m_motionInClient.position[0].y - m_motionInClient.position[1].y);
    m_motionInClient.acceleration = m_motionInClient.velocity[0] - m_motionInClient.velocity[1];

    // Windows APIを利用してマウスボタンの状態を取得する
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
