#pragma once

/// <summary>
/// Sprite�������R���|�[�l���g�ł��B
/// ��ꐬ���Fhandle	�i�e�N�X�`����path�j
/// ��񐬕��Fwidth		�i�C�Ӂj
/// ��O�����Fheight	�i�C�Ӂj
/// ��l�����Fpivot		�i�C�Ӂj
/// ��ܐ����Fr			�i�C�Ӂj
/// ��Z�����Fg			�i�C�Ӂj
/// �掵�����Fb			�i�C�Ӂj
/// �攪�����Flayer		�i�C�Ӂj
/// ��㐬���FisActive	�i�C�Ӂj
/// </summary>
struct Sprite
{
	int handle;					// texture��path
	int width = 32;				// ����
	int height = 32;			// �c��
	float pivot = 0.5f;			// �s�{�b�g

	int r = 255;
	int g = 255;
	int b = 255;

	int layer = 0;
	
	bool isActive = true;
};