#include "GraphicsBuffer.h"

ID3D12Resource* GraphicsBuffer::CreateDefaultBuffer(int size, D3D12_RESOURCE_STATES initialState)
{
    // ヒーププロパティ情報
    D3D12_HEAP_PROPERTIES heapProps;
    memset(&heapProps, 0, sizeof(heapProps));
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 0;
    heapProps.VisibleNodeMask = 0;

    // リソース作成時の詳細情報
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

    // Direct3D12のバッファリソースを作成
    ID3D12Resource* nativeBuffer;
    if (Graphics::GetD3D12Device()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_CREATE_NOT_ZEROED, &resDesc, initialState, nullptr, IID_ID3D12Resource, (void**)&nativeBuffer) < 0)
    {
        assert(0);
    }

    return nativeBuffer;
}

ID3D12Resource* GraphicsBuffer::CreateUploadBuffer(int size)
{
    // ヒーププロパティ情報
    D3D12_HEAP_PROPERTIES heapProps;
    memset(&heapProps, 0, sizeof(heapProps));
    heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
    heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    heapProps.CreationNodeMask = 0;
    heapProps.VisibleNodeMask = 0;

    // リソース作成時の詳細情報
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

    // バッファリソースの作成
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
    // 適切なリソース状態
    m_nativeBufferOriginallyState = GetOriginallyResourceState(m_target, m_usageFlags);

    // インデックスバッファの場合はストライドが 2 または 4 でなければならない
    if (m_target == Target::Index)
    {
        if (m_stride != 2 && m_stride != 4)
        {
            // Invalid Argument Exception (不正な引数)
            assert(0);
        }
    }

    // バッファサイズを計算しておく。
    // バッファサイズ = 要素1個分のバイト数 × 要素数
    m_sizeInBytes = m_stride * m_count;

    // ただし、定数バッファの場合は(256の整数倍)バイトでなければならないのでバッファサイズを調整する。
    if (m_target == Target::Constant)
    {
        m_sizeInBytes = (m_sizeInBytes + 255) & ~255;
    }

    // アップロードバッファを作成
    m_uploadBuffer.Attach(CreateUploadBuffer(m_sizeInBytes));

    // 必要であればデフォルトバッファを作成
    if (m_usageFlags == UsageFlags::None)
    {
        m_nativeBuffer.Attach(CreateDefaultBuffer(m_sizeInBytes, m_nativeBufferOriginallyState));
    }
    else
    {
        m_nativeBuffer = m_uploadBuffer;
    }

    // 初期データが指定されている場合はバッファに書き込む
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

    // 引数の妥当性チェック
    assert(bufferStartIndex >= 0);
    assert(count > 0);
    assert(bufferStartIndex + count <= m_count);

    // 読み込み時の範囲
    D3D12_RANGE readRange;
    readRange.Begin = 0;
    readRange.End = 0;

    // リソースをロックしてバッファの先頭アドレスを取得
    char* lookedPointer;
    if (m_uploadBuffer->Map(0, &readRange, (void**)&lookedPointer) < 0)
    {
        assert(0);
    }

    // UnlockBufferAfterWrite()関数の為にロック開始位置とロックした要素数を保存しておく
    m_lockStartIndex = bufferStartIndex;
    m_lookedCount = count;

    // ロックした領域の先頭アドレスを返す。
    return &lookedPointer[m_stride * bufferStartIndex];
}

void GraphicsBuffer::UpdateSubresource(int bufferStartIndex, int count, const void* data)
{
    // アップロードバッファにデータを書き込む
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

    // 引数の妥当性チェック
    assert(countWritten >= 0);
    assert(m_lockStartIndex + countWritten <= m_count);

    // 実際に書き込んだ範囲（単位はバイト）
    // デバイスドライバはこの情報を使って必要最小限の範囲のデータのみコピーを行います。
    D3D12_RANGE writtenRange;
    writtenRange.Begin = m_stride * m_lockStartIndex;
    writtenRange.End = writtenRange.Begin + m_stride * countWritten;

    // リソースをアンロックしてビデオメモリに転送（コピー）します。
    m_uploadBuffer->Unmap(0, &writtenRange);

    // 非ロック状態の値に戻す
    m_lockStartIndex = -1;
    m_lookedCount = 0;
}


void GraphicsBuffer::UnlockBufferAfterWrite(int countWritten)
{
    assert(m_usageFlags == UsageFlags::LockBufferForWrite);
    InternalUnlockBufferAfterWrite(countWritten);
}
