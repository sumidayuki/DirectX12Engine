#pragma once

/// <summary>
/// 3D���b�V���������_�����O���邽�߂̃R���|�[�l���g�ł��B
/// �`�悷�郁�b�V����F�Ȃǂ̏��������܂��B
/// </summary>
struct MeshRenderer
{
    ComPtr<Mesh> mesh = nullptr;
    ComPtr<Material> material = nullptr;

    // �I�u�W�F�N�g���Ƃ̒萔�o�b�t�@
    ComPtr<GraphicsBuffer> constantBuffer = nullptr;

    bool isStarted = false;
};