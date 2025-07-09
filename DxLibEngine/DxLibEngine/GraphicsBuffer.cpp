#include "GraphicsBuffer.h"

ID3D12Resource* GraphicsBuffer::CreateDefaultBuffer(int size, D3D12_RESOURCE_STATES initialState)
{
    // �q�[�v�v���p�e�B���
    D3D12_HEAP_PROPERTIES heapProps;
    memset(&heapProps, 0, sizeof(heapProps));
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 0;
    heapProps.VisibleNodeMask = 0;

    // ���\�[�X�쐬���̏ڍ׏��
    D3D12_RESOURCE_DESC resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = size;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.Format = DXGI_FORMAT_UNKNOWN;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.SampleDesc.Quality = 0;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // Direct3D12�̃o�b�t�@���\�[�X���쐬
    ID3D12Resource* nativeBuffer;
    if (Graphics::GetD3D12Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_CREATE_NOT_ZEROED, &resDesc, initialState, nullptr, IID_ID3D12Resource, (void**)&nativeBuffer) < 0)
    {
        assert(0);
    }

    return nativeBuffer;
}

ID3D12Resource* GraphicsBuffer::CreateUploadBuffer(int size)
{
    // �q�[�v�v���p�e�B���
    D3D12_HEAP_PROPERTIES heapProps;
    memset(&heapProps, 0, sizeof(heapProps));
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 0;
    heapProps.VisibleNodeMask = 0;

    // ���\�[�X�쐬���̏ڍ׏��
    D3D12_RESOURCE_DESC resDesc;
    memset(&resDesc, 0, sizeof(resDesc));
    resDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    resDesc.Width = size;
    resDesc.Height = 1;
    resDesc.DepthOrArraySize = 1;
    resDesc.Format = DXGI_FORMAT_UNKNOWN;
    resDesc.MipLevels = 1;
    resDesc.SampleDesc.Count = 1;
    resDesc.SampleDesc.Quality = 0;
    resDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    resDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
    resDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    // �o�b�t�@���\�[�X�̍쐬
    ID3D12Resource* uplodeBuffer;
    if (Graphics::GetD3D12Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_CREATE_NOT_ZEROED, &resDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_ID3D12Resource, (void**)&uplodeBuffer) < 0)
    {
        assert(0);
    }

    return uplodeBuffer;
}

D3D12_RESOURCE_STATES GraphicsBuffer::GetOriginallyResourceState(Target target, UsageFlags usageFlags)
{
    D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON;
    if (usageFlags == UsageFlags::LockBufferForWrite)
    {
        initialState |= D3D12_RESOURCE_STATE_GENERIC_READ;
    }
    else
    {
        switch (target)
        {
        case Target::Vertex:
        case Target::Constant:
            initialState |= D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
            break;

        case Target::Index:
            initialState |= D3D12_RESOURCE_STATE_INDEX_BUFFER;
            break;

        case Target::Structured:
            initialState |= D3D12_RESOURCE_STATE_UNORDERED_ACCESS;
            break;
        }
    }

    return initialState;
}

GraphicsBuffer::GraphicsBuffer(Target target, UsageFlags usageFlags, int count, int stride, const void* initialData)
    : m_target(target)
    , m_usageFlags(usageFlags)
    , m_count(count)
    , m_stride(stride)
    , m_sizeInBytes(0)
    , m_lockStartIndex(InvalidIndex)
    , m_lookedCount(0)
{
    // �K�؂ȃ��\�[�X���
    m_nativeBufferOriginallyState = GetOriginallyResourceState(m_target, m_usageFlags);

    // �C���f�b�N�X�o�b�t�@�̏ꍇ�̓X�g���C�h�� 2 �܂��� 4 �łȂ���΂Ȃ�Ȃ�
    if (m_target == Target::Index)
    {
        if (m_stride != 2 && m_stride != 4)
        {
            // Invalid Argument Exception (�s���Ȉ���)
            assert(0);
        }
    }

    // �o�b�t�@�T�C�Y���v�Z���Ă����B
    // �o�b�t�@�T�C�Y = �v�f1���̃o�C�g�� �~ �v�f��
    m_sizeInBytes = m_stride * m_count;

    // �������A�萔�o�b�t�@�̏ꍇ��(256�̐����{)�o�C�g�łȂ���΂Ȃ�Ȃ��̂Ńo�b�t�@�T�C�Y�𒲐�����B
    if (m_target == Target::Constant)
    {
        m_sizeInBytes = (m_sizeInBytes + 255) & ~255;
    }

    // �A�b�v���[�h�o�b�t�@���쐬
    m_uploadBuffer.Attach(CreateUploadBuffer(m_sizeInBytes));

    // �K�v�ł���΃f�t�H���g�o�b�t�@���쐬
    if (m_usageFlags == UsageFlags::None)
    {
        m_nativeBuffer.Attach(CreateDefaultBuffer(m_sizeInBytes, m_nativeBufferOriginallyState));
    }
    else
    {
        m_nativeBuffer = m_uploadBuffer;
    }

    // �����f�[�^���w�肳��Ă���ꍇ�̓o�b�t�@�ɏ�������
    if (initialData)
    {
        UpdateSubresource(0, m_count, initialData);
    }
}


