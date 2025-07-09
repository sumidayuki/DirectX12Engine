#include "TextureImporter.h"
#include "../External/DirectXTex/Include/DirectXTex.h"

// ���C�u�����������N����
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
        OutputDebugStringW((L"File not found: " + assetPath + L"\n").c_str());
        assert(0);
    }

    DirectX::TexMetadata texMetadata;
    DirectX::ScratchImage scratchImage;

    // WIC(.bmp .jpg .png)�摜�̓ǂݍ���
    if (FAILED(DirectX::LoadFromWICFile(assetPath.c_str(), DirectX::WIC_FLAGS_IGNORE_SRGB, &texMetadata, scratchImage)))
    {
        // TGA(.tga)�摜�̓ǂݍ���
        if (FAILED(DirectX::LoadFromTGAFile(assetPath.c_str(), DirectX::TGA_FLAGS_NONE, &texMetadata, scratchImage)))
        {
            // DDS(.dds)�摜�̓ǂݍ���
            if (FAILED(DirectX::LoadFromDDSFile(assetPath.c_str(), DirectX::DDS_FLAGS_NONE, &texMetadata, scratchImage)))
            {
                // HDR(.hdr)�摜�̓ǂݍ���
                if (FAILED(DirectX::LoadFromHDRFile(assetPath.c_str(), &texMetadata, scratchImage)))
                {
                    assert(0);
                }
            }
        }
    }


    // �t���[�����\�[�X�̎擾
    auto frameResource = Graphics::GetCurrentFrameResource();

    // �R�}���h���X�g�̎擾
    auto commandList = frameResource->GetCommandList();

    // D3D12�f�o�C�X�̎擾
    auto d3d12Device = Graphics::GetD3D12Device();

    // �l�C�e�B�u�e�N�X�`�����\�[�X
    ComPtr<ID3D12Resource> nativeTexture;
    ComPtr<ID3D12Resource> uploadBuffer;
    ComPtr<ID3D12DescriptorHeap> descriptorHeap;

    // �e�N�X�`�����\�[�X�̍쐬
    if (DirectX::CreateTextureEx(d3d12Device, texMetadata, D3D12_RESOURCE_FLAG_NONE, DirectX::CREATETEX_DEFAULT, nativeTexture.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // �e�T�u���\�[�X�̏����擾
    std::vector<D3D12_SUBRESOURCE_DATA> subresources;
    if (DirectX::PrepareUpload(d3d12Device, scratchImage.GetImages(), scratchImage.GetImageCount(), texMetadata, subresources) < 0)
    {
        assert(0);
    }

    // ���\�[�X(�e�N�X�`��)�̏ڍ׏����擾����
    const D3D12_RESOURCE_DESC resourceDesc = nativeTexture->GetDesc();

    // �A�b�v���[�h�o�b�t�@�ɕK�v�ȃo�C�g�������߂�
    static constexpr int MaxSubresources = 32;

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT layouts[MaxSubresources];
    UINT numRows[MaxSubresources];
    UINT64 rowSizeInBytes[MaxSubresources];
    UINT64 totalBytes;
    d3d12Device->GetCopyableFootprints(&resourceDesc, 0, (UINT)subresources.size(), 0, layouts, numRows, rowSizeInBytes, &totalBytes);

    // �A�b�v���[�h�o�b�t�@
    D3D12_HEAP_PROPERTIES prop;
    memset(&prop, 0, sizeof(prop));
    prop.Type = D3D12_HEAP_TYPE_UPLOAD;                         // CPU����A�N�Z�X�\�ȏ�Ԃō쐬
    prop.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;     // D3D12_HEAP_TYPE_CUSTOM�ȊO�ł͖��������
    prop.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;      // D3D12_HEAP_TYPE_CUSTOM�ȊO�ł͖��������
    prop.CreationNodeMask = 0;                                  // �P��GPU�̏ꍇ�� 0 �܂��� 1
    prop.VisibleNodeMask = 0;                                   // �P��GPU�̏ꍇ�� 0 �܂��� 1

    // �A�b�v���[�h�o�b�t�@(���\�[�X)�쐬���̏ڍ׏��
    D3D12_RESOURCE_DESC desc;
    memset(&desc, 0, sizeof(desc));                             // �[���N���A
    desc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;           // ����̓o�b�t�@���쐬������
    desc.Format = DXGI_FORMAT_UNKNOWN;                          // �o�b�t�@�̏ꍇ�͏��DXGI_FORMAT_UNKNOWN
    desc.Width = totalBytes;                                    // �o�b�t�@�S�̂̃o�C�g��
    desc.Height = 1;                                            // �o�b�t�@�̏ꍇ�͏��1
    desc.DepthOrArraySize = 1;                                  // �o�b�t�@�̏ꍇ�͏��1
    desc.MipLevels = 1;                                         // �o�b�t�@�̏ꍇ�͏��1
    desc.Alignment = 0;                                         // ���0�ł悢
    desc.SampleDesc.Count = 1;                                  // �}���`�T���v�����OOFF�Ȃ̂ŏ��1
    desc.SampleDesc.Quality = 0;                                // �}���`�T���v�����OOFF�Ȃ̂ŏ��0
    desc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;               // �e�N�X�`���ł͂Ȃ�
    desc.Flags = D3D12_RESOURCE_FLAG_NONE;                      // �e��t���O

    // �A�b�v���[�h�o�b�t�@���쐬
    if (d3d12Device->CreateCommittedResource(&prop, D3D12_HEAP_FLAG_NONE, &desc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_ID3D12Resource, (void**)uploadBuffer.ReleaseAndGetAddressOf()) < 0)
    {
        assert(0);
    }

    // �A�b�v���[�h�o�b�t�@�ւ̏������ݗp�|�C���^���擾����
    char* mapped;
    if (uploadBuffer->Map(0, nullptr, (void**)&mapped) < 0)
    {
        assert(0);
    }

    // �C���[�W���A�b�v���[�h�o�b�t�@�ɏ�������
    // ���ۂ�for���ȂǂŃT�u���\�[�X���ƂɃR�s�[���Ȃ���΂Ȃ�Ȃ��B
    char* copySource = (char*)subresources[0].pData;    // �R�s�[���A�h���X
    char* copyDest = mapped + layouts[0].Offset;        // �R�s�[��A�h���X
    for (UINT y = 0; y < numRows[0]; y++)
    {
        // �C���[�W1�s�����R�s�[
        memcpy(copyDest, copySource, layouts[0].Footprint.RowPitch);

        copySource += subresources[0].RowPitch;     // �R�s�[���A�h���X��1�s�����炷
        copyDest += layouts[0].Footprint.RowPitch;  // �R�s�[��A�h���X��1�s�����炷
    }

    uploadBuffer->Unmap(0, nullptr);

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // �u�A�b�v���[�h�o�b�t�@����e�N�X�`���փR�s�[����v�R�}���h���R�}���h���X�g�ɓ����B
    //----------------------------------------------------------------------------------------------------------------------------------------------------

    // �R�s�[��(�C���[�W)�Ɋւ�����
    D3D12_TEXTURE_COPY_LOCATION locationSource;
    memset(&locationSource, 0, sizeof(locationSource));
    locationSource.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    locationSource.pResource = uploadBuffer.Get();
    locationSource.PlacedFootprint = layouts[0];

    // �R�s�[��(�e�N�X�`��)�Ɋւ�����
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

    // �V�F�[�_�[���\�[�X�r���[(SRV)���i�[����ׂ̃f�B�X�N���v�^�q�[�v�̍쐬
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

    // �V�F�[�_�[���\�[�X�r���[(SRV)�̍쐬
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

    // �R�s�[�������ɃA�b�v���[�h�o�b�t�@���������
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
