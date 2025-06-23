#include "Windows-NativeWindow.h"

namespace Windows
{
    NativeWindow::NativeWindow()
        : m_hWnd(nullptr)
    {
        CreateHandle("");
    }

    void NativeWindow::CreateHandle(const char* windowName)
    {
        WNDCLASSEX wcex;
        memset(&wcex, 0, sizeof(wcex));                                 // ゼロクリア
        wcex.cbSize = sizeof(wcex);                                     // このWNDCLASSEX型変数のサイズ
        wcex.style = CS_HREDRAW | CS_VREDRAW;                           // クラススタイル（0010 | 0001）
        wcex.lpfnWndProc = &NativeWindow::NativeWndProc;                // ウィンドウプロシーシャ関数のアドレス
        wcex.cbClsExtra = 0;                                            // ウィンドウクラス用の拡張領域サイズ（単位はバイト）
        wcex.cbWndExtra = sizeof(LONG_PTR);                             // ウィンドウ用の拡張領域サイズ（単位はバイト）
        wcex.hInstance = ::GetModuleHandle(nullptr);                    // アプリケーションの識別番号
        wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);              // アイコン
        wcex.hIconSm = nullptr;                                         // 小アイコン
        wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);                // マウスカーソル
        wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);     // 背景色
        wcex.lpszMenuName = nullptr;                                    // メニューリソースを使用する場合はその識別名またはID（今回は未使用）
        wcex.lpszClassName = "Windows::NativeWindow";                   // ウィンドウクラス名（任意の文字列）

        if (::RegisterClassEx(&wcex))
        {

        }
        else
        {

        }

        const DWORD dwStyle = WS_OVERLAPPEDWINDOW;      // ウィンドウスタイル
        const DWORD dwExStyle = WS_EX_APPWINDOW;        // 拡張ウィンドウスタイル
        const HMENU hMenu = nullptr;                    // メニューバーの作成

        m_hWnd = ::CreateWindowEx
        (
            dwExStyle,
            wcex.lpszClassName,
            windowName,
            dwStyle,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            nullptr,
            hMenu,
            wcex.hInstance,
            0
        );

        if (!m_hWnd)
        {
            abort();
        }
        else
        {
            ::SetWindowLongPtr(m_hWnd, GWLP_USERDATA, (LONG_PTR)this);
        }
    }

    void NativeWindow::WndProc(Message& msg)
    {
        switch (msg.message)
        {
#if 0
        case WM_MOUSEWHEEL:
        {
            const int delta = GET_WHEEL_DELTA_WPARAM(msg.wParam);
            ::Mouse::OnWheelMove(delta / (float)WHEEL_DELTA);
        }
        break;
#endif 
        case WM_DESTROY:
            ::PostQuitMessage(0);
            break;

        default:
            return this->DefWndProc(msg);
        }

        msg.result = 0;
    }

    LRESULT NativeWindow::NativeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
    {
        if (NativeWindow* nativeWindow = (NativeWindow*)GetWindowLongPtr(hWnd, GWLP_USERDATA))
        {
            Message msg;
            msg.hWnd = hWnd;
            msg.message = message;
            msg.wParam = wParam;
            msg.lParam = lParam;
            msg.result = 0;
            nativeWindow->DefWndProc(msg);
            return msg.result;
        }
        else
        {
            return ::DefWindowProc(hWnd, message, wParam, lParam);
        }
    }

    void NativeWindow::DefWndProc(Message& msg)
    {
        msg.result = ::DefWindowProc(msg.hWnd, msg.message, msg.wParam, msg.lParam);
    }

    void NativeWindow::SetClientSize(const Vector2Int& size)
    {
        // ウィンドウスタイルを取得
        const DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);

        // 拡張ウィンドウスタイルを取得
        const DWORD dwExStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);

        // クライアント領域サイズを基にウィンドウサイズを計算する
        RECT windowRect;
        windowRect.left = 0;
        windowRect.right = size.x;
        windowRect.top = 0;
        windowRect.bottom = size.y;
        ::AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

        // ウィンドウサイズを計算する
        Vector2Int windowSize;
        windowSize.x = windowRect.right - windowRect.left;
        windowSize.y = windowRect.bottom - windowRect.top;
        this->SetWindowSize(windowSize);
    }

    void NativeWindow::SetWindowSize(const Vector2Int& size)
    {
        // ウィンドウサイズを変更する
        ::SetWindowPos(m_hWnd, nullptr, 0, 0, size.x, size.y, SWP_NOMOVE);
    }

    Vector2Int NativeWindow::GetClientSize() const
    {
        RECT rect;
        ::GetClientRect(m_hWnd, &rect);
        return Vector2Int(rect.right - rect.left, rect.bottom - rect.top);
    }

    Vector2Int NativeWindow::GetWindowSize() const
    {
        RECT rect;
        ::GetWindowRect(m_hWnd, &rect);
        return Vector2Int(rect.right - rect.left, rect.bottom - rect.top);
    }

    Vector2Int NativeWindow::GetDesktopSize() const
    {
        return Vector2Int(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
    }

    void NativeWindow::SetPosition(const Vector2Int& position)
    {
        // ウィンドウ位置を変更する
        ::SetWindowPos(m_hWnd, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE);
    }

    void NativeWindow::Centering()
    {
        // デスクトップ画面の中央に来るようにウィンドウ位置を計算してウィンドウ位置を変更する
        this->SetPosition((GetDesktopSize() - GetWindowSize()) / 2);
    }

    void NativeWindow::Finalize()
    {
        if (m_hWnd)
        {
            DestroyWindow(m_hWnd);
            m_hWnd = nullptr;
        }
    }
}