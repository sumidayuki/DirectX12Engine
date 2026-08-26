#pragma once

#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

class EffectManager : public Singleton<EffectManager>
{
	friend class Singleton<EffectManager>;

private:
	Effekseer::Backend::GraphicsDeviceRef m_graphicsDevice;
	EffekseerRenderer::RendererRef m_renderer;
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> m_memoryPool;
	Effekseer::RefPtr<EffekseerRenderer::CommandList> m_commandList;
	Effekseer::ManagerRef m_manager;

private:
	EffectManager() = default;
	~EffectManager() = default;

public:
	// 初期化
	bool Initialize();

	// 終了
	void ShutDown();

	// Effekseerのグラフィックデバイスを取得
	Effekseer::Backend::GraphicsDeviceRef GetGraphicsDevice() const { return m_graphicsDevice; }

	// Effekseerのレンダラーを取得
	EffekseerRenderer::RendererRef GetRenderer() const { return m_renderer; }

	// Effekseerのメモリープールを取得
	Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> GetMemoryPool() const { return m_memoryPool; }

	// Effekseerのコマンドリストを取得
	Effekseer::RefPtr<EffekseerRenderer::CommandList> GetCommandList() const { return m_commandList; }

	// EffekseerのManagerを取得
	Effekseer::ManagerRef GetManager() const { return m_manager; }
};