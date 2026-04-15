#pragma once

class UICanvasSystem : public System
{
private:
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<ID3D12RootSignature> m_rootSignature;

    ComPtr<GraphicsBuffer> m_constantBufferRing;
    ComPtr<GraphicsBuffer> m_quadVertexBuffer;
    ComPtr<GraphicsBuffer> m_quadIndexBuffer;
    BYTE* m_mappedConstants = nullptr;
    static constexpr UINT MAX_UI_PER_FRAME = 256;

    struct ConstantBufferLayout;

public:
    void Start(World& world) override;
    void Draw(World& world) override;
};