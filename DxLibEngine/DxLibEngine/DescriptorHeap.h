#pragma once

// ディスクリプタヒープの種類
enum class DescriptorHeapType
{
	CBV_SRV_UAV,
	RTV,
	DSV,
	SAMPLER,
};

class DescriptorHeap : public Reference
{
private:
	DescriptorHeapType				m_heapType;				// ディスクリプタヒープの種類
	int								m_stride;				// ディスクリプタ1個分のサイズ（単位はバイト）
	int								m_count;				// 要素数
	bool							m_isShaderVisible;		// シェーダの可視性
	ComPtr<ID3D12DescriptorHeap>	m_nativeHeap;			// Direct3D12のインターフェイス

public:
	// コンストラクタ
	DescriptorHeap(DescriptorHeapType heapType, int count, bool isShaderVisible);

	/// <summary>
	/// ディスクリプタヒープの種類を表す値を取得します。
	/// </summary>
	/// <returns>DescriptorHeapType型の値を返します。</returns>
	DescriptorHeapType GetHeapType() const { return m_heapType; }

	/// <summary>
	/// ディスクリプタ1個分のサイズを取得します。（単位はバイト）
	/// </summary>
	/// <returns>int型の値を返します。</returns>
	int GetStride() const { return m_stride; }

	/// <summary>
	/// 要素数を取得します。
	/// </summary>
	/// <returns>int型の値を返します。</returns>
	int GetCount() const { return m_count; }

	/// <summary>
	/// シェーダの可視性を取得します。
	/// </summary>
	/// <returns>bool型の値を返します。</returns>
	bool IsShaderVisible() const { return m_isShaderVisible; }

	/// <summary>
	/// Direct3D12のディスクリプタヒープインターフェイスを取得します。
	/// </summary>
	/// <returns>ID3D12DescriptorHeap型のポインターを返します。</returns>
	ID3D12DescriptorHeap* GetNativeHeapPointer() const { return m_nativeHeap.Get(); }

	/// <summary>
	/// ヒープ先頭からディスクリプタindex個分だけ進んだ場所を返します。(CPU側)
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(int index) const;
	
	/// <summary>
	/// ヒープ先頭からディスクリプタindex個分だけ進んだ場所を返します。(GPU側)
	/// </summary>
	/// <param name="index"></param>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(int index) const;
};