#include "FrameResource.h"

FrameResource::FrameResource(UINT frameIndex, ID3D12Resource* backBuffer, D3D12_CPU_DESCRIPTOR_HANDLE handleRTV)
	: m_frameIndex(frameIndex)
	, m_backBuffer(backBuffer)
	, m_handleRTV(handleRTV)
	, m_descriptorHeapDSV(nullptr)
	, m_commandList(nullptr)
	, m_commandAllocator(nullptr)
	, m_fence(nullptr)
	, m_fenceEvent(nullptr)
{
	// Direct3D12デバイスを取得
	ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

	//------------------------------------------------------------------------------------
	// 深度ステンシルバッファの作成
	//------------------------------------------------------------------------------------

	D3D12_HEAP_PROPERTIES heapProps;
	memset(&heapProps, 0, sizeof(heapProps));						// ゼロクリア
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;						// ビデオメモリ上に作成する
	heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapProps.CreationNodeMask = 0;
	heapProps.VisibleNodeMask = 0;

	D3D12_RESOURCE_DESC resDesc;
	memset(&resDesc, 0, sizeof(resDesc));
	resDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	resDesc.Width = Screen::GetWidth();
	resDesc.Height = Screen::GetHeight();
	resDesc.Format = DXGI_FORMAT_D32_FLOAT;
	resDesc.DepthOrArraySize = 1;
	resDesc.MipLevels = 1;
	resDesc.SampleDesc.Count = 1;
	resDesc.SampleDesc.Quality = 0;
	resDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	resDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	resDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	// 深度ステンシルバッファのクリア時の値
	D3D12_CLEAR_VALUE clearValue;
	memset(&clearValue, 0, sizeof(clearValue));
	clearValue.Format = resDesc.Format;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	// バッファまたはテクスチャのリソースを作成
	if (FAILED(d3d12Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_ID3D12Resource, (void**)m_depthStencilBuffer.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	//-------------------------------------------------------------------------------------
	// ディスクリプタヒープの作成 ＆ ディスクリプタ（深度ステンシルビュー）の作成
	//-------------------------------------------------------------------------------------
	m_descriptorHeapDSV.Attach(new DescriptorHeap(DescriptorHeapType::DSV, 1, false));

	// ディスクリプタの作成先となるCPU側アドレス
	m_handleDSV = m_descriptorHeapDSV->GetCPUDescriptorHandle(0);

	// 深度ステンシルビュー（DSV）の作成
	d3d12Device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_handleDSV);

	// コマンドアロケーターを作成
	if (FAILED(d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_ID3D12CommandAllocator, (void**)m_commandAllocator.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// コマンドリストを作成
	if (FAILED(d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_ID3D12GraphicsCommandList, (void**)m_commandList.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// 「完了値」の初期値
	const UINT64 initialCompletedValue = 1;

	// フェンスを作成
	if (FAILED(d3d12Device->CreateFence(initialCompletedValue, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, (void**)m_fence.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// フェンス用の通知イベントオブジェクトを作成
	m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

	// エラーチェック
	if (!m_fenceEvent)
	{
		assert(0);
	}
}

void FrameResource::Update()
{
	SceneManager::Update();
}

void FrameResource::Render()
{
	WaitUntilReadyToRender();

	SceneManager::Draw();

	//------------------------------------------------------------------------------------
	// コマンド「バックバッファがプレゼント状態に遷移するまで待つ」をコマンドリストに追加する。
	//------------------------------------------------------------------------------------
	D3D12_RESOURCE_BARRIER barriers[1];
	memset(barriers, 0, sizeof(barriers));
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = m_backBuffer.Get();
	barriers[0].Transition.Subresource = 0;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_commandList->ResourceBarrier(_countof(barriers), barriers);

	// コマンドを実行して完了を待ちます。
	ExecuteCommandBuffer();

	// バックバッファの内容を Windows に渡してウィンドウ上にゲーム画面を表示
	if (FAILED(Graphics::GetSwapChain()->Present(0, 0)))
	{
		assert(0);
	}
}

void FrameResource::AddEventAfterGPUCompletion(const GPUCompletionEventHandler& eventHandler)
{
	m_GPUCompletionEventHandlers.push_back(eventHandler);
}

void FrameResource::WaitUntilReadyToRender()
{
	// コマンド「バックバッファがレンダリング可能状態に遷移するまで待つ」をコマンドリストに追加
	D3D12_RESOURCE_BARRIER barriers[1];
	memset(barriers, 0, sizeof(barriers));
	barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barriers[0].Transition.pResource = m_backBuffer.Get();
	barriers[0].Transition.Subresource = 0;
	barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	m_commandList->ResourceBarrier(_countof(barriers), barriers);

	ExecuteCommandBuffer();
}

void FrameResource::ExecuteCommandBuffer()
{
	// コマンドリストを閉じて新規コマンドの追加を禁止します。
	if (FAILED(m_commandList->Close()))
	{
		assert(0);
	}

	// コマンドリスト配列の先頭の要素から順にGPUにコマンドを送信します。
	ID3D12CommandList* const commandLists[] =
	{
		m_commandList.Get(),
	};
	Graphics::GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

	// GPUに最後に送ったコマンドの後ろにフェンスを設置します。
	
	// 今回の完了値 = 前回の完了値 + 1
	const UINT64 completionValue = m_fence->GetCompletedValue() + 1;
	if (FAILED(Graphics::GetCommandQueue()->Signal(m_fence.Get(), completionValue)))
	{
		assert(0);
	}

	// フェンス内の完了値が上書きされたか？
	if (m_fence->GetCompletedValue() < completionValue)
	{
		// フェンス内の完了値が目標値以上になった際に発生するイベントを設定
		if (FAILED(m_fence->SetEventOnCompletion(completionValue, m_fenceEvent)))
		{
			assert(0);
		}

		// イベントオブジェクトがシグナル状態になるまでスレッドを待機状態にする
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//------------------------------------------------------------------------------------
	// 全てのイベントハンドラを起動してリストを空にする
	//------------------------------------------------------------------------------------
	for (auto& eventHandler : m_GPUCompletionEventHandlers)
	{
		eventHandler();
	}
	m_GPUCompletionEventHandlers.clear();

	// 今回のレンダリングに使用したコマンドの記憶領域を回収し、次回のレンダリングに再利用できるようにする
	if (FAILED(m_commandAllocator->Reset()))
	{
		assert(0);
	}

	// コマンドリストの内部データを初期化し、次回のレンダリングに再利用できるようにする
	if (FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr)))
	{
		assert(0);
	}
}