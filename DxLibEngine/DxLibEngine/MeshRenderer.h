#pragma once

/// <summary>
/// 3D���b�V���������_�����O���邽�߂̃R���|�[�l���g�ł��B
/// �`�悷�郁�b�V����F�Ȃǂ̏��������܂��B
/// </summary>
struct MeshRenderer
{
    std::vector<ComPtr<Mesh>>     meshes;
    std::vector<ComPtr<Material>> materials;

    // �I�u�W�F�N�g���Ƃ̒萔�o�b�t�@
    ComPtr<GraphicsBuffer> constantBuffer = nullptr;
};