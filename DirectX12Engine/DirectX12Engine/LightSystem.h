#pragma once

class LightSystem : public System
{
private:
	static const int MAX_LIGHT = 64;

private:
	ComPtr<GraphicsBuffer> m_lightBuffer;
	UINT m_lightSrvIndex = UINT32_MAX;

	int m_activeLightCount;

public:
	int GetActiveLightCount() const { return m_activeLightCount; }

	GraphicsBuffer* GetLightBuffer() { return m_lightBuffer.Get(); }

	/// <summary>
	/// BindlessHeap内のLightバッファSRVインデックスを取得します。
	/// </summary>
	UINT GetLightSrvIndex() const { return m_lightSrvIndex; }

private:
	void Start(World& world) override;
	void Update(World& world) override;
};