#include "DescriptorHeap.h"

DescriptorHeap::DescriptorHeap(DescriptorHeapType heapType, int count, bool isShaderVisible)
    : m_heapType(heapType)
    , m_stride(0)
    , m_count(count)
    , m_isShaderVisible(isShaderVisible)
    , m_nativeHeap(nullptr)
{
    // ID3D12�f�o�C�X���擾
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    // �f�B�X�N���v�^�q�[�v�쐬���̏ڍ׏��
    D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
    memset(&dhDesc, 0, sizeof(dhDesc));
    dhDesc.NumDescriptors = m_count;
    dhDesc.NodeMask = 0;
    dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

    // �V�F�[�_����
    if (m_isShaderVisible)
    {
        dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    }

    // �f�B�X�N���v�^�q�[�v�̎��
    switch (heapType)
    {
        case DescriptorHeapType::CBV_SRV_UAV:   dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; break;
        case DescriptorHeapType::RTV:           dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; break;
        case DescriptorHeapType::DSV:           dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV; break;
        case DescriptorHeapType::SAMPLER:       dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER; break;
        default: assert(0); break;
    }

    // �f�B�X�N���v�^�q�[�v���쐬
    if (d3d12Device->CreateDescriptorHeap(&dhDesc, IID_ID3D12DescriptorHeap, (void**)m_nativeHeap.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // �f�B�X�N���v�^1���̃T�C�Y�i�o�C�g���j���擾
    m_stride = d3d12Device->GetDescriptorHandleIncrementSize(dhDesc.Type);
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUDescriptorHandle(int index) const
{
    // CPU���i�V�X�e����������j�ɍ쐬���ꂽ�L���̈�̐擪�A�h���X���擾
    D3D12_CPU_DESCRIPTOR_HANDLE handle = m_nativeHeap->GetCPUDescriptorHandleForHeapStart();

    // ���̃A�h���X���f�B�X�N���v�^index���������炷
    handle.ptr += m_stride * index;

    return handle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUDescriptorHandle(int index) const
{
    // GPU���i�r�f�I��������j�ɍ쐬���ꂽ�L���̈�̐擪�A�h���X���擾
    D3D12_GPU_DESCRIPTOR_HANDLE handle = m_nativeHeap->GetGPUDescriptorHandleForHeapStart();

    // ���̃A�h���X���f�B�X�N���v�^index���������炷
    handle.ptr += m_stride * index;

    return handle;
}
