#include "TextureImporter.h"
#include "../External/DirectXTex/Include/DirectXTex.h"

// ライブラリをリンクする
#if defined(_DEBUG)
#pragma comment(lib, "../External/DirectXTex/Lib/x64-Debug/DirectXTex.lib")
#else
#pragma comment(lib, "../External/DirectXTex/Lib/x64-Release/DirectXTex.lib")
#endif

TextureImporter::TextureImporter()
    : m_textureType(TextureImporterType::Default)
    , m_npotScale(TextureImporterNPOTScale::None)
    , m_maxTextureSize(2048)
    , m_alphaIsTransparency(true)
    , m_isReadable(false)
    , m_wrapModeU(TextureWrapMode::Clamp)
    , m_wrapModeV(TextureWrapMode::Clamp)
    , m_wrapModeW(TextureWrapMode::Clamp)
    , m_filterMode(FilterMode::Bilinear)
    , m_anisoLevel(1)
    , m_mipmapEnabled(true)
    , m_mipmapFadeDistanceStart(0)
    , m_mipmapFadeDistanceEnd(0)
    , m_mipMapBias(0.0f)
    , m_mipmapFilter(TextureImporterMipFilter::BoxFilter)
    , m_spriteImportMode(SpriteImportMode::None)
    , m_spriteMeshType(SpriteMeshType::Tight)
    , m_spriteBorder(Vector4::zero)
    , m_spritePivot(Vector2::zero)
    , m_spritePixelsPerUnit(100.0f)
{

}


void TextureImporter::SetWrapMode(TextureWrapMode wrapMode)
{
    m_wrapModeU = wrapMode;
    m_wrapModeV = wrapMode;
    m_wrapModeW = wrapMode;
}


