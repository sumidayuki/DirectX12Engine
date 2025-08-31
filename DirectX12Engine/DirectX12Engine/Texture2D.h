#pragma once
#include "Texture.h"

// 2次元テクスチャ
class Texture2D : public Texture
{
    friend class TextureImporter;

private:
    ComPtr<ID3D12Resource>          m_nativeTexture;      // Direct3D12リソース
    ComPtr<ID3D12Resource>          m_uploadBuffer;       // アップロードバッファ
    ComPtr<ID3D12DescriptorHeap>    m_descriptorHeap;     // ディスクリプタヒープ

public:
    // デフォルトコンストラクタ
    Texture2D();

    // 仮想デストラクタ
    virtual ~Texture2D();

    void CreateFromMemory(
        int width,
        int height,
        DXGI_FORMAT format,
        const void* initialData,
        size_t rowPitch);


    // ディスクリプタヒープを取得します。
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap.Get(); }

    // Direct3D12リソースを取得します。
    void* GetNativeTexturePtr() const override { return m_nativeTexture.Get(); }

    ID3D12Resource* GetNativeResource() const { return m_nativeTexture.Get(); }
};
