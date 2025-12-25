#include "DescriptorAllocator.h"

DescriptorAllocator::DescriptorAllocator(UINT capacity, UINT frameCount, DescriptorHeapType type)
    : m_type(type)
    , m_frameCount(frameCount)
    , m_descriptorsPerFrame(capacity)
    , m_capacity(capacity* frameCount)
    , m_currentFrameIndex(0)
    , m_currentOffsetInFrame(0)
{
    ID3D12Device* device = Graphics::GetD3D12Device();

    // ヒープタイプを選択
    D3D12_DESCRIPTOR_HEAP_TYPE dxType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    switch (m_type)
    {
    case DescriptorHeapType::CBV_SRV_UAV:
        dxType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
        flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        break;

    case DescriptorHeapType::RTV:
        dxType = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        break;

    case DescriptorHeapType::DSV:
        dxType = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
        break;

    case DescriptorHeapType::SAMPLER:
        dxType = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER;
        flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
        break;
    }

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = dxType;
    heapDesc.NumDescriptors = m_capacity;
    heapDesc.Flags = flags;
    heapDesc.NodeMask = 0;

    m_heap.Attach(new DescriptorHeap(type, m_capacity, flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

    m_handleIncrementSize = device->GetDescriptorHandleIncrementSize(dxType);

    // ハンドル初期化
    m_cpuStart = m_heap->GetCPUDescriptorHandle(0);
    if (flags == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
    {
        m_gpuStart = m_heap->GetGPUDescriptorHandle(0);
    }
    else
    {
        m_gpuStart = {};
    }
}

void DescriptorAllocator::BeginFrame(UINT frameIndex)
{
    m_currentFrameIndex = frameIndex % m_frameCount;
    m_currentOffsetInFrame = 0;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc, D3D12_CPU_DESCRIPTOR_HANDLE* outCpuHandle)
{
    if (m_currentOffsetInFrame >= m_descriptorsPerFrame) {
        // 容量不足
        assert(false);
        if (outCpuHandle) *outCpuHandle = {};
        return {};
    }

    ID3D12Device* device = Graphics::GetD3D12Device();

    // 現在のフレームの領域内での絶対オフセットを計算
    UINT absoluteOffset = (m_currentFrameIndex * m_descriptorsPerFrame) + m_currentOffsetInFrame;

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_cpuStart;
    cpuHandle.ptr += static_cast<SIZE_T>(absoluteOffset) * m_handleIncrementSize;

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_gpuStart;
    gpuHandle.ptr += static_cast<SIZE_T>(absoluteOffset) * m_handleIncrementSize;

    // SRVを作成
    device->CreateShaderResourceView(resource, &srvDesc, cpuHandle);

    if (outCpuHandle) // <- 追加: CPUハンドルが必要な場合は格納
    {
        *outCpuHandle = cpuHandle;
    }

    ++m_currentOffsetInFrame;

    return gpuHandle;
}