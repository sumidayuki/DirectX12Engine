#include "InputManager.h"
#include "Mouse.h"

// ライブラリをリンクする
#pragma comment(lib, "xinput.lib")


enum class RawDeviceUsagePage
{
    Undefined = 0,
    GenericDesktop = 1,
    SimulationControls = 2,
    VRControls = 3,
    SportControls = 4,
    GameControls = 5,
    GenericDeviceControls = 6,
    KeyboardKeypad = 7,
    LED = 8,
    Button = 9,
};


enum class RawDeviceUsageGenericDesktop
{
    Pointer = 1,
    Mouse = 2,
    Reserved = 3,
    Joystick = 4,
    Gamepad = 5,
    Keyboard = 6,
    Keypad = 7,
    MultiAxisController = 8,
    TabletPCSystemControls = 9,
};


void InputManager::StaticConstructor(HWND hWnd)
{
    m_hInputWnd = CreateInputWindow();

    // 入力デバイス情報
    RAWINPUTDEVICE rawInputDevices[2];
    memset(rawInputDevices, 0, sizeof(rawInputDevices));

    // キーボード
    rawInputDevices[0].usUsagePage = (USHORT)RawDeviceUsagePage::GenericDesktop;
    rawInputDevices[0].usUsage = (USHORT)RawDeviceUsageGenericDesktop::Keyboard;
    rawInputDevices[0].hwndTarget = m_hInputWnd;
    rawInputDevices[0].dwFlags = RIDEV_DEVNOTIFY;

    // マウス
    rawInputDevices[1].usUsagePage = (USHORT)RawDeviceUsagePage::GenericDesktop;
    rawInputDevices[1].usUsage = (USHORT)RawDeviceUsageGenericDesktop::Mouse;
    rawInputDevices[1].hwndTarget = m_hInputWnd;
    rawInputDevices[1].dwFlags = RIDEV_DEVNOTIFY;

    // 入力デバイスの登録
    if (!RegisterRawInputDevices(rawInputDevices, _countof(rawInputDevices), sizeof(RAWINPUTDEVICE)))
    {
    }
    else
    {
    }

    // デバイスの初期化
    Mouse::StaticConstructor(hWnd);
    //Keyboard::StaticConstructor();

    for (int i = 0; i < MaxGamepadCount; i++)
    {
        m_gamepads[i] = new Gamepad((PlayerIndex)i);
    }
}


void InputManager::StaticDestructor()
{
    // 終了処理
    //Keyboard::StaticDestructor();
    Mouse::StaticDestructor();
}


void InputManager::Update()
{
    MSG msg;
    while (::PeekMessage(&msg, m_hInputWnd, 0, 0, PM_REMOVE))
    {
        // 仮想キーメッセージを文字メッセージに変換します。
        ::TranslateMessage(&msg);

        // メッセージをウィンドウプロシージャに配送します。
        ::DispatchMessage(&msg);
    }

    // マウスの入力状態を更新
    Mouse::Update();

    // キーボードの入力状態を更新
    Keyboard::Update();

    // ゲームパッドの入力状態を更新
    for (int i = 0; i < MaxGamepadCount; i++)
    {
        m_gamepads[i]->Update();
    }
}



LRESULT CALLBACK InputManager::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INPUT:
        OnInput((HRAWINPUT)lParam);
        break;

    default:
        return ::DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}


HWND InputManager::CreateInputWindow()
{
    // ウィンドウクラスの登録
    WNDCLASSEX wcex;
    memset(&wcex, 0, sizeof(wcex));                                 // ゼロクリア
    wcex.cbSize = sizeof(wcex);                                     // このWNDCLASSEX型変数のサイズ
    wcex.style = CS_HREDRAW | CS_VREDRAW;						    // クラススタイル (0010 | 0001)
    wcex.lpfnWndProc = &InputManager::WndProc;				        // ウィンドウプロシージャ関数のアドレス
    wcex.hbrBackground = (HBRUSH)::GetStockObject(WHITE_BRUSH);	    // 背景色
    wcex.lpszClassName = "InputManager::CreateInputWindow()";		// ウィンドウクラス名 (任意の文字列)
    if (!::RegisterClassEx(&wcex))
    {
    }
    else
    {
    }

    HWND hWnd = ::CreateWindowEx(WS_EX_APPWINDOW, wcex.lpszClassName, "InputManager", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, nullptr, nullptr, wcex.hInstance, 0);
    if (!hWnd)
    {
        abort();
    }

    return hWnd;
}


void InputManager::OnInput(HRAWINPUT hRawInput)
{
    RAWINPUT rawInputData;
    UINT dwSize = sizeof(RAWINPUT);
    if (!::GetRawInputData(hRawInput, RID_INPUT, &rawInputData, &dwSize, sizeof(RAWINPUTHEADER)))
    {
        assert(0);
    }

    switch (rawInputData.header.dwType)
    {
    case RIM_TYPEMOUSE:
        OnInputMouse(rawInputData.data.mouse);
        break;

    case RIM_TYPEKEYBOARD:
        OnInputKeyboard(rawInputData.data.keyboard);
        break;
    }
}


void InputManager::OnInputMouse(const RAWMOUSE& mouse)
{
    // 左ボタン
    if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_DOWN)
    {
        m_keysLast[(int)KeyCode::Mouse0] = true;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_LEFT_BUTTON_UP)
    {
        m_keysLast[(int)KeyCode::Mouse0] = false;
    }

    // 右ボタン
    if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN)
    {
        m_keysLast[(int)KeyCode::Mouse1] = true;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_RIGHT_BUTTON_UP)
    {
        m_keysLast[(int)KeyCode::Mouse1] = false;
    }

    // 中央ボタン
    if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN)
    {
        m_keysLast[(int)KeyCode::Mouse2] = true;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_MIDDLE_BUTTON_UP)
    {
        m_keysLast[(int)KeyCode::Mouse2] = false;
    }

    // 拡張ボタン1
    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_DOWN)
    {
        m_keysLast[(int)KeyCode::Mouse3] = true;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_4_UP)
    {
        m_keysLast[(int)KeyCode::Mouse3] = false;
    }

    // 拡張ボタン2
    if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_DOWN)
    {
        m_keysLast[(int)KeyCode::Mouse4] = true;
    }
    else if (mouse.usButtonFlags & RI_MOUSE_BUTTON_5_UP)
    {
        m_keysLast[(int)KeyCode::Mouse4] = false;
    }

    // ホイール
    if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
    {

    }
}


void InputManager::OnInputKeyboard(const RAWKEYBOARD& keyboard)
{
    m_keysLast[keyboard.VKey] = true;



}





