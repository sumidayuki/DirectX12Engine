#pragma once

/// <summary>
/// Input�������R���|�[�l���g�ł��B
/// </summary>
struct Input
{
	bool moveUp;		// �����
	bool moveDown;		// ������
	bool moveLeft;		// ������
	bool moveRight;		// �E����
	bool shot;			// �V���b�g

	float horizontal;
	float vartical;
	Vector3 direction;
};