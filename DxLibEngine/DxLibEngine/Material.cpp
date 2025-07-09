#include "Material.h"


Material::Material()
    : m_mainTexture(nullptr)
    , m_color(Color::white)
    , m_descriptorHeap(nullptr)
{
}

void Material::SetColor(const Color color)
{
    m_color = color;
}

void Material::SetMainTexture(Texture2D* texture)
{
    m_mainTexture = texture;

    // �e�N�X�`�����ݒ肳�ꂽ�ꍇ
    if (m_mainTexture)
    {
        // �����A���̃}�e���A���p�̃f�B�X�N���v�^�q�[�v���܂�����Ă��Ȃ���΍쐬����
        if (!m_descriptorHeap)
        {
            m_descriptorHeap.Attach(new DescriptorHeap(DescriptorHeapType::CBV_SRV_UAV, 1, true));
        }

        // �e�N�X�`��������SRV�i�V�F�[�_�[���\�[�X�r���[�j���A���̃}�e���A���p�̃q�[�v�ɃR�s�[����
        // ����ɂ��A���̃}�e���A�����V�F�[�_�[�ɓn������������
        D3D12_CPU_DESCRIPTOR_HANDLE copyTo = m_descriptorHeap->GetCPUDescriptorHandle(0);
        D3D12_CPU_DESCRIPTOR_HANDLE copyFrom = m_mainTexture->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        Graphics::GetD3D12Device()->CopyDescriptorsSimple(1, copyTo, copyFrom, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }
    else
    {
        // �e�N�X�`����nullptr�ɐݒ肳�ꂽ�ꍇ�́A�q�[�v���������
        m_descriptorHeap = nullptr;
    }
}