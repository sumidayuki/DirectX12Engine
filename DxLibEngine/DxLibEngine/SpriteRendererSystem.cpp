#include "SpriteRendererSystem.h"
#include "TransformSystem.h"
#include "CameraSystem.h"

// 定数バッファのメモリレイアウトを表す構造体
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

	// シェーダーファイルをコンパイルしてバイトコードを作成
	ComPtr<ShaderBytecode> vertexShader;
	vertexShader.Attach(new ShaderBytecode(L"SpriteRenderer.hlsl", "VSMain", "vs_5_1"));

	ComPtr<ShaderBytecode> pixelShader;
	pixelShader.Attach(new ShaderBytecode(L"SpriteRenderer.hlsl", "PSMain", "ps_5_1"));

	//-------------------------------------------------------------------------------------
	// ルートシグネチャを作成
	//-------------------------------------------------------------------------------------

	// ルートパラメーター配列
	D3D12_ROOT_PARAMETER rootParameters[3];
	memset(rootParameters, 0, sizeof(rootParameters));

	// 定数バッファ
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[0].Descriptor.ShaderRegister = 0;
	rootParameters[0].Descriptor.RegisterSpace = 0;
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	rootParameters[1].Descriptor.ShaderRegister = 1;
	rootParameters[1].Descriptor.RegisterSpace = 0;
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	// ディスクリプタテーブル
	// ディスクリプタレンジ配列
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

	// 静的サンプラー配列
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

	// ルートシグネチャ作成時の詳細情報
	D3D12_ROOT_SIGNATURE_DESC rsDesc;
	memset(&rsDesc, 0, sizeof(rsDesc));
	rsDesc.NumParameters = _countof(rootParameters);
	rsDesc.pParameters = rootParameters;
	rsDesc.NumStaticSamplers = _countof(staticSamplers);
	rsDesc.pStaticSamplers = staticSamplers;
	rsDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	// ルートシグネチャ作成時の詳細情報をシリアライズする。
	ComPtr<ID3DBlob> serializedRootSignature;
	ComPtr<ID3DBlob> serializeErrorMessage;
	if (D3D12SerializeRootSignature(&rsDesc, D3D_ROOT_SIGNATURE_VERSION_1_0, serializedRootSignature.ReleaseAndGetAddressOf(), serializeErrorMessage.ReleaseAndGetAddressOf()) < 0)
	{
		// エラーメッセージがある場合は「出力」ウィンドウに表示する。
		if (serializeErrorMessage)
		{
			OutputDebugStringA((const char*)serializeErrorMessage->GetBufferPointer());
		}
		assert(0);
	}

	// ルートシグネチャを作成
	if (d3d12Device->CreateRootSignature(0, serializedRootSignature->GetBufferPointer(), serializedRootSignature->GetBufferSize(), IID_ID3D12RootSignature, (void**)m_rootSignature.ReleaseAndGetAddressOf()) < 0)
	{
		assert(0);
	}

	// 入力要素配列
	static const D3D12_INPUT_ELEMENT_DESC inputElements[] =
	{
		// セマンティック名、セマンティック番号、データフォーマット、入力スロット、バイトオフセット、入力クラス、インスタンスごとのバイト数
		{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 8, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	};

	//-------------------------------------------------------------------------------------
	// グラフィックスパイプラインステートを作成
	//-------------------------------------------------------------------------------------

	// グラフィックスパイプラインステート作成時の詳細情報
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpsDesc;
	memset(&gpsDesc, 0, sizeof(gpsDesc));

	// 入力要素
	gpsDesc.InputLayout.NumElements = _countof(inputElements);
	gpsDesc.InputLayout.pInputElementDescs = inputElements;

	// シェーダーを設定
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

	// ルートシグネチャを設定
	gpsDesc.pRootSignature = m_rootSignature.Get();

	// ポリゴンを設定
	gpsDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	// インデックスバッファ内のストリップカット値
	gpsDesc.IBStripCutValue = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_DISABLED;

	// マルチサンプリング設定
	gpsDesc.SampleDesc.Count = 1;
	gpsDesc.SampleDesc.Quality = 0;
	gpsDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	// ラスタライザー設定
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

	// マルチレンダーターゲット設定
	gpsDesc.NumRenderTargets = 1;
	gpsDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	gpsDesc.RTVFormats[1] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[2] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[3] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[4] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[5] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[6] = DXGI_FORMAT_UNKNOWN;
	gpsDesc.RTVFormats[7] = DXGI_FORMAT_UNKNOWN;

	// 深度ステンシルバッファのピクセルフォーマット
	gpsDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// 深度/ステンシル設定
	gpsDesc.DepthStencilState.DepthEnable = TRUE;
	gpsDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	gpsDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	gpsDesc.DepthStencilState.StencilEnable = FALSE;
	gpsDesc.DepthStencilState.StencilReadMask = 0;
	gpsDesc.DepthStencilState.StencilWriteMask = 0;

	// ピクセルごとのカラーブレンド設定
	gpsDesc.BlendState.AlphaToCoverageEnable = FALSE;
	gpsDesc.BlendState.IndependentBlendEnable = FALSE;

	// レンダーターゲット[0]のブレンド設定
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

	// レンダーターゲット[1～7]のブレンド設定
	for (int i = 1; i < 8; i++)
	{
		gpsDesc.BlendState.RenderTarget[i].BlendEnable = FALSE;
		gpsDesc.BlendState.RenderTarget[i].LogicOpEnable = FALSE;
	}

	// キャッシュされたパイプラインステート設定を使用しない
	gpsDesc.CachedPSO.CachedBlobSizeInBytes = 0;
	gpsDesc.CachedPSO.pCachedBlob = nullptr;

	// 各種ビットフラグ
	gpsDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	// ノードマスク
	gpsDesc.NodeMask = 0;

	// グラフィックスパイプラインステートオブジェクトの作成
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
			// 定数バッファの作成
			spriteRenderer->constantBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Constant, GraphicsBuffer::UsageFlags::LockBufferForWrite, 1, sizeof(ConstantBufferLayout)));
		}
		if (!spriteRenderer->descriptorHeap)
		{
			// ディスクリプタヒープを作成
			spriteRenderer->descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
		}

		// スプライトが nullptr 出ない場合はSRVをコピーしておく
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
			OutputDebugStringA("スプライトが無効化されています\n");
			continue;
		}

		if (!spriteRenderer.sprite)
		{
			OutputDebugStringA("スプライトがnullです\n");
			continue;
		}

		// テクスチャが正しく読み込まれているか確認
		if (spriteRenderer.sprite->GetTexture() == nullptr)
		{
			OutputDebugStringA("テクスチャがnullです\n");
		}


		// ワールド変換行列を取得
		const Matrix4x4& localToWorldMatrix = world.GetSystem<TransformSystem>()->GetLocalToWorldMatrix(transform);

		// 定数バッファに書き込む
		ConstantBufferLayout* lockedPointer = (ConstantBufferLayout*)spriteRenderer.constantBuffer->LockBufferForWrite();
		lockedPointer->world = localToWorldMatrix.Transpose();
		lockedPointer->color = spriteRenderer.color;
		lockedPointer->minUV = spriteRenderer.sprite->m_minUV;
		lockedPointer->maxUV = spriteRenderer.sprite->m_maxUV;
		lockedPointer->flipEnable[0] = spriteRenderer.flipX ? 1 : 0;
		lockedPointer->flipEnable[1] = spriteRenderer.flipY ? 1 : 0;
		spriteRenderer.constantBuffer->UnlockBufferAfterWrite();

		// 現在のフレームリソースを取得
		FrameResource* frameResource = Graphics::GetCurrentFrameResource();

		// コマンドリストを取得
		ID3D12GraphicsCommandList* commandList = frameResource->GetCommandList();

		// コマンド「グラフィックスパイプラインステートを変更する」をコマンドリストに追加
		commandList->SetPipelineState(m_graphicsPipelineState.Get());

		// コマンド「ルートシグネチャを変更する」をコマンドに追加
		commandList->SetGraphicsRootSignature(m_rootSignature.Get());

		// コマンド「ディスクリプタヒープを変更する」をコマンドリストに追加
		ID3D12DescriptorHeap* const descriptorHeaps[] = { spriteRenderer.descriptorHeap->GetNativeHeapPointer(), };
		commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

		CameraSystem* cameraSystem = world.GetSystem<CameraSystem>();

		// ルートパラメーターごとの設定
		Camera* camera = cameraSystem->GetCurrent();
		commandList->SetGraphicsRootConstantBufferView(0, cameraSystem->GetCameraBuffer(*camera)->GetNativeBufferPtr()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootConstantBufferView(1, spriteRenderer.constantBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress());
		commandList->SetGraphicsRootDescriptorTable(2, spriteRenderer.descriptorHeap->GetGPUDescriptorHandle(0));

		//---------------------------------------------------------------------------------
		// InputAssembler(IA)ステージの設定
		//---------------------------------------------------------------------------------

		// コマンド「頂点バッファビュー配列を変更する」をコマンドリストに追加
		GraphicsBuffer* vertexBuffer = spriteRenderer.sprite->GetVertexBuffer();

		D3D12_VERTEX_BUFFER_VIEW vbViews[1];
		memset(vbViews, 0, sizeof(vbViews));
		vbViews[0].StrideInBytes = vertexBuffer->GetStride();
		vbViews[0].SizeInBytes = vertexBuffer->GetSizeInBytes();
		vbViews[0].BufferLocation = vertexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
		commandList->IASetVertexBuffers(0, _countof(vbViews), vbViews);

		// コマンド「インデックスバッファビューを変更する」をコマンドリストに追加
		GraphicsBuffer* indexBuffer = spriteRenderer.sprite->GetIndexBuffer();

		D3D12_INDEX_BUFFER_VIEW ibView;
		memset(&ibView, 0, sizeof(ibView));
		ibView.Format = (indexBuffer->GetStride() == 2) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
		ibView.SizeInBytes = indexBuffer->GetSizeInBytes();
		ibView.BufferLocation = indexBuffer->GetNativeBufferPtr()->GetGPUVirtualAddress();
		commandList->IASetIndexBuffer(&ibView);

		// コマンド「ここまでの設定を使って描画を開始する」をコマンドリストに追加
		commandList->DrawIndexedInstanced(indexBuffer->GetCount(), 1, 0, 0, 0);
	}
}