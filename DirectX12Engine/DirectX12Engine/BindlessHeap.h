#pragma once

// このクラスは、テクスチャのSRVを格納するためのディスクリプタヒープを管理します。
// 描画システムはこのクラスを通じてテクスチャのSRVを登録し、シェーダからアクセスできるようにします。
// BindlessHeapとは、GPU側にリソースをバインドすることなく、シェーダからリソースを直接アクセスできるようにするための仕組みです。
// BindlessHeapを使うことで、描画システムはテクスチャのSRVを事前に大量に登録しておき、シェーダからはインデックスを使ってアクセスできるようになります。
class BindlessHeap : public Singleton<BindlessHeap>
{
	friend class Singleton<BindlessHeap>;

public:
	static constexpr UINT MAX_DESCRIPTORS = 4096;

private:
	static inline ComPtr<DescriptorHeap> m_descriptorHeap;
	static inline UINT m_nextSlot = 0;
	static inline std::queue<UINT> m_freeList;

private:
	BindlessHeap();
	~BindlessHeap();

public:
	/// <summary>
	/// 指定されたテクスチャをBindlessHeapに登録します。
	/// </summary>
	/// <param name="texture">登録するテクスチャ</param>
	/// <returns>登録されたテクスチャのインデックス</returns>
	UINT Register(Texture2D* texture);

	/// <summary>
	/// StructureBuffer等のSRVをBindlessHeapに登録します。
	/// </summary>
	/// <param name="buffer">登録するバッファ</param>
	/// <param name="srvDesc">SRVのディスクリプタ情報</param>
	/// <returns>登録されたバッファのインデックス</returns>
	UINT RegisterBuffer(ID3D12Resource* buffer, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

	void Unregister(UINT index);

	DescriptorHeap* GetHeap() { return m_descriptorHeap.Get(); }
};