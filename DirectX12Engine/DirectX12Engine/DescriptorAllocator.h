#pragma once

class DescriptorAllocator
{
private:
    ComPtr<ID3D12DescriptorHeap> m_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;
    UINT m_handleIncrementSize;
    UINT m_currentIndex = 0;
    UINT m_capacity;

public:
    DescriptorAllocator(UINT capacity);

    // SRVÇçÏê¨ÇµÅAÇªÇÃGPUÉnÉìÉhÉãÇï‘Ç∑
    D3D12_GPU_DESCRIPTOR_HANDLE CreateSrv(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
};