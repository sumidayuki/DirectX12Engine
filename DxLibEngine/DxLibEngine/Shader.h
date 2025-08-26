#pragma once

class Shader : public Reference
{
private:
	ComPtr<ID3D12RootSignature> m_rootSignature;
	ComPtr<ID3D12PipelineState> m_pipelineState;

public:
	Shader(const std::wstring& vsPath, const std::string& vsEntry, const std::string& vsTarget, const std::wstring& psPath, const std::string& psEntry, const std::string& psTarget);

	ComPtr<ID3D12RootSignature> GetRootSignature() const { return m_rootSignature; }
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

private:
	void CreateRootSignature();
	void CreatePipelineState(const D3D12_INPUT_ELEMENT_DESC* inputElements, UINT numInputElements, const D3D12_GRAPHICS_PIPELINE_STATE_DESC& basePsoDesc, ShaderBytecode* vertexShader, ShaderBytecode* pixelShader);
};