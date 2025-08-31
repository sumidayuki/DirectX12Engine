#include "Material.h"
#include "Graphics.h" // GetD3D12Device() �̂��߂ɃC���N���[�h

Material::Material()
    : m_diffuseColor(Color::white),
    m_specularColor(Color(0.2f, 0.2f, 0.2f, 1.f)) // �f�t�H���g�̋��ʔ��ːF�͍T���߂ȊD�F��
{
    // �S�Ẵ|�C���^�ƃn���h�����[���N���A
    for (int i = 0; i < (int)TextureSlot::Max; ++i)
    {
        m_textures[i] = nullptr;
        m_textureHandles[i] = { 0 };
    }
}

void Material::SetTexture(TextureSlot slot, Texture2D* texture, DescriptorAllocator* allocator)
{
    int slotIndex = (int)slot;
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        assert(false);
        return;
    }

    // 1. �e�N�X�`���ւ̃|�C���^���X�V
    m_textures[slotIndex] = texture;

    // 2. �e�N�X�`����nullptr�A�܂��̓A���P�[�^�������Ȃ�A�n���h�����N���A���ďI��
    if (!texture || !allocator)
    {
        m_textureHandles[slotIndex] = { 0 };
        return;
    }

    // 3. SRV�i�V�F�[�_�[���\�[�X�r���[�j�̃f�X�N���v�^�i�������j���쐬
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = texture->GetNativeResource()->GetDesc().Format; // �e�N�X�`���̃��\�[�X����t�H�[�}�b�g���擾
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;

    // 4. �A���P�[�^��SRV�̍쐬���˗����A�Ԃ��ꂽGPU�n���h����ۑ�
    m_textureHandles[slotIndex] = allocator->CreateSrv(texture->GetNativeResource(), srvDesc);
}

D3D12_GPU_DESCRIPTOR_HANDLE Material::GetGpuDescriptorHandle(TextureSlot slot) const
{
    int slotIndex = (int)slot;
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        assert(false);
        return { 0 };
    }
    return m_textureHandles[slotIndex];
}