void* GraphicsBuffer::InternalLockBufferForWrite(int bufferStartIndex, int count)
{
    if (m_lockStartIndex >= 0)
    {
        assert(0);
    }

    // �����̑Ó����`�F�b�N
    assert(bufferStartIndex >= 0);
    assert(count > 0);
    assert(bufferStartIndex + count <= m_count);

    // �ǂݍ��ݎ��͈̔�
    D3D12_RANGE readRange;
    readRange.Begin = 0;
    readRange.End = 0;

    // ���\�[�X�����b�N���ăo�b�t�@�̐擪�A�h���X���擾
    char* lookedPointer;
    if (m_uploadBuffer->Map(0, &readRange, (void**)&lookedPointer) < 0)
    {
        assert(0);
    }

    // UnlockBufferAfterWrite()�֐��ׂ̈Ƀ��b�N�J�n�ʒu�ƃ��b�N�����v�f����ۑ����Ă���
    m_lockStartIndex = bufferStartIndex;
    m_lookedCount = count;

    // ���b�N�����̈�̐擪�A�h���X��Ԃ��B
    return &lookedPointer[m_stride * bufferStartIndex];
}

void GraphicsBuffer::UpdateSubresource(int bufferStartIndex, int count, const void* data)
{
    // �A�b�v���[�h�o�b�t�@�Ƀf�[�^����������
    void* looked = InternalLockBufferForWrite(0, m_count);
    memcpy(looked, data, m_stride * count);
    InternalUnlockBufferAfterWrite(count);

    if (m_usageFlags == UsageFlags::None)
    {
        ID3D12GraphicsCommandList* commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

        D3D12_RESOURCE_BARRIER barriers[1];
        memset(barriers, 0, sizeof(barriers));
        barriers[0].Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barriers[0].Transition.pResource = m_nativeBuffer.Get();
        barriers[0].Transition.Subresource = 0;
        barriers[0].Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barriers[0].Transition.StateBefore = m_nativeBufferOriginallyState;
        barriers[0].Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
        commandList->ResourceBarrier(_countof(barriers), barriers);
        commandList->CopyBufferRegion(m_nativeBuffer.Get(), 0, m_uploadBuffer.Get(), 0, m_stride * m_count);

        barriers[0].Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
        barriers[0].Transition.StateAfter = m_nativeBufferOriginallyState;
        commandList->ResourceBarrier(_countof(barriers), barriers);
    }
}

[[nodiscard]] void* GraphicsBuffer::LockBufferForWrite(int bufferStartIndex, int count)
{
    assert(m_usageFlags == UsageFlags::LockBufferForWrite);
    return InternalLockBufferForWrite(bufferStartIndex, count);
}


void GraphicsBuffer::InternalUnlockBufferAfterWrite(int countWritten)
{
    if (m_lockStartIndex < 0)
    {
        assert(0);
    }

    // �����̑Ó����`�F�b�N
    assert(countWritten >= 0);
    assert(m_lockStartIndex + countWritten <= m_count);

    // ���ۂɏ������񂾔͈́i�P�ʂ̓o�C�g�j
    // �f�o�C�X�h���C�o�͂��̏����g���ĕK�v�ŏ����͈̔͂̃f�[�^�̂݃R�s�[���s���܂��B
    D3D12_RANGE writtenRange;
    writtenRange.Begin = m_stride * m_lockStartIndex;
    writtenRange.End = writtenRange.Begin + m_stride * countWritten;

    // ���\�[�X���A�����b�N���ăr�f�I�������ɓ]���i�R�s�[�j���܂��B
    m_uploadBuffer->Unmap(0, &writtenRange);

    // �񃍃b�N��Ԃ̒l�ɖ߂�
    m_lockStartIndex = -1;
    m_lookedCount = 0;
}


void GraphicsBuffer::UnlockBufferAfterWrite(int countWritten)
{
    assert(m_usageFlags == UsageFlags::LockBufferForWrite);
    InternalUnlockBufferAfterWrite(countWritten);
}
