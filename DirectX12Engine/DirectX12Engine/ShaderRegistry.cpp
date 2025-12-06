#include "Precompiled.h"
#include "ShaderRegistry.h"

void ShaderRegistry::StaticConstructor()
{
}

void ShaderRegistry::StaticDestructor()
{
	m_shaderCache.clear();
}

void ShaderRegistry::AllShadersCompile()
{
}

void ShaderRegistry::LoadShader(const std::wstring& path)
{
	auto json = Json::parse(path);
}

void ShaderRegistry::GetShader(const std::wstring& path)
{
}
