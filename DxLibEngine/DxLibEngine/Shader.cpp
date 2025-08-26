#include "Shader.h"

Shader::Shader(const std::wstring& vsPath, const std::string& vsEntry, const std::string& vsTarget, const std::wstring& psPath, const std::string& psEntry, const std::string& psTarget)
{
	ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

	ComPtr<ShaderBytecode> vertexShader;
	vertexShader.Attach(new ShaderBytecode(vsPath.c_str(), vsEntry.c_str(), vsTarget.c_str()));

	ComPtr<ShaderBytecode> pixelShader;
	pixelShader.Attach(new ShaderBytecode(psPath.c_str(), psEntry.c_str(), psTarget.c_str()));

	// ルートシグネチャ
	D3D12_ROOT_PARAMETER rootParameters[5];
	memset(rootParameters, 0, sizeof(rootParameters));

	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	D3D12_DESCRIPTOR_RANGE ranges[1];
	memset(ranges, 0, sizeof(ranges));
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[0].NumDescriptors = (UINT)Material::TextureSlot::Max;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
	rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_DESCRIPTOR_RANGE lightRanges[1];
	memset(lightRanges, 0, sizeof(lightRanges));
	lightRanges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	lightRanges[0].NumDescriptors = 1;
	lightRanges[0].BaseShaderRegister = 3;
	lightRanges[0].RegisterSpace = 0;
	lightRanges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[3].DescriptorTable.NumDescriptorRanges = _countof(lightRanges);
	rootParameters[3].DescriptorTable.pDescriptorRanges = lightRanges;
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	rootParameters[4].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[4].Descriptor.ShaderRegister = 2;
	rootParameters[4].Descriptor.RegisterSpace = 0;
	rootParameters[4].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_STATIC_SAMPLER_DESC sampler;
	memset(&sampler, 0, sizeof(sampler));
	sampler.Filter = D3D12_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(rsDesc));
	rsDesc.NumParameters = _countof(rootParameters);
	rsDesc.pParameters = rootParameters;
	rsDesc.pStaticSamplers = &sampler;
	rsDesc.NumStaticSamplers = 1;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ComPtr<ID3DBlob> serializeRootSignature;
	ComPtr<ID3DBlob> errorBlob;
	HRESULT hr = D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, &serializeRootSignature, &errorBlob);
	if (FAILED(hr))
	{
		if (errorBlob)
		{
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());
		}
		assert(SUCCEEDED(hr));
	}

	hr = d3d12Device->CreateRootSignature(0, serializeRootSignature->GetBufferPointer(), serializeRootSignature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));
	assert(SUCCEEDED(hr));

	static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.pRootSignature = m_rootSignature.Get();
	psoDesc.VS = { vertexShader->GetBytecodePointer(), vertexShader->GetBytecodeLength() };
	psoDesc.PS = { pixelShader->GetBytecodePointer(), pixelShader->GetBytecodeLength() };
	psoDesc.InputLayout = { inputElements, _countof(inputElements) };
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	psoDesc.SampleMask = UINT_MAX;

	psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;

	psoDesc.DepthStencilState.DepthEnable = TRUE;
	psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	psoDesc.DepthStencilState.StencilEnable = FALSE;

	psoDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
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
}

void Shader::CreateRootSignature()
{
}

void Shader::CreatePipelineState(const D3D12_INPUT_ELEMENT_DESC* inputElements, UINT numInputElements, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& basePsoDesc, ShaderBytecode* vertexShader, ShaderBytecode* pixelShader)
{
}
