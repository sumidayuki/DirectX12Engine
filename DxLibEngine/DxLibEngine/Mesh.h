#pragma once

#include "Reference.h"
#include "GraphicsBuffer.h"
#include "Bounds.h"

/// <summary>
/// 3D���f���̃W�I���g���f�[�^��\���܂��B
/// ���_�o�b�t�@�ƃC���f�b�N�X�o�b�t�@�ւ̎Q�Ƃ�ێ����܂��B
/// </summary>
class Mesh : public Reference
{
public:
    /// <summary>
    /// ���_�̃��C�A�E�g���`���܂��B
    /// �K�v�ɉ����Ė@���AUV���W�A�J���[�Ȃǂ�ǉ��ł��܂��B
    /// </summary>
    struct Vertex
    {
        Vector3 position; // ���_���W (x, y, z)
        Vector3 normal;   // �@��
        Vector2 uv;       // �e�N�X�`�����W (u, v)
    };

private:
    ComPtr<GraphicsBuffer>  m_vertexBuffer;   // ���_�o�b�t�@
    ComPtr<GraphicsBuffer>  m_indexBuffer;    // �C���f�b�N�X�o�b�t�@
    Bounds                  m_bounds;         // ���f����Ԃł̋��E�{�b�N�X
    UINT                    m_indexCount;     // �C���f�b�N�X��

public:
    // �R���X�g���N�^
    Mesh()
        : m_vertexBuffer(nullptr)
        , m_indexBuffer(nullptr)
        , m_indexCount(0)
    {
    }

    // ���z�f�X�g���N�^
    virtual ~Mesh() = default;

    /// <summary>
    /// ���_�f�[�^�ƃC���f�b�N�X�f�[�^���烁�b�V�����Z�b�g�A�b�v���܂��B
    /// </summary>
    /// <param name="vertices">���_�f�[�^�̔z��</param>
    /// <param name="indices">�C���f�b�N�X�f�[�^�̔z��</param>
    void SetupMesh(const std::vector<Vertex>& vertices, const std::vector<uint32_t>& indices)
    {
        // ���_�o�b�t�@�̍쐬
        m_vertexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Vertex,
            GraphicsBuffer::UsageFlags::None,
            (int)vertices.size(),
            sizeof(Vertex),
            vertices.data()
        ));

        // �C���f�b�N�X�o�b�t�@�̍쐬
        m_indexBuffer.Attach(new GraphicsBuffer(
            GraphicsBuffer::Target::Index,
            GraphicsBuffer::UsageFlags::None,
            (int)indices.size(),
            sizeof(uint32_t), // 3D���f���͒��_������������32bit�C���f�b�N�X���g�p
            indices.data()
        ));

        m_indexCount = (UINT)indices.size();

        // TODO: ���_�f�[�^����o�E���f�B���O�{�b�N�X���v�Z���鏈��
    }

    /// <summary>
    /// ���_�o�b�t�@���擾���܂��B
    /// </summary>
    /// <returns>���_�o�b�t�@�̃|�C���^</returns>
    GraphicsBuffer* GetVertexBuffer() const { return m_vertexBuffer.Get(); }

    /// <summary>
    /// �C���f�b�N�X�o�b�t�@���擾���܂��B
    /// </summary>
    /// <returns>�C���f�b�N�X�o�b�t�@�̃|�C���^</returns>
    GraphicsBuffer* GetIndexBuffer() const { return m_indexBuffer.Get(); }

    /// <summary>
    /// �C���f�b�N�X�����擾���܂��B
    /// </summary>
    /// <returns>�C���f�b�N�X��</returns>
    UINT GetIndexCount() const { return m_indexCount; }

    /// <summary>
    /// ���E�{�b�N�X���擾���܂��B
    /// </summary>
    /// <returns>���E�{�b�N�X</returns>
    const Bounds& GetBounds() const { return m_bounds; }
};