#pragma once

class Plane
{
private:
    // ���ʂ̖@���x�N�g��(a,b,c)
    Vector3 m_normal;

    // ���ʂ���@���ɉ��������_�܂ł̋���(d)
    float m_distance;

public:
    // �f�t�H���g�R���X�g���N�^
    Plane() = default;

    // ���ʂ��`����@���x�N�g���Ƌ�������ɐV�������ʂ��쐬���܂��B
    // ���� inNormal �͐��K������Ă��邱�Ƃ�z�肵�Ă��܂��B
    Plane(const Vector3& inNormal, float distance);
};