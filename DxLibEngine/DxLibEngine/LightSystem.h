#pragma once

class LightSystem : public System
{
private:
	static const int MAX_LIGHT = 32;

private:
	ComPtr<GraphicsBuffer> m_lightBuffer;
	int m_activeLightCount;
	D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptorHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptorHandle;

public:
	/// <summary>
	/// 他のシステムがライト情報にアクセスするためのインターフェイス
	/// </summary>
	/// <returns></returns>
	D3D12_GPU_DESCRIPTOR_HANDLE GetLightBufferGpuHandle() const;

	int GetActiveLightCount() const;

private:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};