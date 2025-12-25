#include "Material.h"
#include "Shader.h"
#include "ShaderRegistry.h"

Material::Material()
{
    SetShader(ShaderRegistry::GetShader("Standard"));
    m_constantBufferData.assign(m_shader->GetMaterialBufferSize(), 0);
}

Material::~Material() {}

void Material::SetShader(Shader* shader)
{
    m_shader = shader;
}

void Material::SetColor(uint64_t id, const Color& value)
{
    if (!m_shader || m_constantBufferData.empty()) return;

    // シェーダーから「このIDの変数が何バイト目にあるか」を取得
    const auto& table = m_shader->GetVariableTable();
    auto it = table.find(id);

    if (it != table.end())
    {
        const auto& info = it->second;
        // 指定されたオフセット位置に、Color(float4)をコピー
        assert(info.size == sizeof(Color));
        memcpy(m_constantBufferData.data() + info.offset, &value, sizeof(Color));
    }
}

void Material::SetFloat(uint64_t id, float value)
{
    if (!m_shader) return;

    const auto& table = m_shader->GetVariableTable();
    auto it = table.find(id);

    if (it != table.end())
    {
        const auto& info = it->second;
        assert(info.size >= sizeof(float));
        memcpy(m_constantBufferData.data() + info.offset, &value, sizeof(float));
    }
}

void Material::SetVector(uint64_t id, const Vector4& value)
{
    if (!m_shader) return;

	const auto& table = m_shader->GetVariableTable();
	auto it = table.find(id);

	if (it != table.end())
	{
		const auto& info = it->second;
		assert(info.size == sizeof(Vector4));
		memcpy(m_constantBufferData.data() + info.offset, &value, sizeof(Vector4));
	}
}

void Material::SetMatrix(uint64_t id, const Matrix4x4& value)
{
	if (!m_shader) return;

	const auto& table = m_shader->GetVariableTable();
	auto it = table.find(id);
	
    if (it != table.end())
	{
		const auto& info = it->second;
		assert(info.size == sizeof(Matrix4x4));
		memcpy(m_constantBufferData.data() + info.offset, &value, sizeof(Matrix4x4));
	}
}

void Material::SetTexture(uint64_t id, Texture2D* texture)
{
    m_textures[id] = texture;
}

Texture2D* Material::GetTexture(uint64_t id) const
{
    auto it = m_textures.find(id);
    if (it != m_textures.end())
    {
        return it->second.Get();
    }
    return nullptr;
}