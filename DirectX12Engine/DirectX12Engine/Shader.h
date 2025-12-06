#pragma once

// 入力レイアウトのJSON要素に対応する構造体
struct InputElement
{
    std::string Semantic;
    std::string Format; // 文字列のまま保持し、パース時に変換
};

struct RasterizerInfo
{
    std::string FillMode;
    std::string CullMode;
    bool FrontCCW;
};

struct DepthInfo
{
    bool Enable;
    std::string WriteMask; // "All" -> D3D12_DEPTH_WRITE_MASK_ALL
    std::string Func;
};

struct BlendInfo
{
    bool Enable;
    std::string Src;
    std::string Dst;
    std::string Op;
};

/// <summary>
/// .shaderファイルから読み込んだシェーダー設定を保持する構造体
/// </summary>
struct ShaderInfo
{
    // HLSLファイル情報
    std::wstring hlslPath;
    std::string vsEntry;
    std::string vsShaderModel;
    std::string psEntry;
    std::string psShaderModel;

    // PSO固定機能情報
    std::vector<InputElement> inputLayout; // JSONをそのまま保持
    std::string primitiveTopology;
    RasterizerInfo rasterizer;
    DepthInfo depth;
    BlendInfo blend;

    std::string rtvFormatString; // 文字列のまま保持
    std::string dsvFormatString; // 文字列のまま保持
};

class Shader : public Reference
{
private:
    ShaderInfo m_info;
    ComPtr<ID3D12PipelineState> m_graphicsPipelineState;
    ComPtr<ID3D12PipelineState> m_skinningGPSO;

    // コンパイル済みバイトコード
    ComPtr<ShaderBytecode> m_vertexShader;
    ComPtr<ShaderBytecode> m_pixelShader;

public:
    Shader(const ShaderInfo& info);

    /// <summary>
    /// ShaderInfoに基づき、HLSLコンパイルと単一PSO生成を実行します。
    /// </summary>
    bool Create(ID3D12RootSignature* rootSig);

    ID3D12PipelineState* GetPSO(bool isSkinning = false) const { return isSkinning ? m_skinningGPSO.Get() : m_graphicsPipelineState.Get(); }
};