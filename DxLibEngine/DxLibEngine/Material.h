#pragma once

/// <summary>
/// �I�u�W�F�N�g�̕\�ʂ̌����ڂ��`����A�Z�b�g�B
/// �F�A�e�N�X�`���A�V�F�[�_�[�̃p�����[�^�Ȃǂ��Ǘ����܂��B
/// </summary>
class Material : public Reference
{
public:
    // �V�F�[�_�[���̃e�N�X�`���X���b�g�ɑΉ������邽�߂̃C���f�b�N�X
    enum class TextureSlot
    {
        Diffuse = 0,  // ��{�F�e�N�X�`�� (t0)
        Normal,       // �@���}�b�v (t1)
        Specular,     // ���ʔ��˃}�b�v (t2)
        // �K�v�ɉ����đ��̃}�b�v���ǉ�
        Max
    };

private:
    // --- Material Properties ---
    Color m_diffuseColor;   // �g�U���ːF
    Color m_specularColor;  // ���ʔ��ːF

    // --- Textures ---
    ComPtr<Texture2D> m_textures[(int)TextureSlot::Max];

    // --- DirectX 12 Resources ---
    ComPtr<DescriptorHeap> m_descriptorHeap;

public:
    Material();
    ~Material() = default;

    // --- Colors ---
    void SetDiffuseColor(const Color& color) { m_diffuseColor = color; }
    const Color& GetDiffuseColor() const { return m_diffuseColor; }

    void SetSpecularColor(const Color& color) { m_specularColor = color; }
    const Color& GetSpecularColor() const { return m_specularColor; }

    // --- Textures ---
    // C++�̎������ŁA�ݒ肳�ꂽ�S�e�N�X�`�������Ƀf�X�N���v�^�q�[�v���č\�z����K�v������
    void SetTexture(TextureSlot slot, Texture2D* texture);
    Texture2D* GetTexture(TextureSlot slot) const { return m_textures[(int)slot].Get(); }

    // --- DirectX 12 Getters ---
    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_descriptorHeap ? m_descriptorHeap->GetNativeHeapPointer() : nullptr; }

    // ����X���b�g�̃f�X�N���v�^�n���h�����擾
    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(TextureSlot slot) const
    {
        return m_descriptorHeap ? m_descriptorHeap->GetGPUDescriptorHandle((int)slot) : D3D12_GPU_DESCRIPTOR_HANDLE{ 0 };
    }
};