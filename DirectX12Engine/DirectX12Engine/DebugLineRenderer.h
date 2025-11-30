#pragma once

class DebugLineRenderer
{
private:
	struct Line
	{
		Vector3 start;
		Vector3 end;
		Color color;
	};

private:
    // 1フレームあたりの最大ライン数（頂点数 = ライン数 * 2）
    static constexpr UINT MAX_LINES = 10000;
    static constexpr UINT MAX_VERTICES = MAX_LINES * 2;

    struct DebugVertex
    {
        Vector3 position;
        Color color;
    };

    ComPtr<ID3D12RootSignature> m_rootSignature;
    ComPtr<ID3D12PipelineState> m_pipelineState;
    ComPtr<GraphicsBuffer> m_vertexBuffer;
    D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

    // CPUから書き込むためのポインタ
    DebugVertex* m_mappedVertices = nullptr;

private:
	std::vector<Line> m_lines;

public:
    void Initialize();
    void Shutdown();

public:
	void AddLine(const Vector3& start, const Vector3& end, const Color& color);
	void Render(const Matrix4x4& viewProjectionMatrix);
};