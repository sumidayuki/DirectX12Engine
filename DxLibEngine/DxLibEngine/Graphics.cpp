#include "Graphics.h"

// ライブラリをリンクする
#pragma comment(lib, "d3d12.lib")       // Direct3D12
#pragma comment(lib, "dxguid.lib")      // DirectX Global Unique IDentifier
#pragma comment(lib, "dxgi.lib")        // DirectX Graphics Infrastructure
#pragma comment(lib, "Shlwapi.lib")     // StrStrIW()

bool Graphics::StaticConstructor(HWND hWnd, const Resolution& resolution)
{
	// D3D12デバッグインターフェイスを取得します。
	ComPtr<ID3D12Debug> d3d12Debug;
	if (FAILED(D3D12GetDebugInterface(IID_ID3D12Debug, (void**)d3d12Debug.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}
	else
	{
		// デバッグ機能を有効にする
		d3d12Debug->EnableDebugLayer();
	}

	//-------------------------------------------------------------------------------------
	// DXGIファクトリの作成
	//-------------------------------------------------------------------------------------
	ComPtr<IDXGIFactory4> dxgiFactory4;
	if (FAILED(CreateDXGIFactory(IID_IDXGIFactory4, (void**)dxgiFactory4.ReleaseAndGetAddressOf())))
	{
		assert(0);
		return false;
	}

	// アダプタの列挙
	UINT adapterIndex = 0;

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapterMeetsRequirements;
	while (SUCCEEDED(dxgiFactory4->EnumAdapters1(adapterIndex, dxgiAdapter1.ReleaseAndGetAddressOf())))
	{
		// 上位インターフェイスの取得
		ComPtr<IDXGIAdapter4> dxgiAdapter4;
		if (FAILED(dxgiAdapter1->QueryInterface(dxgiAdapter4.ReleaseAndGetAddressOf())))
		{
			assert(0);
		}

		// アダプタの詳細情報の取得
		DXGI_ADAPTER_DESC3 adapterDesc;
		if (FAILED(dxgiAdapter4->GetDesc3(&adapterDesc)))
		{
			assert(0);
		}

		// Intel社製の統合GPUはD3D12デバイス作成に失敗する可能性があるので選ばないようにする
		if (!StrStrIW(adapterDesc.Description, L"Intel"))
		{
			dxgiAdapterMeetsRequirements = dxgiAdapter4;
			break;
		}

		// 次のアダプタへ
		++adapterIndex;
	}

	// 最適なアダプタは見つかったか？
	if (dxgiAdapterMeetsRequirements)
	{
		DXGI_ADAPTER_DESC3 desc;
		if (FAILED(dxgiAdapterMeetsRequirements->GetDesc3(&desc)))
		{
			assert(0);
		}

		// ワイド文字列からマルチバイト文字列への変換
		char adapterDescription[256];
		size_t numOfCharConverted;
		wcstombs_s(&numOfCharConverted, adapterDescription, desc.Description, sizeof(adapterDescription));
	}
	else
	{
		assert(0);
		return false;
	}

	// Direct3D12デバイスを作成
	if (FAILED(D3D12CreateDevice(dxgiAdapterMeetsRequirements.Get(), D3D_FEATURE_LEVEL_12_0, IID_ID3D12Device, (void**)m_d3d12Device.ReleaseAndGetAddressOf())))
	{
		assert(0);
		return false;
	}

	//-------------------------------------------------------------------------------------
	// コマンドキュー作成
	//-------------------------------------------------------------------------------------

	// コマンドキュー作成時の詳細情報
	D3D12_COMMAND_QUEUE_DESC cqDesc;
	memset(&cqDesc, 0, sizeof(cqDesc));
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cqDesc.Priority = 0;
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.NodeMask = 0;

	// コマンドキューを作成
	if (FAILED(m_d3d12Device->CreateCommandQueue(&cqDesc, IID_ID3D12CommandQueue, (void**)m_directCommandQueue.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	//-------------------------------------------------------------------------------------
	// スワップチェーン作成
	//-------------------------------------------------------------------------------------

	// スワップチェーン作成時の詳細情報
	DXGI_SWAP_CHAIN_DESC scDesc;
	memset(&scDesc, 0, sizeof(scDesc));
	scDesc.BufferCount = BackBafferCount;
	scDesc.BufferDesc.Width = resolution.width;
	scDesc.BufferDesc.Height = resolution.height;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferDesc.RefreshRate.Denominator = 1;
	scDesc.BufferDesc.RefreshRate.Numerator = 60;
	scDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_STRETCHED;
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.Windowed = TRUE;
	scDesc.OutputWindow = hWnd;
	scDesc.SampleDesc.Count = 1;
	scDesc.SampleDesc.Quality = 0;
	scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	scDesc.Flags = 0;

	// スワップチェーン（バージョン0）を作成
	ComPtr<IDXGISwapChain> dxgiSwapChain;
	if (FAILED(dxgiFactory4->CreateSwapChain(m_directCommandQueue.Get(), &scDesc, dxgiSwapChain.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// スワップチェーンのインターフェイスバージョンを変更
	if (FAILED(dxgiSwapChain->QueryInterface(IID_IDXGISwapChain3, (void**)m_dxgiSwapChain.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// RTV用のディスクリプタヒープ作成時の詳細情報
	m_descriptorHeapRTV.Attach(new DescriptorHeap(DescriptorHeapType::RTV, BackBafferCount, false));

	// バックバッファの枚数分だけRTVを作成
	for (int i = 0; i < BackBafferCount; i++)
	{
		// スワップチェーンが内部で管理しているi番目のバックバッファを取得
		ComPtr<ID3D12Resource> backBuffer;
		if (FAILED(m_dxgiSwapChain->GetBuffer(i, IID_ID3D12Resource, (void**)backBuffer.ReleaseAndGetAddressOf())))
		{
			assert(0);
		}

		// RTVを作成する位置
		const D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descriptorHeapRTV->GetCPUDescriptorHandle(i);

		// ディスクリプタヒープ内にRTVを作成
		m_d3d12Device->CreateRenderTargetView(backBuffer.Get(), nullptr, handle);

		// フレームリソースの作成
		m_frameResource[i].Attach(new FrameResource(backBuffer.Get(), handle));
	}

	return true;
}

void Graphics::StaticDestructor()
{
	// ComPtr系の変数は nullptr を代入することで参照カウントが1減らされる。
	for (int i = 0; i < BackBafferCount; i++)
	{
		m_frameResource[i] = nullptr;
	}

	m_descriptorHeapRTV = nullptr;
	m_dxgiSwapChain = nullptr;
	m_directCommandQueue = nullptr;
	m_d3d12Device = nullptr;
}

FrameResource* Graphics::GetCurrentFrameResource()
{
	// 「何番目のバックバッファ」にレンダリングすればよいのかをスワップチェーンに教えてもらう。
	const UINT currentIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// currentIndex番目のフレームリソースを返す。
	return m_frameResource[currentIndex].Get();
}

void Graphics::WaitForCompletionOfPresent()
{
	GetCurrentFrameResource()->WaitUntilReadyToRender();
}
