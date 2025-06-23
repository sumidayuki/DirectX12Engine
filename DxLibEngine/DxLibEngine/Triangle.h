#pragma once

/// <summary>
/// �O�p�`��\���܂��B
/// �������F�����̒��_(x, y)
/// �������F�E���̒��_(x, y)
/// ��O�����F�V��̒��_(x, y)
/// �ǉ���RGB���ݒ�ł��܂����A�f�t�H���g�ł͔��F�ɂȂ��Ă��܂��B
/// </summary>
struct Triangle
{
	float x1, y1;
	float x2, y2;
	float x3, y3;
	int r = 255;
	int g = 255;
	int b = 255;

	int layer = 0;

	bool isActive = true;
};