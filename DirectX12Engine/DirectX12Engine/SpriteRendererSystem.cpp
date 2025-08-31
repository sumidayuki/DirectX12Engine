#include "SpriteRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"

// �萔�o�b�t�@�̃��������C�A�E�g��\���\����
struct SpriteRendererSystem::ConstantBufferLayout
{
	Matrix4x4       world;
	Color           color;
	Vector2         minUV;
	Vector2         maxUV;
	unsigned int    flipEnable[4];
};

void SpriteRendererSystem::StaticConstructor()
{
	ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

	// �V�F�[�_�[�t�@�C�����R���p�C�����ăo�C�g�R�[�h���쐬
	ComPtr<ShaderBytecode> vertexShader;
	vertexShader.Attach(new ShaderBytecode(L"SpriteRenderer.hlsl", "VSMain", "vs_5_1"));

	ComPtr<ShaderBytecode> pixelShader;
	pixelShader.Attach(new ShaderBytecode(L"SpriteRenderer.hlsl", "PSMain", "ps_5_1"));

	//-------------------------------------------------------------------------------------
	// ���[�g�V�O�l�`�����쐬
	//-------------------------------------------------------------------------------------

	// ���[�g�p�����[�^�[�z��
	D3D12_ROOT_PARAMETER rootParameters[3];
	memset(rootParameters, 0, sizeof(rootParameters));

	// �萔�o�b�t�@
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// �f�B�X�N���v�^�e�[�u��
	// �f�B�X�N���v�^�����W�z��
	D3D12_DESCRIPTOR_RANGE ranges[1];
	memset(ranges, 0, sizeof(ranges));
	ranges[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
	ranges[0].BaseShaderRegister = 0;
	ranges[0].RegisterSpace = 0;
	ranges[0].NumDescriptors = 1;
	ranges[0].OffsetInDescriptorsFromTableStart = 0;

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(ranges);
	rootParameters[2].DescriptorTable.pDescriptorRanges = ranges;
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	// �ÓI�T���v���[�z��
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1];
	memset(staticSamplers, 0, sizeof(staticSamplers));
	staticSamplers[0].ShaderRegister = 0;
	staticSamplers[0].RegisterSpace = 0;
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	staticSamplers[0].BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	staticSamplers[0].MaxAnisotropy = 0;
	staticSamplers[0].MinLOD = 0.0f;
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;
	staticSamplers[0].MipLODBias = 0.0f;

	// ���[�g�V�O�l�`���쐬���̏ڍ׏��
	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(rsDesc));
	rsDesc.NumParameters = _countof(rootParameters);
	rsDesc.pParameters = rootParameters;
	rsDesc.NumStaticSamplers = _countof(staticSamplers);
	rsDesc.pStaticSamplers = staticSamplers;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ���[�g�V�O�l�`���쐬���̏ڍ׏����V���A���C�Y����B
	ComPtr<ID3DBlob> serializedRootSignature;
	ComPtr<ID3DBlob> serializeErrorMessage;
	if (D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, serializedRootSignature.ReleaseAndGetAddressOf(), serializeErrorMessage.ReleaseAndGetAddressOf()) < 0)
	{
		// �G���[���b�Z�[�W������ꍇ�́u�o�́v�E�B���h�E�ɕ\������B
		if (serializeErrorMessage)
		{
			OutputDebugStringA((const char*)serializeErrorMessage->GetBufferPointer());
		}
		assert(0);
	}

	// ���[�g�V�O�l�`�����쐬
	if (d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_ID3D12RootSignature, (void**)m_rootSignature.ReleaseAndGetAddressOf()) < 0)
	{
		assert(0);
	}

	// ���͗v�f�z��
	static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		// �Z�}���e�B�b�N���A�Z�}���e�B�b�N�ԍ��A�f�[�^�t�H�[�}�b�g�A���̓X���b�g�A�o�C�g�I�t�Z�b�g�A���̓N���X�A�C���X�^���X���Ƃ̃o�C�g��
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//-------------------------------------------------------------------------------------
	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g���쐬
	//-------------------------------------------------------------------------------------

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�쐬���̏ڍ׏��
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc;
	memset(&gpsDesc, 0, sizeof(gpsDesc));

	// ���͗v�f
	gpsDesc.InputLayout.NumElements = _countof(inputElements);
	gpsDesc.InputLayout.pInputElementDescs = inputElements;

	// �V�F�[�_�[��ݒ�
	gpsDesc.VS.BytecodeLength = vertexShader->GetBytecodeLength();
	gpsDesc.VS.pShaderBytecode = vertexShader->GetBytecodePointer();
	gpsDesc.HS.BytecodeLength = 0;
	gpsDesc.HS.pShaderBytecode = nullptr;
	gpsDesc.DS.BytecodeLength = 0;
	gpsDesc.DS.pShaderBytecode = nullptr;
	gpsDesc.GS.BytecodeLength = 0;
	gpsDesc.GS.pShaderBytecode = nullptr;
	gpsDesc.PS.BytecodeLength = pixelShader->GetBytecodeLength();
	gpsDesc.PS.pShaderBytecode = pixelShader->GetBytecodePointer();

	// ���[�g�V�O�l�`����ݒ�
	gpsDesc.pRootSignature = m_rootSignature.Get();

	// �|���S����ݒ�
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// �C���f�b�N�X�o�b�t�@���̃X�g���b�v�J�b�g�l
	gpsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// �}���`�T���v�����O�ݒ�
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ���X�^���C�U�[�ݒ�
	gpsDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
	gpsDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
	gpsDesc.RasterizerState.AntialiasedLineEnable = FALSE;
	gpsDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	gpsDesc.RasterizerState.FrontCounterClockwise = FALSE;
	gpsDesc.RasterizerState.MultisampleEnable = FALSE;
	gpsDesc.RasterizerState.ForcedSampleCount = 0;
	gpsDesc.RasterizerState.DepthClipEnable = TRUE;
	gpsDesc.RasterizerState.DepthBias = 0;
	gpsDesc.RasterizerState.DepthBiasClamp = 0.0f;
	gpsDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;

	// �}���`�����_�[�^�[�Q�b�g�ݒ�
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[2] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[3] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[4] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[5] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[6] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[7] = DXGI_FORMAT_UNKNOWN;

	// �[�x�X�e���V���o�b�t�@�̃s�N�Z���t�H�[�}�b�g
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// �[�x/�X�e���V���ݒ�
	gpsDesc.DepthStencilState.DepthEnable = TRUE;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.StencilEnable = FALSE;
	gpsDesc.DepthStencilState.StencilReadMask = 0;
	gpsDesc.DepthStencilState.StencilWriteMask = 0;

	// �s�N�Z�����Ƃ̃J���[�u�����h�ݒ�
	gpsDesc.BlendState.AlphaToCoverageEnable = FALSE;
	gpsDesc.BlendState.IndependentBlendEnable = FALSE;

	// �����_�[�^�[�Q�b�g[0]�̃u�����h�ݒ�
	gpsDesc.BlendState.RenderTarget[0].BlendEnable = TRUE;
	gpsDesc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	gpsDesc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
	gpsDesc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	gpsDesc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	gpsDesc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	gpsDesc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	gpsDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = 0x0f;
	gpsDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
	gpsDesc.BlendState.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;

	// �����_�[�^�[�Q�b�g[1�`7]�̃u�����h�ݒ�
	for (int i = 1; i < 8; i++)
	{
		gpsDesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		gpsDesc.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
	}

	// �L���b�V�����ꂽ�p�C�v���C���X�e�[�g�ݒ���g�p���Ȃ�
	gpsDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	gpsDesc.CachedPSO.pCachedBlob = nullptr;

	// �e��r�b�g�t���O
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// �m�[�h�}�X�N
	gpsDesc.NodeMask = 0;

	// �O���t�B�b�N�X�p�C�v���C���X�e�[�g�I�u�W�F�N�g�̍쐬
	if (d3d12Device->CreateGraphicsPipelineState(&gpsDesc, IID_ID3D12PipelineState, (void**)m_graphicsPipelineState.ReleaseAndGetAddressOf()) < 0)
	{
		assert(0);
	}
}

