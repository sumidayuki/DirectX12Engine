#include "Sprite.h"

// �R���X�g���N�^�i�摜�̕`��j
Sprite::Sprite(const std::string& filePath, int posX, int posY)
	: m_x(posX), m_y(posY)
{
	m_texture = LoadGraph(filePath.c_str());
}

// �f�X�g���N�^�i�摜�̍폜�j
Sprite::~Sprite()
{
	DeleteGraph(m_texture);
}

// �摜��`�悵�܂��B
void Sprite::Draw()
{
	DrawGraph(m_x, m_y, m_texture, TRUE);
}

// ���W��ύX���܂��B
void Sprite::SetPosition(int newX, int newY)
{
	m_x = newX;
	m_y = newY;
}
