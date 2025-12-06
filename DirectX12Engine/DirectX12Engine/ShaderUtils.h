#pragma once

// 文字列からDXGI_FORMATを変換するマップ
static const std::unordered_map<std::string, DXGI_FORMAT> FormatMap =
{
    { "R32G32B32A32_FLOAT", DXGI_FORMAT_R32G32B32A32_FLOAT },
    { "R32G32B32A32_SINT",  DXGI_FORMAT_R32G32B32A32_SINT },
    { "R32G32B32_FLOAT",    DXGI_FORMAT_R32G32B32_FLOAT },
    { "R32G32_FLOAT",       DXGI_FORMAT_R32G32_FLOAT },
    { "R8G8B8A8_UNORM",     DXGI_FORMAT_R8G8B8A8_UNORM },
    { "D32_FLOAT",          DXGI_FORMAT_D32_FLOAT }
};

// 文字列からD3D12_FILL_MODEを変換するマップ
static const std::unordered_map<std::string, D3D12_FILL_MODE> FillModeMap =
{
    { "Solid",      D3D12_FILL_MODE_SOLID },
    { "Wireframe",  D3D12_FILL_MODE_WIREFRAME }
};

// 文字列からD3D12_CULL_MODEを変換するマップ
static const std::unordered_map<std::string, D3D12_CULL_MODE> CullModeMap =
{
    { "None", D3D12_CULL_MODE_NONE },
    { "Front", D3D12_CULL_MODE_FRONT },
    { "Back", D3D12_CULL_MODE_BACK }
};

// 文字列からD3D12_COMPARISON_FUNCを変換するマップ
static const std::unordered_map<std::string, D3D12_COMPARISON_FUNC> FuncMap =
{
    { "Less",       D3D12_COMPARISON_FUNC_LESS },
    { "LessEqual",  D3D12_COMPARISON_FUNC_LESS_EQUAL },
    { "Equal",      D3D12_COMPARISON_FUNC_EQUAL }
};

// 文字列からD3D12_BLENDを変換するマップ
static const std::unordered_map<std::string, D3D12_BLEND> BlendMap =
{
    { "ZERO",           D3D12_BLEND_ZERO },
    { "ONE",            D3D12_BLEND_ONE },
    { "SRC_ALPHA",      D3D12_BLEND_SRC_ALPHA },
    { "INV_SRC_ALPHA",  D3D12_BLEND_INV_SRC_ALPHA }
};

// 文字列からD3D12_BLEND_OPを変換するマップ
static const std::unordered_map<std::string, D3D12_BLEND_OP> BlendOpMap =
{
    { "ADD", D3D12_BLEND_OP_ADD }
};

// ユーティリティ関数: 文字列からDXGI_FORMATのバイトサイズを取得
inline size_t GetFormatByteSize(DXGI_FORMAT format)
{
    switch (format)
    {
    case DXGI_FORMAT_R32G32B32A32_FLOAT: return 16;
    case DXGI_FORMAT_R32G32B32A32_SINT: return 16;
    case DXGI_FORMAT_R32G32B32_FLOAT:    return 12;
    case DXGI_FORMAT_R32G32_FLOAT:       return 8;
    case DXGI_FORMAT_R8G8B8A8_UNORM:     return 4;
    case DXGI_FORMAT_D32_FLOAT:          return 4;
    default: assert(false); return 0;
    }
}