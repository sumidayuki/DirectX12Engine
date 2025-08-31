#include "DescriptorAllocator.h"

DescriptorAllocator::DescriptorAllocator(UINT capacity) : m_capacity(capacity)
{
    ID3D12Device* device = Graphics::GetD3D12Device();

    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    heapDesc.NumDescriptors = m_capacity;
    heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_heap));

    m_cpuStart = m_heap->GetCPUDescriptorHandleForHeapStart();
    m_gpuStart = m_heap->GetGPUDescriptorHandleForHeapStart();
    m_handleIncrementSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorAllocator::CreateSrv(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc)
{
    if (m_currentIndex >= m_capacity) {
        // —e—Ê•s‘«
        assert(false);
        return {};
    }

    ID3D12Device* device = Graphics::GetD3D12Device();

    D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_cpuStart;
    cpuHandle.ptr += (size_t)m_currentIndex * m_handleIncrementSize;

    device->CreateShaderResourceView(resource, &srvDesc, cpuHandle);

    D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_gpuStart;
    gpuHandle.ptr += (size_t)m_currentIndex * m_handleIncrementSize;

    m_currentIndex++;

    return gpuHandle;
}