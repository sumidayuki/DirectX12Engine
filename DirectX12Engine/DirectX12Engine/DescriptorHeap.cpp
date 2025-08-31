#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(DescriptorHeapType heapType, int count, bool isShaderVisible)
    : m_heapType(heapType)
    , m_stride(0)
    , m_count(count)
    , m_isShaderVisible(isShaderVisible)
    , m_nativeHeap(nullptr)
{
    // ID3D12デバイスを取得
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // ディスクリプタヒープ作成時の詳細情報
    D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
    memset(&dhDesc, 0, sizeof(dhDesc));
    dhDesc.NumDescriptors = m_count;
    dhDesc.NodeMask = 0;
    dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // シェーダ可視性
    if (m_isShaderVisible)
    {
        dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }

    // ディスクリプタヒープの種類
    switch (heapType)
    {
        case DescriptorHeapType::CBV_SRV_UAV:   dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; break;
        case DescriptorHeapType::RTV:           dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
        case DescriptorHeapType::DSV:           dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
        case DescriptorHeapType::SAMPLER:       dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; break;
        default: assert(0); break;
    }

    // ディスクリプタヒープを作成
    if (d3d12Device->CreateDescriptorHeap(&dhDesc, IID_ID3D12DescriptorHeap, (void**)m_nativeHeap.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // ディスクリプタ1個分のサイズ（バイト数）を取得
    m_stride = d3d12Device->GetDescriptorHandleIncrementSize(dhDesc.Type);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(int index) const
{
    // CPU側（システムメモリ上）に作成された記憶領域の先頭アドレスを取得
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_nativeHeap->GetCPUDescriptorHandleForHeapStart();

    // そのアドレスをディスクリプタindex個分だけずらす
    handle.ptr += m_stride * index;

    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(int index) const
{
    // GPU側（ビデオメモリ上）に作成された記憶領域の先頭アドレスを取得
    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_nativeHeap->GetGPUDescriptorHandleForHeapStart();

    // そのアドレスをディスクリプタindex個分だけずらす
    handle.ptr += m_stride * index;

    return handle;
}
