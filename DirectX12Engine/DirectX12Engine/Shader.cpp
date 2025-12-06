#include "Shader.h"
#include "ShaderUtils.h"

Shader::Shader(const ShaderInfo& info)
	: m_info(info)
{
}

bool Shader::Create(ID3D12RootSignature* rootSig)
{
    ID3D12Device* device = Graphics::GetD3D12Device();

    // HLSLコンパイル
    m_vertexShader.Attach(new ShaderBytecode(m_info.hlslPath.c_str(), m_info.vsEntry.c_str(), m_info.vsShaderModel.c_str()));
    m_pixelShader.Attach(new ShaderBytecode(m_info.hlslPath.c_str(), m_info.psEntry.c_str(), m_info.psShaderModel.c_str()));

    if (!m_vertexShader || !m_pixelShader) return false;

    // Input Layout の構築 (JSONの順序からオフセットを計算)
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements;
    std::vector<D3D12_INPUT_ELEMENT_DESC> inputElements2;
    UINT currentOffset = 0;

    for (const auto& input : m_info.inputLayout)
    {
        DXGI_FORMAT format = FormatMap.at(input.Format); // 文字列をDXGI_FORMATに変換

        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = input.Semantic.c_str();
        element.SemanticIndex = 0; // JSONに無いが、ここでは0と仮定
        element.Format = format;
        element.InputSlot = 0;
        element.AlignedByteOffset = currentOffset;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;

        inputElements.push_back(element);
        inputElements2.push_back(element);
        currentOffset += (UINT)GetFormatByteSize(format); // 次のオフセットを計算
    }

    {
        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = "BONEINDICES";
        element.SemanticIndex = 0; // JSONに無いが、ここでは0と仮定
        element.Format = DXGI_FORMAT_R32G32B32A32_SINT;
        element.InputSlot = 0;
        element.AlignedByteOffset = currentOffset;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;
        
        inputElements2.push_back(element);
        currentOffset += (UINT)GetFormatByteSize(DXGI_FORMAT_R32G32B32A32_SINT); // 次のオフセットを計算
    }
    {
        D3D12_INPUT_ELEMENT_DESC element = {};
        element.SemanticName = "BONEWEIGHTS";
        element.SemanticIndex = 0; // JSONに無いが、ここでは0と仮定
        element.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        element.InputSlot = 0;
        element.AlignedByteOffset = currentOffset;
        element.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
        element.InstanceDataStepRate = 0;

        inputElements2.push_back(element);
        currentOffset += (UINT)GetFormatByteSize(DXGI_FORMAT_R32G32B32A32_FLOAT); // 次のオフセットを計算
    }

    // PSOディスクリプタの構築
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc2 = {};
    psoDesc.pRootSignature = rootSig;
    psoDesc2.pRootSignature = rootSig;

    // シェーダーバイトコードを設定
    psoDesc.VS = { m_vertexShader->GetBytecodePointer(), m_vertexShader->GetBytecodeLength() };
    psoDesc2.VS = { m_vertexShader->GetBytecodePointer(), m_vertexShader->GetBytecodeLength() };
    psoDesc.PS = { m_pixelShader->GetBytecodePointer(), m_pixelShader->GetBytecodeLength() };
    psoDesc2.PS = { m_pixelShader->GetBytecodePointer(), m_pixelShader->GetBytecodeLength() };

    // 入力レイアウトを設定
    psoDesc.InputLayout = { inputElements.data(), (UINT)inputElements.size() };
    psoDesc2.InputLayout = { inputElements.data(), (UINT)inputElements.size() };

    // プリミティブトポロジーの設定
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // JSONの内容に応じて変換・設定
    psoDesc2.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // JSONの内容に応じて変換・設定

    // ラスタライザーステート (RasterizerInfoから変換)
    D3D12_RASTERIZER_DESC rsDesc;
    memset(&rsDesc, 0, sizeof(rsDesc));
    rsDesc.FillMode = FillModeMap.at(m_info.rasterizer.FillMode);
    rsDesc.CullMode = CullModeMap.at(m_info.rasterizer.CullMode);
    rsDesc.FrontCounterClockwise = m_info.rasterizer.FrontCCW;
    psoDesc.RasterizerState = rsDesc;
    psoDesc2.RasterizerState = rsDesc;

    // デプスステンシルステート (DepthInfoから変換)
    D3D12_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = m_info.depth.Enable;
    dsDesc.DepthWriteMask = (m_info.depth.WriteMask == "All") ? D3D12_DEPTH_WRITE_MASK_ALL : D3D12_DEPTH_WRITE_MASK_ZERO;
    dsDesc.DepthFunc = FuncMap.at(m_info.depth.Func);
    psoDesc.DepthStencilState = dsDesc;
    psoDesc2.DepthStencilState = dsDesc;

    // ブレンドステート (BlendInfoから変換)
    D3D12_BLEND_DESC blendDesc;
    blendDesc.RenderTarget[0].BlendEnable = m_info.blend.Enable;
    blendDesc.RenderTarget[0].BlendEnable = m_info.blend.Enable;
    if (m_info.blend.Enable)
    {
        blendDesc.RenderTarget[0].SrcBlend = BlendMap.at(m_info.blend.Src);
        blendDesc.RenderTarget[0].DestBlend = BlendMap.at(m_info.blend.Dst);
        blendDesc.RenderTarget[0].BlendOp = BlendOpMap.at(m_info.blend.Op);
        // RGBとAlphaのブレンド設定は同じと仮定
        blendDesc.RenderTarget[0].SrcBlendAlpha = blendDesc.RenderTarget[0].SrcBlend;
        blendDesc.RenderTarget[0].DestBlendAlpha = blendDesc.RenderTarget[0].DestBlend;
        blendDesc.RenderTarget[0].BlendOpAlpha = blendDesc.RenderTarget[0].BlendOp;
        blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
    }
    psoDesc.BlendState = blendDesc;
    psoDesc2.BlendState = blendDesc;

    // 出力フォーマット
    psoDesc.NumRenderTargets = 1;
    psoDesc2.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = FormatMap.at(m_info.rtvFormatString);
    psoDesc2.RTVFormats[0] = FormatMap.at(m_info.rtvFormatString);
    psoDesc.DSVFormat = FormatMap.at(m_info.dsvFormatString);
    psoDesc2.DSVFormat = FormatMap.at(m_info.dsvFormatString);
    psoDesc.SampleDesc.Count = 1;
    psoDesc2.SampleDesc.Count = 1;

    // PSOの生成
    HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(m_graphicsPipelineState.ReleaseAndGetAddressOf()));
    HRESULT hr2 = device->CreateGraphicsPipelineState(&psoDesc2, IID_PPV_ARGS(m_skinningGPSO.ReleaseAndGetAddressOf()));

    if (FAILED(hr))
    {
        assert(false);
        return false;
    }

    if (FAILED(hr2))
    {
        assert(false);
        return false;
    }

    return true;
}
