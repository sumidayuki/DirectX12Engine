#pragma once

/// <summary>
/// ���b�V���̃����_�����O���@���`���܂��B
/// </summary>
struct MeshRenderer
{
    // �`��Ɏg�p����}�e���A���̃��X�g�B
    // Mesh�̃T�u���b�V���C���f�b�N�X�ƑΉ����܂��B
    std::vector<ComPtr<Material>> materials;
};