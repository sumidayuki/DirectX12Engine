//#pragma once
//
///// <summary>
///// クラス名：DxWindow
///// 概要：DxLibを使用したWindow作成を行います。
///// 用途：Window制御の全般を行います。
///// </summary>
//class DxWindow
//{
//private:
//	HWND m_hWnd;
//
//public:
//	// コンストラクタ
//	DxWindow();
//
//	// デストラクタ
//	virtual ~DxWindow() = default;
//
//	// HWND型のハンドルを取得します。
//	HWND GetHandle() { return m_hWnd; }
//
//	// クライアントサイズを設定します。Vector2Int型で指定してください。
//	void SetClientSize(const Vector2Int& size);
//
//	// クライアントサイズを取得します。戻り値はVector2Int型です。
//	Vector2Int GetClientSize() const;
//
//	void SetWindowSize(const Vector2Int& size);
//
//	Vector2Int GetWindowSize() const;
//
//	Vector2Int GetDesktopSize() const;
//
//	void SetPosition(const Vector2Int& position);
//
//	// ウィンドウを画面の中央に表示します。
//	void Centering();
//
//protected:
//	void CreateHandle(const char* windowName);
//};