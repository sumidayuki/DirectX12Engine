#include "Sprite.h"

// コンストラクタ（画像の描画）
Sprite::Sprite(const std::string& filePath, int posX, int posY)
	: m_x(posX), m_y(posY)
{
	m_texture = LoadGraph(filePath.c_str());
}

// デストラクタ（画像の削除）
Sprite::~Sprite()
{
	DeleteGraph(m_texture);
}

// 画像を描画します。
void Sprite::Draw()
{
	DrawGraph(m_x, m_y, m_texture, TRUE);
}

// 座標を変更します。
void Sprite::SetPosition(int newX, int newY)
{
	m_x = newX;
	m_y = newY;
}
