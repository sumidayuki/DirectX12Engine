#include "Sprite.h"

Sprite::Sprite(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY)
    : m_texture(texture)
    , m_rect(rect)
    , m_pivot(pivot)
    , m_pixelsPerUnitX(pixelsPerUnitX)
    , m_pixelsPerUnitY(pixelsPerUnitY)
    , m_bounds(Vector3::zero, Vector3::zero)
    , m_textureRectOffset(Vector2::zero)
{
}


Sprite* Sprite::Create(Texture2D* texture, const Rect& rect, const Vector2& pivot, float pixelsPerUnitX, float pixelsPerUnitY)
{
    // �����̑Ó����`�F�b�N
    assert(texture != nullptr);
    assert(rect.width <= texture->GetWidth());
    assert(rect.height <= texture->GetHeight());
    assert(pixelsPerUnitX > 0.0f);
    assert(pixelsPerUnitY > 0.0f);

    // ���f����ԓ��ł̃X�v���C�g�T�C�Y(�����ƍ���)���v�Z����
    Vector2 spriteSize;
    spriteSize.x = rect.width / pixelsPerUnitX;
    spriteSize.y = rect.height / pixelsPerUnitY;

    // �X�v���C�g�̍���������s�{�b�g�ʒu�܂ł̃I�t�Z�b�g���v�Z����
    Vector2 pivotOffset;
    pivotOffset.x = spriteSize.x * pivot.x;
    pivotOffset.y = spriteSize.y * pivot.y;
    
    const std::vector<Vector2> vertices
    {
        Vector2(0.0f - pivotOffset.x,         0.0f - pivotOffset.y), // �����̒��_
        Vector2(0.0f - pivotOffset.x, spriteSize.y - pivotOffset.y), // ����̒��_
        Vector2(spriteSize.x - pivotOffset.x,         0.0f - pivotOffset.y), // �E���̒��_
        Vector2(spriteSize.x - pivotOffset.x, spriteSize.y - pivotOffset.y), // �E��̒��_
    };

    // �C���f�b�N�X�z��̍쐬
    const std::vector<uint16_t> triangles
    {
        0, 1, 2,    // �����̎O�p�`
        2, 1, 3,    // �E��̎O�p�`
    };

    // �X�v���C�g�̍쐬
    Sprite* sprite = new Sprite(texture, rect, pivot, pixelsPerUnitX, pixelsPerUnitY);
    sprite->OverrideGeometry(vertices, triangles);
    return sprite;
}


void Sprite::CalcBoundingRect(const Vector2 vertices[], int vertexCount, Vector2& minPoint, Vector2& maxPoint)
{
    minPoint.x = Mathf::Infinity;
    minPoint.y = Mathf::Infinity;
    maxPoint.x = Mathf::NegativeInfinity;
    maxPoint.y = Mathf::NegativeInfinity;

    for (int i = 0; i < vertexCount; i++)
    {
        // ��U���[�J���ϐ��Ɋi�[����
        const float x = vertices[i].x;
        const float y = vertices[i].y;

        // X�������̍ŏ��l�ƍő�l���X�V���Ă���
        if (x < minPoint.x)
        {
            minPoint.x = x;
        }
        else if (x > maxPoint.x)
        {
            maxPoint.x = x;
        }

        // Y�������̍ŏ��l�ƍő�l���X�V���Ă���
        if (y < minPoint.y)
        {
            minPoint.y = y;
        }
        else if (y > maxPoint.y)
        {
            maxPoint.y = y;
        }
    }
}


