#pragma once

/// <summary>
/// 各種ディスクリプタヒープ(CBV/SRV/UAV, RTV, DSV, SAMPLER)を
/// フレームリングバッファ方式で管理するアロケーター。
/// </summary>
class DescriptorAllocator
{
private:
    ComPtr<ID3D12DescriptorHeap> m_heap;
    D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStart;
    D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStart;
    UINT m_handleIncrementSize;

    DescriptorHeapType m_type;

    UINT m_capacity;
    UINT m_frameCount;
    UINT m_descriptorsPerFrame;

    UINT m_currentFrameIndex;
    UINT m_currentOffsetInFrame;

public:
    /// <summary>
    /// 指定容量のディスクリプタヒープを作成します。
    /// </summary>
    /// <param name="capacity">1フレーム当たりのディスクリプタ数</param>
    /// <param name="frameCount">フレーム数(バックバッファ数)</param>
    /// <param name="type">ディスクリプタヒープタイプ</param>
    DescriptorAllocator(UINT capacity, UINT frameCount, DescriptorHeapType type);

    /// <summary>
    /// 新しいフレームを開始し、ヒープ内の領域を切り替えます。
    /// </summary>
    /// <param name="frameIndex"></param>
    void BeginFrame(UINT frameIndex);

    /// <summary>
    /// SRVを作成し、そのGPUハンドルを返します。
    /// この関数は CBV_SRV_UAV ヒープのみで有効。
    /// </summary>
    /// <param name="resource"></param>
    /// <param name="srvDesc"></param>
    /// <returns></returns>
    D3D12_GPU_DESCRIPTOR_HANDLE CreateSRV(ID3D12Resource* resource, const D3D12_SHADER_RESOURCE_VIEW_DESC& srvDesc);

    ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
    D3D12_GPU_DESCRIPTOR_HANDLE GetGPUStart() const { return m_gpuStart; }
    D3D12_CPU_DESCRIPTOR_HANDLE GetCPUStart() const { return m_cpuStart; }

    UINT GetHandleIncrementSize() const { return m_handleIncrementSize; }
    DescriptorHeapType GetType() const { return m_type; }
};