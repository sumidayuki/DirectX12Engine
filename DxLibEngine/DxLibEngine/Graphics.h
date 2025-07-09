#pragma once

class Graphics
{
public:
	// 作成するバッファの枚数（定数）
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
	/// DirectX12のグラフィックスコンポーネントである「Direct3D12」を初期化して、グラフィックス機能を利用可能な状態にします。
	/// </summary>
	/// <param name="hWnd"></param>
	/// <param name="resolution"></param>
	/// <returns></returns>
	static bool StaticConstructor(HWND hWnd, const Resolution& resolution);

	/// <summary>
	/// グラフィックス機能の終了処理を行います。
	/// </summary>
	static void StaticDestructor();

public:
	/// <summary>
	/// Direct3D12デバイスを取得します。
	/// </summary>
	/// <returns></returns>
	static ID3D12Device* GetD3D12Device() { return m_d3d12Device.Get(); }

	/// <summary>
	/// コマンドキューを取得します。
	/// </summary>
	/// <returns></returns>
	static ID3D12CommandQueue* GetCommandQueue() { return m_directCommandQueue.Get(); }

	/// <summary>
	/// スワップチェーンを取得します。
	/// </summary>
	/// <returns></returns>
	static IDXGISwapChain3* GetSwapChain() { return m_dxgiSwapChain.Get(); }

	/// <summary>
	/// 現在レンダリング先となっているバックバッファに対応するフレームリソースを取得します。
	/// </summary>
	/// <returns></returns>
	static FrameResource* GetCurrentFrameResource();

	static void WaitForCompletionOfPresent();
};