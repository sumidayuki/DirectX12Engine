#pragma once

/// <summary>
/// オブジェクトの表面の見た目を定義するアセット。
/// 色、テクスチャ、シェーダーのパラメータなどを管理します。
/// </summary>
class Material : public Reference
{
private:
    // シェーダー
    Shader* m_shader;

	std::vector<uint8_t> m_constantBufferData;                  // 定数バッファデータ
    std::unordered_map<uint64_t, ComPtr<Texture2D>> m_textures;   

public:
    Material();
    ~Material();

    // シェーダーの設定（ここでバッファをリサイズする）
    void SetShader(Shader* shader);
    Shader* GetShader() const { return m_shader; }

    void SetColor(uint64_t id, const Color& value);
    void SetColor(std::string_view name, const Color& value) { SetColor(Shader::PropertyToID(name), value); }

    void SetFloat(uint64_t id, float value);
    void SetFloat(std::string_view name, float value) { SetFloat(Shader::PropertyToID(name), value); }

    void SetVector(uint64_t id, const Vector4& value);
	void SetVector(std::string_view name, const Vector4& value) { SetVector(Shader::PropertyToID(name), value); }

    void SetMatrix(uint64_t id, const Matrix4x4& value);
	void SetMatrix(std::string_view name, const Matrix4x4& value) { SetMatrix(Shader::PropertyToID(name), value); }

    // テクスチャ設定
    void SetTexture(uint64_t id, Texture2D* texture);

    void SetTexture(std::string_view name, Texture2D* texture)
    {
        uint64_t id = Shader::PropertyToID(name);
        m_textures[id] = texture;

        std::string indexVarName = std::string(name) + "Index";
        WriteBindlessIndex(indexVarName, texture);
    }

    Texture2D* GetTexture(uint64_t id) const;

    void SetBaseColor(const Color& color) { SetColor("_BaseColor", color); }
    void SetMainTexture(Texture2D* tex) { SetTexture("_MainTex", tex); }

    const void* GetConstantBufferData() const { return m_constantBufferData.data(); }

    size_t GetConstantBufferSize() const { return m_constantBufferData.size(); }

    const std::unordered_map<uint64_t, ComPtr<Texture2D>>& GetTextures() const { return m_textures; }

private:
	void WriteBindlessIndex(const std::string& indexVarName, Texture2D* texture);
};