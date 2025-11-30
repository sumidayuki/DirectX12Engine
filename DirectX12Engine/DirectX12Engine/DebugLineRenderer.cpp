#include "DebugLineRenderer.h"

void DebugLineRenderer::AddLine(const Vector3& start, const Vector3& end, const Color& color)
{
	if (m_lines.size() >= MAX_LINES)
	{
		return;
	}

	Line line;
	line.start = start;
	line.end = end;
	line.color = color;
	m_lines.push_back(line);
}

void DebugLineRenderer::Initialize()
{
	ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

	ComPtr<ShaderBytecode> vertexShader;
	vertexShader.Attach(new ShaderBytecode(L"LineShader.hlsl", "VSMain", "vs_5_1"));

	ComPtr<ShaderBytecode> pixelShader;
	pixelShader.Attach(new ShaderBytecode(L"LineShader.hlsl", "PSMain", "ps_5_1"));

	// ルートシグネチャの作成
	D3D12_ROOT_PARAMETER param = {};
	param.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	param.Descriptor.ShaderRegister = 0;
	param.ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	D3D12_ROOT_SIGNATURE_DESC rsDesc = {};
	rsDesc.NumParameters = 1;
	rsDesc.pParameters = &param;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serializedRootSignature;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializedRootSignature, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		assert(SUCCEEDED(hr));
	}

	hr = d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	assert(SUCCEEDED(hr));
	
	// 入力レイアウト
	static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 } 
	};

	// グラフィックスパイプラインステート
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = { vertexShader->GetBytecodePointer(), vertexShader->GetBytecodeLength() };
	psoDesc.PS = { pixelShader->GetBytecodePointer(), pixelShader->GetBytecodeLength() };
	psoDesc.InputLayout = { inputElements, _countof(inputElements) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
	psoDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	psoDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	psoDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	psoDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	if (FAILED(d3d12Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_pipelineState.ReleaseAndGetAddressOf()))))
	{
		assert(0);
	}

	// 頂点バッファの作成
	const UINT vertexBufferSize = sizeof(DebugVertex) * MAX_VERTICES;

	m_vertexBuffer.Attach(new GraphicsBuffer(
		GraphicsBuffer::Target::Vertex,
		GraphicsBuffer::UsageFlags::LockBufferForWrite,
		1,
		vertexBufferSize

	));

	// 頂点バッファビューの作成
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = sizeof(DebugVertex);
	m_vertexBufferView.SizeInBytes = vertexBufferSize;
	
	// 頂点バッファをマップ
	m_mappedVertices = (DebugVertex*)m_vertexBuffer->LockBufferForWrite();
}

void DebugLineRenderer::Render(const Matrix4x4& viewProjectionMatrix)
{
	if (m_lines.empty())
	{
		return;
	}

	// 頂点データの転送
	UINT vertexIndex = 0;
	for (const Line& line : m_lines)
	{
		m_mappedVertices[vertexIndex].position = line.start;
		m_mappedVertices[vertexIndex].color = line.color;
		vertexIndex++;
		m_mappedVertices[vertexIndex].position = line.end;
		m_mappedVertices[vertexIndex].color = line.color;
		vertexIndex++;
	}
	
	ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();
	
	// パイプラインステートとルートシグネチャの設定
	commandList->SetPipelineState(m_pipelineState.Get());
	commandList->SetGraphicsRootSignature(m_rootSignature.Get());
	
	// ビュープロジェクション行列の設定
	CameraSystem* cameraSystem = SceneManager::GetCurrentScene()->GetWorld().GetSystem<CameraSystem>();
	if (!cameraSystem) return;

	Camera* camera = cameraSystem->GetCurrent();
	if (!camera) return;
	
	commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());
	
	// 頂点バッファの設定
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	
	// ラインの描画
	commandList->DrawInstanced(static_cast<UINT>(m_lines.size()) * 2, 1, 0, 0);
	
	// ラインリストをクリア
	m_lines.clear();
}

void DebugLineRenderer::Shutdown()
{

}
