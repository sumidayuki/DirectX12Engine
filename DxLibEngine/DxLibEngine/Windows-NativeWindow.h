#pragma once

namespace Windows
{
	// ウィンドウメッセージ
	struct Message
	{
		HWND hWnd;
		LPARAM lParam;
		WPARAM wParam;
		UINT message;
		LRESULT result;
	};

	class NativeWindow
	{
	private:
		HWND m_hWnd;

	public:
		NativeWindow();

		virtual ~NativeWindow() = default;

		HWND GetHandle() { return m_hWnd; }

		void SetClientSize(const Vector2Int& size);

		Vector2Int GetClientSize() const;

		void SetWindowSize(const Vector2Int& size);

		Vector2Int GetWindowSize() const;

		Vector2Int GetDesktopSize() const;

		void SetPosition(const Vector2Int& position);

		void Centering();

		void Finalize();

	protected:
		void DefWndProc(Message& msg);

		virtual void CreateHandle(const char* windowName);

		virtual void WndProc(Message& msg);

	private:
		static LRESULT NativeWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}