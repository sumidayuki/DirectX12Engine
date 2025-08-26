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
	// Direct3D12�f�o�C�X���擾
	ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

	//------------------------------------------------------------------------------------
	// �[�x�X�e���V���o�b�t�@�̍쐬
	//------------------------------------------------------------------------------------

	D3D12_HEAP_PROPERTIES heapProps;
	memset(&heapProps, 0, sizeof(heapProps));						// �[���N���A
	heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;						// �r�f�I��������ɍ쐬����
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

	// �[�x�X�e���V���o�b�t�@�̃N���A���̒l
	D3D12_CLEAR_VALUE clearValue;
	memset(&clearValue, 0, sizeof(clearValue));
	clearValue.Format = resDesc.Format;
	clearValue.DepthStencil.Depth = 1.0f;
	clearValue.DepthStencil.Stencil = 0;

	// �o�b�t�@�܂��̓e�N�X�`���̃��\�[�X���쐬
	if (FAILED(d3d12Device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &clearValue, IID_ID3D12Resource, (void**)m_depthStencilBuffer.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	//-------------------------------------------------------------------------------------
	// �f�B�X�N���v�^�q�[�v�̍쐬 �� �f�B�X�N���v�^�i�[�x�X�e���V���r���[�j�̍쐬
	//-------------------------------------------------------------------------------------
	m_descriptorHeapDSV.Attach(new DescriptorHeap(DescriptorHeapType::DSV, 1, false));

	// �f�B�X�N���v�^�̍쐬��ƂȂ�CPU���A�h���X
	m_handleDSV = m_descriptorHeapDSV->GetCPUDescriptorHandle(0);

	// �[�x�X�e���V���r���[�iDSV�j�̍쐬
	d3d12Device->CreateDepthStencilView(m_depthStencilBuffer.Get(), nullptr, m_handleDSV);

	// �R�}���h�A���P�[�^�[���쐬
	if (FAILED(d3d12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_ID3D12CommandAllocator, (void**)m_commandAllocator.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// �R�}���h���X�g���쐬
	if (FAILED(d3d12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_ID3D12GraphicsCommandList, (void**)m_commandList.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// �u�����l�v�̏����l
	const UINT64 initialCompletedValue = 1;

	// �t�F���X���쐬
	if (FAILED(d3d12Device->CreateFence(initialCompletedValue, D3D12_FENCE_FLAG_NONE, IID_ID3D12Fence, (void**)m_fence.ReleaseAndGetAddressOf())))
	{
		assert(0);
	}

	// �t�F���X�p�̒ʒm�C�x���g�I�u�W�F�N�g���쐬
	m_fenceEvent = ::CreateEvent(nullptr, FALSE, FALSE, nullptr);

	// �G���[�`�F�b�N
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
	// �R�}���h�u�o�b�N�o�b�t�@���v���[���g��ԂɑJ�ڂ���܂ő҂v���R�}���h���X�g�ɒǉ�����B
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

	// �R�}���h�����s���Ċ�����҂��܂��B
	ExecuteCommandBuffer();

	// �o�b�N�o�b�t�@�̓��e�� Windows �ɓn���ăE�B���h�E��ɃQ�[����ʂ�\��
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
	// �R�}���h�u�o�b�N�o�b�t�@�������_�����O�\��ԂɑJ�ڂ���܂ő҂v���R�}���h���X�g�ɒǉ�
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
	// �R�}���h���X�g����ĐV�K�R�}���h�̒ǉ����֎~���܂��B
	if (FAILED(m_commandList->Close()))
	{
		assert(0);
	}

	// �R�}���h���X�g�z��̐擪�̗v�f���珇��GPU�ɃR�}���h�𑗐M���܂��B
	ID3D12CommandList* const commandLists[] =
	{
		m_commandList.Get(),
	};
	Graphics::GetCommandQueue()->ExecuteCommandLists(_countof(commandLists), commandLists);

	// GPU�ɍŌ�ɑ������R�}���h�̌��Ƀt�F���X��ݒu���܂��B
	
	// ����̊����l = �O��̊����l + 1
	const UINT64 completionValue = m_fence->GetCompletedValue() + 1;
	if (FAILED(Graphics::GetCommandQueue()->Signal(m_fence.Get(), completionValue)))
	{
		assert(0);
	}

	// �t�F���X���̊����l���㏑�����ꂽ���H
	if (m_fence->GetCompletedValue() < completionValue)
	{
		// �t�F���X���̊����l���ڕW�l�ȏ�ɂȂ����ۂɔ�������C�x���g��ݒ�
		if (FAILED(m_fence->SetEventOnCompletion(completionValue, m_fenceEvent)))
		{
			assert(0);
		}

		// �C�x���g�I�u�W�F�N�g���V�O�i����ԂɂȂ�܂ŃX���b�h��ҋ@��Ԃɂ���
		::WaitForSingleObject(m_fenceEvent, INFINITE);
	}

	//------------------------------------------------------------------------------------
	// �S�ẴC�x���g�n���h�����N�����ă��X�g����ɂ���
	//------------------------------------------------------------------------------------
	for (auto& eventHandler : m_GPUCompletionEventHandlers)
	{
		eventHandler();
	}
	m_GPUCompletionEventHandlers.clear();

	// ����̃����_�����O�Ɏg�p�����R�}���h�̋L���̈��������A����̃����_�����O�ɍė��p�ł���悤�ɂ���
	if (FAILED(m_commandAllocator->Reset()))
	{
		assert(0);
	}

	// �R�}���h���X�g�̓����f�[�^�����������A����̃����_�����O�ɍė��p�ł���悤�ɂ���
	if (FAILED(m_commandList->Reset(m_commandAllocator.Get(), nullptr)))
	{
		assert(0);
	}
}