void Sprite::OverrideGeometry(const std::vector<Vector2>& vertices, const std::vector<uint16_t>& triangles)
{
    // �O���t�B�b�N�X�o�b�t�@��j������
    m_vertexBuffer = nullptr;
    m_indexBuffer = nullptr;

    // �����o�ϐ��ɃR�s�[���Ă���
    m_vertices = vertices;
    m_triangles = triangles;

    // ���_��
    const int vertexCount = (int)vertices.size();

    // �S�Ă̒��_���܂����`�̍ŏ��l�ƍő�l�����߂�B
    Vector2 minPoint;
    Vector2 maxPoint;
    CalcBoundingRect(&m_vertices[0], vertexCount, minPoint, maxPoint);

    // ���E�{�b�N�X�����肷��
    m_bounds.SetMinMax(Vector3(minPoint, 0.0f), Vector3(maxPoint, 0.0f));

    // �����ƉE���UV���W�����߂�
    Vector2 minUV;
    minUV.x = m_rect.x / m_texture->GetWidth();
    minUV.y = m_rect.y / m_texture->GetHeight();

    Vector2 maxUV;
    maxUV.x = (m_rect.x + m_rect.width) / m_texture->GetWidth();
    maxUV.y = (m_rect.y + m_rect.height) / m_texture->GetHeight();

    // �X�v���C�g�T�C�Y(�����ƍ���)
    Vector2 spriteSize;
    spriteSize.x = maxPoint.x - minPoint.x;
    spriteSize.y = maxPoint.y - minPoint.y;

    // �e�N�X�`�����W(u,v)�̔z����쐬����
    m_uv.resize(vertexCount);
    m_minUV = Vector2::positiveInfinity;
    m_maxUV = Vector2::negativeInfinity;
    for (int i = 0; i < vertexCount; i++)
    {
        // ��`����������Ώۂ̒��_�܂ł̃I�t�Z�b�g
        Vector2 offset;
        offset.x = m_vertices[i].x - minPoint.x;
        offset.y = m_vertices[i].y - minPoint.y;

        // �X�v���C�g�T�C�Y(�����ƍ���)�����ꂼ��1.0�Ƃ������̃I�t�Z�b�g�̔䗦
        Vector2 ratio;
        ratio.x = offset.x / spriteSize.x;
        ratio.y = offset.y / spriteSize.y;

        // �e�N�X�`�����W(u,v)
        m_uv[i].x = minUV.x + (maxUV.x - minUV.x) * ratio.x;
        m_uv[i].y = minUV.y + (maxUV.y - minUV.y) * ratio.y;

        if (m_uv[i].x < m_minUV.x)
        {
            m_minUV.x = m_uv[i].x;
        }
        else if (m_uv[i].x > m_maxUV.x)
        {
            m_maxUV.x = m_uv[i].x;
        }

        if (m_uv[i].y < m_minUV.y)
        {
            m_minUV.y = m_uv[i].y;
        }
        else if (m_uv[i].y > m_maxUV.y)
        {
            m_maxUV.y = m_uv[i].y;
        }
    }

    //----------------------------------------------------------------------------------------------------------------------------------------------------
    // ���_�o�b�t�@�̍쐬 (�r�f�I��������ɒ��_�f�[�^��u������)
    //----------------------------------------------------------------------------------------------------------------------------------------------------
    std::vector<Vertex> initialData(vertexCount);
    for (int i = 0; i < vertexCount; i++)
    {
        initialData[i].position = m_vertices[i];     // �ʒu(x,y)���R�s�[
        initialData[i].uv = m_uv[i];                 // �e�N�X�`�����W(u,v)���R�s�[
    }
    m_vertexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Vertex, GraphicsBuffer::UsageFlags::None, vertexCount, sizeof(Vertex), &initialData[0]));

    // �C���f�b�N�X�z���p�ӂ��� (��U�V�X�e����������ɃC���f�b�N�X�f�[�^��p�ӂ��Ă���)

    // �C���f�b�N�X��
    const int indexCount = (int)m_triangles.size();

    // �C���f�b�N�X�o�b�t�@�̍쐬
    m_indexBuffer.Attach(new GraphicsBuffer(GraphicsBuffer::Target::Index, GraphicsBuffer::UsageFlags::None, indexCount, sizeof(uint16_t), &m_triangles[0]));
}