//#pragma once
//
///// <summary>
///// �N���X���FDxWindow
///// �T�v�FDxLib���g�p����Window�쐬���s���܂��B
///// �p�r�FWindow����̑S�ʂ��s���܂��B
///// </summary>
//class DxWindow
//{
//private:
//	HWND m_hWnd;
//
//public:
//	// �R���X�g���N�^
//	DxWindow();
//
//	// �f�X�g���N�^
//	virtual ~DxWindow() = default;
//
//	// HWND�^�̃n���h�����擾���܂��B
//	HWND GetHandle() { return m_hWnd; }
//
//	// �N���C�A���g�T�C�Y��ݒ肵�܂��BVector2Int�^�Ŏw�肵�Ă��������B
//	void SetClientSize(const Vector2Int& size);
//
//	// �N���C�A���g�T�C�Y���擾���܂��B�߂�l��Vector2Int�^�ł��B
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
//	// �E�B���h�E����ʂ̒����ɕ\�����܂��B
//	void Centering();
//
//protected:
//	void CreateHandle(const char* windowName);
//};