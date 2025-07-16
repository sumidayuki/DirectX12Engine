#include "Material.h"

Material::Material()
    : m_diffuseColor(Color::white),      // �f�t�H���g�̊g�U���ːF�͔�
    m_specularColor(Color(0.f, 0.f, 0.f, 1.f)), // �f�t�H���g�̋��ʔ��ːF�͍�
    m_descriptorHeap(nullptr)
{
    // �S�Ẵe�N�X�`���|�C���^��nullptr�ŏ�����
    for (int i = 0; i < (int)TextureSlot::Max; ++i)
    {
        m_textures[i] = nullptr;
    }
}

/// <summary>
/// �w�肳�ꂽ�X���b�g�Ƀe�N�X�`����ݒ肵�܂��B
/// �����Ńf�X�N���v�^�q�[�v���Ǘ����A�K�؂ȏꏊ�Ƀf�X�N���v�^���R�s�[���܂��B
/// </summary>
/// <param name="slot">�e�N�X�`����ݒ肷��X���b�g</param>
/// <param name="texture">�ݒ肷��e�N�X�`��</param>
void Material::SetTexture(TextureSlot slot, Texture2D* texture)
{
    int slotIndex = (int)slot;
    // �X���b�g�̃C���f�b�N�X���͈͓����`�F�b�N
    if (slotIndex < 0 || slotIndex >= (int)TextureSlot::Max)
    {
        // �͈͊O�̏ꍇ�͉������Ȃ��i�܂��̓A�T�[�g�j
        return;
    }

    // 1. �e�N�X�`���ւ̃|�C���^��z��̓K�؂ȏꏊ�ɕۑ�
    m_textures[slotIndex] = texture;

    // 2. �f�X�N���v�^�q�[�v���Ȃ���΁A�S�X���b�g�����m�ۂ��č쐬����
    if (!m_descriptorHeap)
    {
        m_descriptorHeap.Attach(new DescriptorHeap(
            DescriptorHeapType::CBV_SRV_UAV,
            (int)TextureSlot::Max, // �S�e�N�X�`���X���b�g���̃T�C�Y���m��
            true)); // �V�F�[�_�[���猩����悤�ɂ���
    }

    // 3. �e�N�X�`�����L���ȏꍇ�̂݁A�f�X�N���v�^���R�s�[����
    if (m_textures[slotIndex])
    {
        // �R�s�[��̃n���h�����A�X���b�g�̃C���f�b�N�X���g���Čv�Z
        D3D12_CPU_DESCRIPTOR_HANDLE copyTo = m_descriptorHeap->GetCPUDescriptorHandle(slotIndex);

        // �R�s�[���̃n���h�����擾
        //�i�e�e�N�X�`�������g�̃f�X�N���v�^��ێ����Ă��邱�Ƃ�O��Ƃ���j
        D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = m_textures[slotIndex]->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();

        // �f�o�C�X���擾���ăf�X�N���v�^���R�s�[
        Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    // ����: texture��nullptr�̏ꍇ�A���̃}�e���A���̃f�X�N���v�^�q�[�v����
    // �Ή�����f�X�N���v�^�͍X�V����܂��񂪁A�V�F�[�_�[���ŕ��򂷂�Ζ�肠��܂���B
}