#include "ProceduralSkyRenderer.h"
#include "ShaderRegistry.h"
#include "ProceduralSky.hlsli"

void ProceduralSkyRenderer::Initialize(float time, float dayLength, bool isMoveTime, bool enableClouds, bool enableStars, bool enableMoon, bool enableSun, bool enableSunGlow)
{
	m_time = time;
	m_dayLength = dayLength;
	m_isMoveTime = isMoveTime;
	m_enableClouds = enableClouds;
	m_enableStars = enableStars;
	m_enableMoon = enableMoon;
	m_enableSun = enableSun;
	m_enableSunGlow = enableSunGlow;
	m_skyConstantsBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ProceduralSkyConstants)));
}

void ProceduralSkyRenderer::Draw(World& world)
{
	if (m_isMoveTime)
	{
		m_time += (1.0f / m_dayLength) * Time::GetDeltaTime();
		if (m_time > 1.0f)
		{
			m_time -= 1.0f;
		}
	}

	ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

	Shader* shader = ShaderRegistry::GetShader("ProceduralSky");
	if (!shader)
	{
		return;
	}

	commandList->SetGraphicsRootSignature(ShaderRegistry::GetRootSignature());
	
	CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();
	if (!cameraSystem)
	{
		return;
	}
	commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*cameraSystem->GetCurrent())->GetNativeBufferPtr()->GetGPUVirtualAddress());

	ProceduralSkyConstants* skyConstants = (ProceduralSkyConstants*)m_skyConstantsBuffer->LockBufferForWrite();
	skyConstants->timeOfDay = m_time;
	skyConstants->time = Time::GetTime();
	skyConstants->enableClouds = m_enableClouds ? 1 : 0;
	skyConstants->enableStars = m_enableStars ? 1 : 0;
	skyConstants->enableMoon = m_enableMoon ? 1 : 0;
	skyConstants->enableSun = m_enableSun ? 1 : 0;
	skyConstants->enableSunGlow = m_enableSunGlow ? 1 : 0;
	m_skyConstantsBuffer->UnlockBufferAfterWrite();

	commandList->SetGraphicsRootConstantBufferView(1, m_skyConstantsBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());

	commandList->SetPipelineState(shader->GetPSO(false));
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	commandList->DrawInstanced(3, 1, 0, 0);
}