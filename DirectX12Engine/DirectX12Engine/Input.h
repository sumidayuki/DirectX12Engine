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
	bool dash;
	bool attack;

	float horizontal;
	float vartical;
	Vector2 direction;
};