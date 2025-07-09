#include "Material.h"


Material::Material()
    : m_mainTexture(nullptr)
    , m_color(Color::white)
    , m_descriptorHeap(nullptr)
{
}

void Material::SetColor(const Color color)
{
    m_color = color;
}

void Material::SetMainTexture(Texture2D* texture)
{
    m_mainTexture = texture;

    // テクスチャが設定された場合
    if (m_mainTexture)
    {
        // もし、このマテリアル用のディスクリプタヒープがまだ作られていなければ作成する
        if (!m_descriptorHeap)
        {
            m_descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
        }

        // テクスチャが持つSRV（シェーダーリソースビュー）を、このマテリアル用のヒープにコピーする
        // これにより、このマテリアルをシェーダーに渡す準備が整う
        D3D12_CPU_DESCRIPTOR_HANDLE copyTo = m_descriptorHeap->GetCPUDescriptorHandle(0);
        D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = m_mainTexture->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    else
    {
        // テクスチャがnullptrに設定された場合は、ヒープも解放する
        m_descriptorHeap = nullptr;
    }
}