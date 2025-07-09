#include "Graphics.h"

// ���C�u�����������N����
#pragma comment(lib, "d3d12.lib")       // Direct3D12
#pragma comment(lib, "dxguid.lib")      // DirectX Global Unique IDentifier
#pragma comment(lib, "dxgi.lib")        // DirectX Graphics Infrastructure
#pragma comment(lib, "Shlwapi.lib")     // StrStrIW()

bool Graphics::StaticConstructor(HWND hWnd, const Resolution& resolution)
{
	// D3D12�f�o�b�O�C���^�[�t�F�C�X���擾���܂��B
	ComPtr<ID3D12Debug> d3d12Debug;
	if (FAILED(D3D12GetDebugInterface(IID_ID3D12Debug, (void**)d3d12Debug.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}
	else
	{
		// �f�o�b�O�@�\��L���ɂ���
		d3d12Debug->EnableDebugLayer();
	}

	//-------------------------------------------------------------------------------------
	// DXGI�t�@�N�g���̍쐬
	//-------------------------------------------------------------------------------------
	ComPtr<IDXGIFactory4> dxgiFactory4;
	if (FAILED(CreateDXGIFactory(IID_IDXGIFactory4, (void**)dxgiFactory4.ReleaseAndGetAddressOf())))
	{
		assert(0);
		return false;
	}

	// �A�_�v�^�̗�
	UINT adapterIndex = 0;

	ComPtr<IDXGIAdapter1> dxgiAdapter1;
	ComPtr<IDXGIAdapter4> dxgiAdapterMeetsRequirements;
	while (SUCCEEDED(dxgiFactory4->EnumAdapters1(adapterIndex, dxgiAdapter1.ReleaseAndGetAddressOf())))
	{
		// ��ʃC���^�[�t�F�C�X�̎擾
		ComPtr<IDXGIAdapter4> dxgiAdapter4;
		if (FAILED(dxgiAdapter1->QueryInterface(dxgiAdapter4.ReleaseAndGetAddressOf())))
		{
			assert(0);
		}

		// �A�_�v�^�̏ڍ׏��̎擾
		DXGI_ADAPTER_DESC3 adapterDesc;
		if (FAILED(dxgiAdapter4->GetDesc3(&adapterDesc)))
		{
			assert(0);
		}

		// Intel�А��̓���GPU��D3D12�f�o�C�X�쐬�Ɏ��s����\��������̂őI�΂Ȃ��悤�ɂ���
		if (!StrStrIW(adapterDesc.Description, L"Intel"))
		{
			dxgiAdapterMeetsRequirements = dxgiAdapter4;
			break;
		}

		// ���̃A�_�v�^��
		++adapterIndex;
	}

	// �œK�ȃA�_�v�^�͌����������H
	if (dxgiAdapterMeetsRequirements)
	{
		DXGI_ADAPTER_DESC3 desc;
		if (FAILED(dxgiAdapterMeetsRequirements->GetDesc3(&desc)))
		{
			assert(0);
		}

		// ���C�h�����񂩂�}���`�o�C�g������ւ̕ϊ�
		char adapterDescription[256];
		size_t numOfCharConverted;
		wcstombs_s(&numOfCharConverted, adapterDescription, desc.Description, sizeof(adapterDescription));
	}
	else
	{
		assert(0);
		return false;
	}

	// Direct3D12�f�o�C�X���쐬
	if (FAILED(D3D12CreateDevice(dxgiAdapterMeetsRequirements.Get(), D3D_FEATURE_LEVEL_12_0, IID_ID3D12Device, (void**)m_d3d12Device.ReleaseAndGetAddressOf())))
	{
		assert(0);
		return false;
	}

	//-------------------------------------------------------------------------------------
	// �R�}���h�L���[�쐬
	//-------------------------------------------------------------------------------------

	// �R�}���h�L���[�쐬���̏ڍ׏��
	D3D12_COMMAND_QUEUE_DESC cqDesc;
	memset(&cqDesc, 0, sizeof(cqDesc));
	cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	cqDesc.Priority = 0;
	cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	cqDesc.NodeMask = 0;

	// �R�}���h�L���[���쐬
	if (FAILED(m_d3d12Device->CreateCommandQueue(&cqDesc, IID_ID3D12CommandQueue, (void**)m_directCommandQueue.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	//-------------------------------------------------------------------------------------
	// �X���b�v�`�F�[���쐬
	//-------------------------------------------------------------------------------------

	// �X���b�v�`�F�[���쐬���̏ڍ׏��
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

	// �X���b�v�`�F�[���i�o�[�W����0�j���쐬
	ComPtr<IDXGISwapChain> dxgiSwapChain;
	if (FAILED(dxgiFactory4->CreateSwapChain(m_directCommandQueue.Get(), &scDesc, dxgiSwapChain.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// �X���b�v�`�F�[���̃C���^�[�t�F�C�X�o�[�W������ύX
	if (FAILED(dxgiSwapChain->QueryInterface(IID_IDXGISwapChain3, (void**)m_dxgiSwapChain.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// RTV�p�̃f�B�X�N���v�^�q�[�v�쐬���̏ڍ׏��
	m_descriptorHeapRTV.Attach(new DescriptorHeap(DescriptorHeapType::RTV, BackBafferCount, false));

	// �o�b�N�o�b�t�@�̖���������RTV���쐬
	for (int i = 0; i < BackBafferCount; i++)
	{
		// �X���b�v�`�F�[���������ŊǗ����Ă���i�Ԗڂ̃o�b�N�o�b�t�@���擾
		ComPtr<ID3D12Resource> backBuffer;
		if (FAILED(m_dxgiSwapChain->GetBuffer(i, IID_ID3D12Resource, (void**)backBuffer.ReleaseAndGetAddressOf())))
		{
			assert(0);
		}

		// RTV���쐬����ʒu
		const D3D12_CPU_DESCRIPTOR_HANDLE handle = m_descriptorHeapRTV->GetCPUDescriptorHandle(i);

		// �f�B�X�N���v�^�q�[�v����RTV���쐬
		m_d3d12Device->CreateRenderTargetView(backBuffer.Get(), nullptr, handle);

		// �t���[�����\�[�X�̍쐬
		m_frameResource[i].Attach(new FrameResource(backBuffer.Get(), handle));
	}

	return true;
}

void Graphics::StaticDestructor()
{
	// ComPtr�n�̕ϐ��� nullptr �������邱�ƂŎQ�ƃJ�E���g��1���炳���B
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
	// �u���Ԗڂ̃o�b�N�o�b�t�@�v�Ƀ����_�����O����΂悢�̂����X���b�v�`�F�[���ɋ����Ă��炤�B
	const UINT currentIndex = m_dxgiSwapChain->GetCurrentBackBufferIndex();

	// currentIndex�Ԗڂ̃t���[�����\�[�X��Ԃ��B
	return m_frameResource[currentIndex].Get();
}

void Graphics::WaitForCompletionOfPresent()
{
	GetCurrentFrameResource()->WaitUntilReadyToRender();
}
