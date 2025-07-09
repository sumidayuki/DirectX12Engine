#pragma once

class Graphics
{
public:
	// �쐬����o�b�t�@�̖����i�萔�j
	static inline constexpr int BackBafferCount = 2;
	friend class Application;

private:
	static inline ComPtr<ID3D12Device>			m_d3d12Device;
	static inline ComPtr<ID3D12CommandQueue>	m_directCommandQueue;
	static inline ComPtr<IDXGISwapChain3>		m_dxgiSwapChain;
	static inline ComPtr<DescriptorHeap>		m_descriptorHeapRTV;
	static inline ComPtr<FrameResource>			m_frameResource[BackBafferCount];

private:
	/// <summary>
	/// DirectX12�̃O���t�B�b�N�X�R���|�[�l���g�ł���uDirect3D12�v�����������āA�O���t�B�b�N�X�@�\�𗘗p�\�ȏ�Ԃɂ��܂��B
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="resolution"></param>
	/// <returns></returns>
	static bool StaticConstructor(HWND hWnd, const Resolution& resolution);

	/// <summary>
	/// �O���t�B�b�N�X�@�\�̏I���������s���܂��B
	/// </summary>
	static void StaticDestructor();

public:
	/// <summary>
	/// Direct3D12�f�o�C�X���擾���܂��B
	/// </summary>
	/// <returns></returns>
	static ID3D12Device* GetD3D12Device() { return m_d3d12Device.Get(); }

	/// <summary>
	/// �R�}���h�L���[���擾���܂��B
	/// </summary>
	/// <returns></returns>
	static ID3D12CommandQueue* GetCommandQueue() { return m_directCommandQueue.Get(); }

	/// <summary>
	/// �X���b�v�`�F�[�����擾���܂��B
	/// </summary>
	/// <returns></returns>
	static IDXGISwapChain3* GetSwapChain() { return m_dxgiSwapChain.Get(); }

	/// <summary>
	/// ���݃����_�����O��ƂȂ��Ă���o�b�N�o�b�t�@�ɑΉ�����t���[�����\�[�X���擾���܂��B
	/// </summary>
	/// <returns></returns>
	static FrameResource* GetCurrentFrameResource();

	static void WaitForCompletionOfPresent();
};