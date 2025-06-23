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
        memset(&wcex, 0, sizeof(wcex));                                 // �[���N���A
        wcex.cbSize = sizeof(wcex);                                     // ����WNDCLASSEX�^�ϐ��̃T�C�Y
        wcex.style = CS_HREDRAW | CS_VREDRAW;                           // �N���X�X�^�C���i0010 | 0001�j
        wcex.lpfnWndProc = &NativeWindow::NativeWndProc;                // �E�B���h�E�v���V�[�V���֐��̃A�h���X
        wcex.cbClsExtra = 0;                                            // �E�B���h�E�N���X�p�̊g���̈�T�C�Y�i�P�ʂ̓o�C�g�j
        wcex.cbWndExtra = sizeof(LONG_PTR);                             // �E�B���h�E�p�̊g���̈�T�C�Y�i�P�ʂ̓o�C�g�j
        wcex.hInstance = ::GetModuleHandle(nullptr);                    // �A�v���P�[�V�����̎��ʔԍ�
        wcex.hIcon = ::LoadIcon(nullptr, IDI_APPLICATION);              // �A�C�R��
        wcex.hIconSm = nullptr;                                         // ���A�C�R��
        wcex.hCursor = ::LoadCursor(nullptr, IDC_ARROW);                // �}�E�X�J�[�\��
        wcex.hbrBackground = (HBRUSH)::GetStockObject(BLACK_BRUSH);     // �w�i�F
        wcex.lpszMenuName = nullptr;                                    // ���j���[���\�[�X���g�p����ꍇ�͂��̎��ʖ��܂���ID�i����͖��g�p�j
        wcex.lpszClassName = "Windows::NativeWindow";                   // �E�B���h�E�N���X���i�C�ӂ̕�����j

        if (::RegisterClassEx(&wcex))
        {

        }
        else
        {

        }

        const DWORD dwStyle = WS_OVERLAPPEDWINDOW;      // �E�B���h�E�X�^�C��
        const DWORD dwExStyle = WS_EX_APPWINDOW;        // �g���E�B���h�E�X�^�C��
        const HMENU hMenu = nullptr;                    // ���j���[�o�[�̍쐬

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
        // �E�B���h�E�X�^�C�����擾
        const DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);

        // �g���E�B���h�E�X�^�C�����擾
        const DWORD dwExStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);

        // �N���C�A���g�̈�T�C�Y����ɃE�B���h�E�T�C�Y���v�Z����
        RECT windowRect;
        windowRect.left = 0;
        windowRect.right = size.x;
        windowRect.top = 0;
        windowRect.bottom = size.y;
        ::AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);

        // �E�B���h�E�T�C�Y���v�Z����
        Vector2Int windowSize;
        windowSize.x = windowRect.right - windowRect.left;
        windowSize.y = windowRect.bottom - windowRect.top;
        this->SetWindowSize(windowSize);
    }

    void NativeWindow::SetWindowSize(const Vector2Int& size)
    {
        // �E�B���h�E�T�C�Y��ύX����
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
        // �E�B���h�E�ʒu��ύX����
        ::SetWindowPos(m_hWnd, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE);
    }

    void NativeWindow::Centering()
    {
        // �f�X�N�g�b�v��ʂ̒����ɗ���悤�ɃE�B���h�E�ʒu���v�Z���ăE�B���h�E�ʒu��ύX����
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