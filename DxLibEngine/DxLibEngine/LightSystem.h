#pragma once

class LightSystem : public System
{
private:
	static const int MAX_LIGHT = 32;

private:
	ComPtr<GraphicsBuffer> m_lightBuffer;					// 全てのライト情報を格納する構造化バッファ
	D3D12_GPU_DESCRIPTOR_HANDLE m_lightBufferGpuHandle;		// m_lightBufferのSRVを格納するデスクリプタヒープ

	int m_activeLightCount = 0; // 現在有効なライトの数

public:
	/// <summary>
	/// 現在有効なライトの数を取得します。
	/// </summary>
	int GetActiveLightCount() const { return m_activeLightCount; }

	/// <summary>
	/// このシステムが管理するデスクリプタヒープを取得します。
	/// （MeshRendererSystemがSetDescriptorHeapsで設定するために使用）
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetLightBufferGpuHandle() const { return m_lightBufferGpuHandle; }

private:
	/// <summary>
	/// システムの初期化処理。リソースの作成などを行います。
	/// </summary>
	void Start(ComponentManager& cm, World& world) override;

	/// <summary>
	/// 毎フレームの更新処理。シーン内のライト情報を収集し、バッファを更新します。
	/// </summary>
	void Update(ComponentManager& cm, World& world) override;
};