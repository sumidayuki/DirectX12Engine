#pragma once
#include "ButtonControl.h"


//�u1���v��\���R���g���[��
class AxisControl
{
private:
    float m_value;

public:
    // �l��ݒ肵�܂��B
    void SetValue(float value) { m_value = value; }

    // �l���擾���܂��B
    float GetValue() const { return m_value; }
};


//�u2���v��\���R���g���[��
class Vector2Control
{
private:
    AxisControl m_x;
    AxisControl m_y;

public:
    // ��Ԃ��X�V���܂��B
    void Update();

    // X���R���g���[����Ԃ��܂��B
    const AxisControl& X() const { return m_x; }

    // Y���R���g���[����Ԃ��܂��B
    const AxisControl& Y() const { return m_x; }
};


//�u�A�i���O�X�e�B�b�N�v��\���R���g���[��
class StickControl : public Vector2Control
{
private:
    ButtonControl m_down;
    ButtonControl m_right;
    ButtonControl m_left;
    ButtonControl m_up;

public:
    // �X�e�B�b�N�̏�Ԃ��X�V���܂��B
    void Update(const Vector2& value);

    // �X�e�B�b�N��������(-1.0 �` 0.0)�ɓ��͂���Ă��邩�ǂ���
    const ButtonControl& Down() const { return m_down; }

    // �X�e�B�b�N���E����(0.0 �` 1.0)�ɓ��͂���Ă��邩�ǂ���
    const ButtonControl& Right() const { return m_right; }

    // �X�e�B�b�N��������(-1.0 �` 0.0)�ɓ��͂���Ă��邩�ǂ���
    const ButtonControl& Left() const { return m_left; }

    // �X�e�B�b�N���㔼��(0.0 �` 1.0)�ɓ��͂���Ă��邩�ǂ���
    const ButtonControl& Up() const { return m_up; }
};

