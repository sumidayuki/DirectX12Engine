#pragma once

class Rect
{
public:
	float x;		// ��`�̍����X���W
	float y;		// ��`�̍����Y���W
	float width;	// ��`�̉���
	float height;	// ��`�̍���

public:
	// �f�t�H���g�R���X�g���N�^
	Rect() = default;

	// �����t���R���X�g���N�^
	Rect(float x, float y, float width, float height);
};