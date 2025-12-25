#pragma once

class SkinnedMeshRendererSystem : public System
{
private:
	friend class Application;

	// リングバッファとして使用するオブジェクト定数バッファ
	static inline ComPtr<GraphicsBuffer> m_objectConstantBufferRing;
	static inline BYTE* m_mappedObjectConstants = nullptr;
	static inline UINT m_currentObjectBufferIndex = 0;

	std::unordered_map<Texture2D*, D3D12_GPU_DESCRIPTOR_HANDLE> m_srvCache;

private:
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRV(Texture2D* tex, DescriptorAllocator* allocator);

private:
	static void StaticConstructor();
	static void StaticDestructor();

public:
	void Start(World& world) override;

	void Draw(World& world) override;
};