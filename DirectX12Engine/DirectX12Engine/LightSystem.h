#pragma once

class LightSystem : public System
{
private:
	static const int MAX_LIGHT = 64;

private:
	ComPtr<GraphicsBuffer> m_lightBuffer;					// 全てのライト情報を格納する構造化バッファ
	D3D12_GPU_DESCRIPTOR_HANDLE m_lightBufferGpuHandle;		// m_lightBufferのSRVを格納するデスクリプタヒープ
	D3D12_CPU_DESCRIPTOR_HANDLE m_lightBufferCpuHandle;

	ComPtr<ID3D12DescriptorHeap> m_staticSrvHeap;

	int m_activeLightCount; // 現在有効なライトの数

public:
	/// <summary>
	/// 現在有効なライトの数を取得します。
	/// </summary>
	int GetActiveLightCount() const { return m_activeLightCount; }

	GraphicsBuffer* GetLightBuffer() { return m_lightBuffer.Get(); }

	/// <summary>
	/// このシステムが管理するデスクリプタヒープを取得します。
	/// （MeshRendererSystemがSetDescriptorHeapsで設定するために使用）
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetLightBufferGpuHandle() const { return m_lightBufferGpuHandle; }
	D3D12_CPU_DESCRIPTOR_HANDLE GetLightBufferCpuHandle() const { return m_lightBufferCpuHandle; }

private:
	/// <summary>
	/// システムの初期化処理。リソースの作成などを行います。
	/// </summary>
	void Start(World& world) override;

	/// <summary>
	/// 毎フレームの更新処理。シーン内のライト情報を収集し、バッファを更新します。
	/// </summary>
	void Update(World& world) override;
};