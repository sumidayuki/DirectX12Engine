#pragma once

enum class RenderType
{
	Sprite,
	Circle,
	Box,
	Triangle,
};

enum class Layer : int
{
	BackGround = 0,
	Bullet = 4,
	Enemy = 5,
	Player = 6,
	UI = 10,
	Default = 1,
};

/// <summary>
/// �����_�����O�������R���|�[�l���g�ł��B
/// ��ꐬ���Flayer			�i�C�Ӂj 
/// ��񐬕��ForderInLayer	�i�C�Ӂj 
/// ��O�����Ftype			�i�����_�����O�^�C�v���w�肵�Ă��������j 
/// ��l�����Fsprite		�i�����_�����O�^�C�v�� Sprite �̏ꍇ�͐ݒ肵�Ă��������j 
/// ��ܐ����Fcircle		�i�����_�����O�^�C�v�� Circle �̏ꍇ�͐ݒ肵�Ă��������j 
/// ��Z�����Fbox			�i�����_�����O�^�C�v�� Box �̏ꍇ�͐ݒ肵�Ă��������j 
/// �掵�����Ftriangle		�i�����_�����O�^�C�v�� Triangle �̏ꍇ�͐ݒ肵�Ă��������j 
/// </summary>
struct RenderCommand
{
	Layer layer = Layer::Default;
	int orderInLayer = 0;

	RenderType type;
	Sprite sprite = Sprite{};
	Circle circle = Circle{};
	Box box = Box{};
	Triangle triangle = Triangle{};
};