void SpriteRendererSystem::StaticDestructor()
{
	m_graphicsPipelineState = nullptr;
	m_rootSignature = nullptr;
}

void SpriteRendererSystem::SetSprite(SpriteRenderer* spriteRenderer, Sprite* sprite)
{
	if (sprite != spriteRenderer->sprite.Get())
	{
		spriteRenderer->sprite = sprite;

		if (!spriteRenderer->constantBuffer)
		{
			// �萔�o�b�t�@�̍쐬
			spriteRenderer->constantBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ConstantBufferLayout)));
		}
		if (!spriteRenderer->descriptorHeap)
		{
			// �f�B�X�N���v�^�q�[�v���쐬
			spriteRenderer->descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
		}

		// �X�v���C�g�� nullptr �o�Ȃ��ꍇ��SRV���R�s�[���Ă���
		if (spriteRenderer->sprite)
		{
			const D3D12_CPU_DESCRIPTOR_HANDLE copyTo = spriteRenderer->descriptorHeap->GetCPUDescriptorHandle(0);
			const D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = spriteRenderer->sprite->GetTexture()->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
			Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		}
	}
}

void SpriteRendererSystem::Start(ComponentManager& cm, World& world)
{
	
}

void SpriteRendererSystem::Draw(ComponentManager& cm, World& world)
{
	View<Transform, SpriteRenderer> view(cm);

	int spriteCount = 0;

	for (auto [entity, transform, spriteRenderer] : view)
	{
		spriteCount++;

		if (!spriteRenderer.isEnabled)
		{
			OutputDebugStringA("�X�v���C�g������������Ă��܂�\n");
			continue;
		}

		if (!spriteRenderer.sprite)
		{
			OutputDebugStringA("�X�v���C�g��null�ł�\n");
			continue;
		}

		// �e�N�X�`�����������ǂݍ��܂�Ă��邩�m�F
		if (spriteRenderer.sprite->GetTexture() == nullptr)
		{
			OutputDebugStringA("�e�N�X�`����null�ł�\n");
		}


		// ���[���h�ϊ��s����擾
		const Matrix4x4& localToWorldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

		// �萔�o�b�t�@�ɏ�������
		ConstantBufferLayout* lockedPointer = (ConstantBufferLayout*)spriteRenderer.constantBuffer->LockBufferForWrite();
		lockedPointer->world = localToWorldMatrix.Transpose();
		lockedPointer->color = spriteRenderer.color;
		lockedPointer->minUV = spriteRenderer.sprite->m_minUV;
		lockedPointer->maxUV = spriteRenderer.sprite->m_maxUV;
		lockedPointer->flipEnable[0] = spriteRenderer.flipX ? 1 : 0;
		lockedPointer->flipEnable[1] = spriteRenderer.flipY ? 1 : 0;
		spriteRenderer.constantBuffer->UnlockBufferAfterWrite();

		// ���݂̃t���[�����\�[�X���擾
		FrameResource* frameResource = Graphics::GetCurrentFrameResource();

		// �R�}���h���X�g���擾
		ID3D12GraphicsCommandList* commandList = frameResource->GetCommandList();

		// �R�}���h�u�O���t�B�b�N�X�p�C�v���C���X�e�[�g��ύX����v���R�}���h���X�g�ɒǉ�
		commandList->SetPipelineState(m_graphicsPipelineState.Get());

		// �R�}���h�u���[�g�V�O�l�`����ύX����v���R�}���h�ɒǉ�
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());

		// �R�}���h�u�f�B�X�N���v�^�q�[�v��ύX����v���R�}���h���X�g�ɒǉ�
		ID3D12DescriptorHeap* const descriptorHeaps[] = { spriteRenderer.descriptorHeap->GetNativeHeapPointer(), };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();

		// ���[�g�p�����[�^�[���Ƃ̐ݒ�
		Camera* camera = cameraSystem->GetCurrent();
		commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, spriteRenderer.constantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, spriteRenderer.descriptorHeap->GetGPUDescriptorHandle(0));

		//---------------------------------------------------------------------------------
		// InputAssembler(IA)�X�e�[�W�̐ݒ�
		//---------------------------------------------------------------------------------

		// �R�}���h�u���_�o�b�t�@�r���[�z���ύX����v���R�}���h���X�g�ɒǉ�
		GraphicsBuffer* vertexBuffer = spriteRenderer.sprite->GetVertexBuffer();

		D3D12_VERTEX_BUFFER_VIEW vbViews[1];
		memset(vbViews, 0, sizeof(vbViews));
		vbViews[0].StrideInBytes = vertexBuffer->GetStride();
		vbViews[0].SizeInBytes = vertexBuffer->GetSizeInBytes();
		vbViews[0].BufferLocation = vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
		commandList->IASetVertexBuffers(0, _countof(vbViews), vbViews);

		// �R�}���h�u�C���f�b�N�X�o�b�t�@�r���[��ύX����v���R�}���h���X�g�ɒǉ�
		GraphicsBuffer* indexBuffer = spriteRenderer.sprite->GetIndexBuffer();

		D3D12_INDEX_BUFFER_VIEW ibView;
		memset(&ibView, 0, sizeof(ibView));
		ibView.Format = (indexBuffer->GetStride() == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = indexBuffer->GetSizeInBytes();
		ibView.BufferLocation = indexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
		commandList->IASetIndexBuffer(&ibView);

		// �R�}���h�u�����܂ł̐ݒ���g���ĕ`����J�n����v���R�}���h���X�g�ɒǉ�
		commandList->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}
}