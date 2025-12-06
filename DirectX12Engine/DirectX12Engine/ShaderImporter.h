#pragma once

class ShaderImporter : public AssetImporter
{
public:
	ShaderImporter();
	virtual ~ShaderImporter() override = default;

	Shader* Import(const std::wstring path);

	Shader* Import(const wchar_t* path);

	Shader* Import();

private:
	void ProcessPath(const Json& json, ShaderInfo& info);
	void ProcessInputLayout(const Json& json, ShaderInfo& info);
	void ProcessPrimitiveTopology(const Json& json, ShaderInfo& info);
	void ProcessRasterizer(const Json& json, ShaderInfo& info);
	void ProcessDepth(const Json& json, ShaderInfo& info);
	void ProcessBlend(const Json& json, ShaderInfo& info);
	void ProcessRTVFormat(const Json& json, ShaderInfo& info);
	void ProcessDSVFormat(const Json& json, ShaderInfo& info);
};