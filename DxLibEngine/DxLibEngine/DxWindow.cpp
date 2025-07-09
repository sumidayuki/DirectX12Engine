//#include "DxWindow.h"
//
//DxWindow::DxWindow()
//	: m_hWnd(nullptr)
//{
//	CreateHandle("自作エンジン");
//}
//
//void DxWindow::CreateHandle(const char* windowName)
//{
//	SetGraphMode(1280, 720, 32);
//
//	ChangeWindowMode(!Screen::IsFullScreen());
//
//	SetUseZBuffer3D(TRUE); // Zバッファ有効
//	SetWriteZBuffer3D(TRUE);
//
//	if (Screen::IsFullScreen()) 
//	{
//		SetFullScreenResolutionMode(DX_FSRESOLUTIONMODE_BORDERLESS_WINDOW);
//	}
//
//	if (DxLib_Init() == -1)
//	{
//		throw std::runtime_error("DxLib_Init failed");
//	}
//
//	m_hWnd = GetMainWindowHandle();
//
//	SetMainWindowText(windowName);
//
//	SetDrawScreen(DX_SCREEN_BACK);
//}
//
//
//void DxWindow::SetClientSize(const Vector2Int& size)
//{
//	const DWORD dwStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_STYLE);
//	const DWORD dwExStyle = (DWORD)::GetWindowLongPtr(m_hWnd, GWL_EXSTYLE);
//
//	RECT windowRect;
//	windowRect.left = 0;
//	windowRect.right = size.x;
//	windowRect.top = 0;
//	windowRect.bottom = size.y;
//	::AdjustWindowRectEx(&windowRect, dwStyle, FALSE, dwExStyle);
//
//	Vector2Int windowSize;
//	windowSize.x = windowRect.right - windowRect.left;
//	windowSize.y = windowRect.bottom - windowRect.top;
//	this->SetWindowSize(windowSize);
//}
//
//Vector2Int DxWindow::GetClientSize() const
//{
//	RECT rect;
//	::GetClientRect(m_hWnd, &rect);
//	return Vector2Int(rect.right - rect.left, rect.bottom - rect.top);
//}
//
//void DxWindow::SetWindowSize(const Vector2Int& size)
//{
//	SetGraphMode(size.x, size.y, 32);
//}
//
//Vector2Int DxWindow::GetWindowSize() const
//{
//	RECT rect;
//	::GetWindowRect(m_hWnd, &rect);
//	return Vector2Int(rect.right - rect.left, rect.bottom - rect.top);
//}
//
//Vector2Int DxWindow::GetDesktopSize() const
//{
//	return Vector2Int(::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
//}
//
//void DxWindow::SetPosition(const Vector2Int& position)
//{
//	::SetWindowPos(m_hWnd, nullptr, position.x, position.y, 0, 0, SWP_NOSIZE);
//}
//
//void DxWindow::Centering()
//{
//	this->SetPosition((GetDesktopSize() - GetWindowSize()) / 2);
//}