Texture2D* TextureImporter::Import()
{
    const std::wstring& assetPath = GetAssetPath();

    if (GetFileAttributesW(assetPath.c_str()) == INVALID_FILE_ATTRIBUTES)
    {
        OutputDebugStringW((L"TextureImporter Error: File not found: " + assetPath + L"\n").c_str());
        return nullptr; // assert(0) から変更
    }

    DirectX::TexMetadata texMetadata;
    DirectX::ScratchImage scratchImage;

    // WIC(.bmp .jpg .png)画像の読み込み
    if (FAILED(DirectX::LoadFromWICFile(assetPath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, &texMetadata, scratchImage)))
    {
        // TGA(.tga)画像の読み込み
        if (FAILED(DirectX::LoadFromTGAFile(assetPath.c_str(), DirectX::TGA_FLAGS_NONE, &texMetadata, scratchImage)))
        {
            // DDS(.dds)画像の読み込み
            if (FAILED(DirectX::LoadFromDDSFile(assetPath.c_str(), DirectX::DDS_FLAGS_NONE, &texMetadata, scratchImage)))
            {
                // HDR(.hdr)画像の読み込み
                if (FAILED(DirectX::LoadFromHDRFile(assetPath.c_str(), &texMetadata, scratchImage)))
                {
                    assert(0);
                }
            }
        }
    }


    // フレームリソースの取得
    auto frameResource = Graphics::GetCurrentFrameResource();

    // コマンドリストの取得
    auto commandList = frameResource->GetCommandList();

    // D3D12デバイスの取得
    auto d3d12Device = Graphics::GetD3D12Device();

    // ネイティブテクスチャリソース
    ComPtr<ID3D12Resource> nativeTexture;
    ComPtr<ID3D12Resource> uploadBuffer;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // テクスチャリソースの作成
    if (DirectX::CreateTextureEx(d3d12Device, texMetadata, D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, nativeTexture.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // 各サブリソースの情報を取得
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    if (DirectX::PrepareUpload(d3d12Device, scratchImage.GetImages(), scratchImage.GetImageCount(), texMetadata, subresources) < 0)
    {
        assert(0);
    }

    // リソース(テクスチャ)の詳細情報を取得する
    const D3D12_RESOURCE_DESC resourceDesc = nativeTexture->GetDesc();

    // アップロードバッファに必要なバイト数を求める
    static constexpr int MaxSubresources = 32;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[MaxSubresources];
    UINT numRows[MaxSubresources];
    UINT64 rowSizeInBytes[MaxSubresources];
    UINT64 totalBytes;
    d3d12Device->GetCopyableFootprints(&resourceDesc, 0, (UINT)subresources.size(), 0, layouts, numRows, rowSizeInBytes, &totalBytes);

    // アップロードバッファ
    D3D12_HEAP_PROPERTIES prop;
    memset(&prop, 0, sizeof(prop));
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;                         // CPUからアクセス可能な状態で作成
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;     // D3D12_HEAP_TYPE_CUSTOM以外では無視される
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;      // D3D12_HEAP_TYPE_CUSTOM以外では無視される
    prop.CreationNodeMask = 0;                                  // 単一GPUの場合は 0 または 1
    prop.VisibleNodeMask = 0;                                   // 単一GPUの場合は 0 または 1

    // アップロードバッファ(リソース)作成時の詳細情報
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));                             // ゼロクリア
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;           // 今回はバッファを作成したい
    desc.Format = DXGI_FORMAT_UNKNOWN;                          // バッファの場合は常にDXGI_FORMAT_UNKNOWN
    desc.Width = totalBytes;                                    // バッファ全体のバイト数
    desc.Height = 1;                                            // バッファの場合は常に1
    desc.DepthOrArraySize = 1;                                  // バッファの場合は常に1
    desc.MipLevels = 1;                                         // バッファの場合は常に1
    desc.Alignment = 0;                                         // 常に0でよい
    desc.SampleDesc.Count = 1;                                  // マルチサンプリングOFFなので常に1
    desc.SampleDesc.Quality = 0;                                // マルチサンプリングOFFなので常に0
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;               // テクスチャではない
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;                      // 各種フラグ

    // アップロードバッファを作成
    if (d3d12Device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_ID3D12Resource, (void**)uploadBuffer.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // アップロードバッファへの書き込み用ポインタを取得する
    char* mapped;
    if (uploadBuffer->Map(0, nullptr, (void**)&mapped) < 0)
    {
        assert(0);
    }

    // イメージをアップロードバッファに書き込む
    // 実際はfor文などでサブリソースごとにコピーしなければならない。
    char* copySource = (char*)subresources[0].pData;    // コピー元アドレス
    char* copyDest = mapped + layouts[0].Offset;        // コピー先アドレス
    for (UINT y = 0; y < numRows[0]; y++)
    {
        // イメージ1行分をコピー
        memcpy(copyDest, copySource, layouts[0].Footprint.RowPitch);

        copySource += subresources[0].RowPitch;     // コピー元アドレスを1行分ずらす
        copyDest += layouts[0].Footprint.RowPitch;  // コピー先アドレスを1行分ずらす
    }

    uploadBuffer->Unmap(0, nullptr);

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // 「アップロードバッファからテクスチャへコピーする」コマンドをコマンドリストに入れる。
    //----------------------------------------------------------------------------------------------------------------------------------------------------

    // コピー元(イメージ)に関する情報
    D3D12_TEXTURE_COPY_LOCATION locationSource;
    memset(&locationSource, 0, sizeof(locationSource));
    locationSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    locationSource.pResource = uploadBuffer.Get();
    locationSource.PlacedFootprint = layouts[0];

    // コピー先(テクスチャ)に関する情報
    D3D12_TEXTURE_COPY_LOCATION locationDest;
    memset(&locationDest, 0, sizeof(locationDest));
    locationDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    locationDest.pResource = nativeTexture.Get();
    locationDest.SubresourceIndex = 0;
    commandList->CopyTextureRegion(&locationDest, 0, 0, 0, &locationSource, nullptr);

    D3D12_RESOURCE_BARRIER barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = nativeTexture.Get();
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    commandList->ResourceBarrier(1, &barrier);

    // シェーダーリソースビュー(SRV)を格納する為のディスクリプタヒープの作成
    D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
    memset(&dhDesc, 0, sizeof(dhDesc));
    dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    dhDesc.NumDescriptors = 1;
    dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dhDesc.NodeMask = 0;
    if (d3d12Device->CreateDescriptorHeap(&dhDesc, IID_ID3D12DescriptorHeap, (void**)descriptorHeap.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // シェーダーリソースビュー(SRV)の作成
    d3d12Device->CreateShaderResourceView(nativeTexture.Get(), nullptr, descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    Texture2D* texture2D = new Texture2D();
    texture2D->m_dimension = TextureDimension::Tex2D;
    texture2D->m_width = (int)resourceDesc.Width;
    texture2D->m_height = (int)resourceDesc.Height;
    texture2D->m_anisoLevel = 0;
    texture2D->m_filterMode = FilterMode::Point;
    texture2D->m_wrapModeU = TextureWrapMode::Clamp;
    texture2D->m_wrapModeV = TextureWrapMode::Clamp;
    texture2D->m_wrapModeW = TextureWrapMode::Clamp;
    texture2D->m_mipMapBias = 0.0f;
    texture2D->m_nativeTexture = nativeTexture;
    texture2D->m_uploadBuffer = uploadBuffer;
    texture2D->m_descriptorHeap = descriptorHeap;
    texture2D->m_isReadable = false;

    // コピー完了時にアップロードバッファを解放する
    frameResource->AddEventAfterGPUCompletion
    (
        [texture2D]()->void
        {
            texture2D->m_uploadBuffer = nullptr;
        }
    );

    return texture2D;
}


Texture2D* TextureImporter::Import(const wchar_t* path)
{
    SetAssetPath(path);
    return Import();
}

Texture2D* TextureImporter::Import(const void* data, size_t size)
{
    // メモリ上のデータから画像を読み込む
    DirectX::TexMetadata texMetadata;
    DirectX::ScratchImage scratchImage;

    HRESULT hr = DirectX::LoadFromWICMemory(reinterpret_cast<const uint8_t*>(data), size, DirectX::WIC_FLAGS_NONE, &texMetadata, scratchImage);

    // 読み込みに失敗した場合は安全にnullptrを返す
    if (FAILED(hr))
    {
        OutputDebugStringA("TextureImporter Error: Failed to load image from memory.\n");
        return nullptr;
    }

    // フレームリソースの取得
    auto frameResource = Graphics::GetCurrentFrameResource();

    // コマンドリストの取得
    auto commandList = frameResource->GetCommandList();

    // D3D12デバイスの取得
    auto d3d12Device = Graphics::GetD3D12Device();

    // ネイティブテクスチャリソース
    ComPtr<ID3D12Resource> nativeTexture;
    ComPtr<ID3D12Resource> uploadBuffer;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // テクスチャリソースの作成
    if (DirectX::CreateTextureEx(d3d12Device, texMetadata, D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, nativeTexture.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // 各サブリソースの情報を取得
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    if (DirectX::PrepareUpload(d3d12Device, scratchImage.GetImages(), scratchImage.GetImageCount(), texMetadata, subresources) < 0)
    {
        assert(0);
    }

    // リソース(テクスチャ)の詳細情報を取得する
    const D3D12_RESOURCE_DESC resourceDesc = nativeTexture->GetDesc();

    // アップロードバッファに必要なバイト数を求める
    static constexpr int MaxSubresources = 32;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[MaxSubresources];
    UINT numRows[MaxSubresources];
    UINT64 rowSizeInBytes[MaxSubresources];
    UINT64 totalBytes;
    d3d12Device->GetCopyableFootprints(&resourceDesc, 0, (UINT)subresources.size(), 0, layouts, numRows, rowSizeInBytes, &totalBytes);

    // アップロードバッファ
    D3D12_HEAP_PROPERTIES prop;
    memset(&prop, 0, sizeof(prop));
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;                         // CPUからアクセス可能な状態で作成
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;     // D3D12_HEAP_TYPE_CUSTOM以外では無視される
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;      // D3D12_HEAP_TYPE_CUSTOM以外では無視される
    prop.CreationNodeMask = 0;                                  // 単一GPUの場合は 0 または 1
    prop.VisibleNodeMask = 0;                                   // 単一GPUの場合は 0 または 1

    // アップロードバッファ(リソース)作成時の詳細情報
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));                             // ゼロクリア
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;           // 今回はバッファを作成したい
    desc.Format = DXGI_FORMAT_UNKNOWN;                          // バッファの場合は常にDXGI_FORMAT_UNKNOWN
    desc.Width = totalBytes;                                    // バッファ全体のバイト数
    desc.Height = 1;                                            // バッファの場合は常に1
    desc.DepthOrArraySize = 1;                                  // バッファの場合は常に1
    desc.MipLevels = 1;                                         // バッファの場合は常に1
    desc.Alignment = 0;                                         // 常に0でよい
    desc.SampleDesc.Count = 1;                                  // マルチサンプリングOFFなので常に1
    desc.SampleDesc.Quality = 0;                                // マルチサンプリングOFFなので常に0
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;               // テクスチャではない
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;                      // 各種フラグ

    // アップロードバッファを作成
    if (d3d12Device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_ID3D12Resource, (void**)uploadBuffer.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // アップロードバッファへの書き込み用ポインタを取得する
    char* mapped;
    if (uploadBuffer->Map(0, nullptr, (void**)&mapped) < 0)
    {
        assert(0);
    }

    // イメージをアップロードバッファに書き込む
    // 実際はfor文などでサブリソースごとにコピーしなければならない。
    char* copySource = (char*)subresources[0].pData;    // コピー元アドレス
    char* copyDest = mapped + layouts[0].Offset;        // コピー先アドレス
    for (UINT y = 0; y < numRows[0]; y++)
    {
        // イメージ1行分をコピー
        memcpy(copyDest, copySource, layouts[0].Footprint.RowPitch);

        copySource += subresources[0].RowPitch;     // コピー元アドレスを1行分ずらす
        copyDest += layouts[0].Footprint.RowPitch;  // コピー先アドレスを1行分ずらす
    }

    uploadBuffer->Unmap(0, nullptr);

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // 「アップロードバッファからテクスチャへコピーする」コマンドをコマンドリストに入れる。
    //----------------------------------------------------------------------------------------------------------------------------------------------------

    // コピー元(イメージ)に関する情報
    D3D12_TEXTURE_COPY_LOCATION locationSource;
    memset(&locationSource, 0, sizeof(locationSource));
    locationSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    locationSource.pResource = uploadBuffer.Get();
    locationSource.PlacedFootprint = layouts[0];

    // コピー先(テクスチャ)に関する情報
    D3D12_TEXTURE_COPY_LOCATION locationDest;
    memset(&locationDest, 0, sizeof(locationDest));
    locationDest.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    locationDest.pResource = nativeTexture.Get();
    locationDest.SubresourceIndex = 0;
    commandList->CopyTextureRegion(&locationDest, 0, 0, 0, &locationSource, nullptr);

    D3D12_RESOURCE_BARRIER barrier;
    memset(&barrier, 0, sizeof(barrier));
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = nativeTexture.Get();
    barrier.Transition.Subresource = 0;
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    commandList->ResourceBarrier(1, &barrier);

    // シェーダーリソースビュー(SRV)を格納する為のディスクリプタヒープの作成
    D3D12_DESCRIPTOR_HEAP_DESC dhDesc;
    memset(&dhDesc, 0, sizeof(dhDesc));
    dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    dhDesc.NumDescriptors = 1;
    dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dhDesc.NodeMask = 0;
    if (d3d12Device->CreateDescriptorHeap(&dhDesc, IID_ID3D12DescriptorHeap, (void**)descriptorHeap.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // シェーダーリソースビュー(SRV)の作成
    d3d12Device->CreateShaderResourceView(nativeTexture.Get(), nullptr, descriptorHeap->GetCPUDescriptorHandleForHeapStart());

    Texture2D* texture2D = new Texture2D();
    texture2D->m_dimension = TextureDimension::Tex2D;
    texture2D->m_width = (int)resourceDesc.Width;
    texture2D->m_height = (int)resourceDesc.Height;
    texture2D->m_anisoLevel = 0;
    texture2D->m_filterMode = FilterMode::Point;
    texture2D->m_wrapModeU = TextureWrapMode::Clamp;
    texture2D->m_wrapModeV = TextureWrapMode::Clamp;
    texture2D->m_wrapModeW = TextureWrapMode::Clamp;
    texture2D->m_mipMapBias = 0.0f;
    texture2D->m_nativeTexture = nativeTexture;
    texture2D->m_uploadBuffer = uploadBuffer;
    texture2D->m_descriptorHeap = descriptorHeap;
    texture2D->m_isReadable = false;

    // コピー完了時にアップロードバッファを解放する
    frameResource->AddEventAfterGPUCompletion
    (
        [texture2D]()->void
        {
            texture2D->m_uploadBuffer = nullptr;
        }
    );

    return texture2D;
}

Texture2D* TextureImporter::Import(int width, int height, DXGI_FORMAT format, const void* initialData, size_t rowPitch)
{
    // この関数の実装は、以前 Texture2D::CreateFromMemory に実装しようとしたものと全く同じです。
    // TextureImporterが、生のピクセルデータからテクスチャを作成する責務を持ちます。

    if (width == 0 || height == 0) {
        assert(0 && "Texture dimensions cannot be zero.");
        return nullptr;
    }

    auto d3d12Device = Graphics::GetD3D12Device();
    auto commandList = Graphics::GetCurrentFrameResource()->GetCommandList();

    // 1. 最終的なテクスチャリソースを作成
    D3D12_RESOURCE_DESC texDesc = {};
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Width = width;
    texDesc.Height = height;
    texDesc.DepthOrArraySize = 1;
    texDesc.MipLevels = 1;
    texDesc.Format = format;
    texDesc.SampleDesc.Count = 1;

    D3D12_HEAP_PROPERTIES defaultHeapProps = {};
    defaultHeapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    ComPtr<ID3D12Resource> nativeTexture;
    if (FAILED(d3d12Device->CreateCommittedResource(
        &defaultHeapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
        D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&nativeTexture))))
    {
        assert(0); return nullptr;
    }

    // 2. アップロード用バッファの作成とデータコピー
    UINT64 uploadBufferSize;
    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
    d3d12Device->GetCopyableFootprints(&texDesc, 0, 1, 0, &layout, nullptr, nullptr, &uploadBufferSize);

    D3D12_HEAP_PROPERTIES uploadHeapProps = {};
    uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;

    D3D12_RESOURCE_DESC bufferDesc = {};
    bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    bufferDesc.Width = uploadBufferSize;
    bufferDesc.Height = 1;
    bufferDesc.DepthOrArraySize = 1;
    bufferDesc.MipLevels = 1;
    bufferDesc.SampleDesc.Count = 1;
    bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

    ComPtr<ID3D12Resource> uploadBuffer;
    if (FAILED(d3d12Device->CreateCommittedResource(
        &uploadHeapProps, D3D12_HEAP_FLAG_NONE, &bufferDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&uploadBuffer))))
    {
        assert(0); return nullptr;
    }

    void* mappedPtr = nullptr;
    uploadBuffer->Map(0, nullptr, &mappedPtr);
    for (int y = 0; y < height; ++y) {
        memcpy(
            static_cast<uint8_t*>(mappedPtr) + layout.Offset + y * layout.Footprint.RowPitch,
            static_cast<const uint8_t*>(initialData) + y * rowPitch,
            rowPitch
        );
    }
    uploadBuffer->Unmap(0, nullptr);

    D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
    srcLocation.pResource = uploadBuffer.Get();
    srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    srcLocation.PlacedFootprint = layout;

    D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
    dstLocation.pResource = nativeTexture.Get();
    dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;

    commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);

    // 3. リソースバリアとSRVの作成、オブジェクトを返す
    D3D12_RESOURCE_BARRIER barrier = {};
    barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    barrier.Transition.pResource = nativeTexture.Get();
    barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
    barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
    barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &barrier);

    Texture2D* texture2D = new Texture2D();
    // (texture2Dのメンバー設定処理)

    Graphics::GetCurrentFrameResource()->AddEventAfterGPUCompletion([texture2D, uploadBuffer_ = uploadBuffer]() mutable {
        // 解放予約
        });

    return texture2D;
}