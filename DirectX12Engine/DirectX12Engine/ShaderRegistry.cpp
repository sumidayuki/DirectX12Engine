#include "Precompiled.h"
#include "ShaderRegistry.h"
#include "ShaderImporter.h"

void ShaderRegistry::StaticConstructor()
{
    ShaderRegistry::LoadShader(L"Assets/Shaders/Standard.shader"); // Standardシェーダーを明示的に読み込む
    ShaderRegistry::LoadShader(L"Assets/Shaders/PBR.shader");   // PBRシェーダーを明示的に読み込む
}

void ShaderRegistry::StaticDestructor()
{
    m_shaderCache.clear();
}

void ShaderRegistry::AllShadersCompile()
{
    ID3D12Device* d3d12Device = Graphics::GetD3D12Device();

    CD3DX12_DESCRIPTOR_RANGE ranges[2];
    ranges[0].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0, 0);  // t0 space0: Lights
    ranges[1].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, BindlessHeap::MAX_DESCRIPTORS, 0, 1);  // t0[] space1: Bindless

    CD3DX12_ROOT_PARAMETER rootParameters[6];
    rootParameters[0].InitAsConstantBufferView(0);             // b0: Camera
    rootParameters[1].InitAsConstantBufferView(1);             // b1: Object
    rootParameters[2].InitAsConstants(1, 2);                   // b2: LightConstants
    rootParameters[3].InitAsConstantBufferView(3);             // b3: Material
    rootParameters[4].InitAsDescriptorTable(1, &ranges[0]);    // t0 space0: Lights
    rootParameters[5].InitAsDescriptorTable(1, &ranges[1]);    // t0[] space1: Bindless Textures

    CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR);

    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc;
    rootSigDesc.Init(6, rootParameters, 1, &sampler, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
    if (FAILED(hr))
    {
        if (error) OutputDebugStringA((char*)error->GetBufferPointer());
    }

    hr = d3d12Device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature));

    assert(SUCCEEDED(hr));

    for (auto& shader : m_shaderCache)
    {
        shader.second->Create(m_rootSignature.Get());
    }
}

void ShaderRegistry::LoadShader(const std::wstring& path)
{
    ShaderImporter importer;
    Shader* impShader = importer.Import(path);
    m_shaderCache[UTF16LEtoUTF8::Convert(importer.GetFileNameWithoutExtension())] = impShader;
}
Shader* ShaderRegistry::GetShader(const std::string& path)
{
    auto it = m_shaderCache.find(path);
    if (m_shaderCache.end() == it)
    {
        assert(0);
        return nullptr;
    }
    return it->second;
}