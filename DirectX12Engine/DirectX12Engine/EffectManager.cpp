#include "EffectManager.h"

bool EffectManager::Initialize()
{
	ID3D12Device* device = Graphics::GetD3D12Device();
	ID3D12CommandQueue* commandQueue = Graphics::GetCommandQueue();

	m_graphicsDevice = EffekseerRendererDX12::CreateGraphicsDevice(device, commandQueue, Graphics::BackBafferCount);

	DXGI_FORMAT renderTargetFormats[] =
	{
		DXGI_FORMAT_R8G8B8A8_UNORM
	};

	m_renderer = EffekseerRendererDX12::Create(m_graphicsDevice, renderTargetFormats, 1, DXGI_FORMAT_D32_FLOAT, false, 8000);

	// 1フレーム用メモリ
	m_memoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(m_graphicsDevice);

	m_commandList = EffekseerRenderer::CreateCommandList(m_graphicsDevice, m_memoryPool);

	m_renderer->SetCommandList(m_commandList);

	constexpr int32_t MAX_INSTANCE_COUNT = 8000;

	m_manager = Effekseer::Manager::Create(MAX_INSTANCE_COUNT);

	if (!m_manager)
	{
		return false;
	}

	// レンダラー
	m_manager->SetSpriteRenderer(m_renderer->CreateSpriteRenderer());
	m_manager->SetRibbonRenderer(m_renderer->CreateRibbonRenderer());
	m_manager->SetRingRenderer(m_renderer->CreateRingRenderer());
	m_manager->SetTrackRenderer(m_renderer->CreateTrackRenderer());
	m_manager->SetModelRenderer(m_renderer->CreateModelRenderer());

	// ローダー
	m_manager->SetTextureLoader(m_renderer->CreateTextureLoader());
	m_manager->SetModelLoader(m_renderer->CreateModelLoader());
	m_manager->SetMaterialLoader(m_renderer->CreateMaterialLoader());

	return true;
}

void EffectManager::ShutDown()
{
	m_manager->StopAllEffects();
	m_manager.Reset();
	m_commandList.Reset();
	m_memoryPool.Reset();
	m_renderer.Reset();
	m_graphicsDevice.Reset();
}