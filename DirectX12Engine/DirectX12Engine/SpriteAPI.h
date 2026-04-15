#pragma once

namespace SpriteAPI
{
	struct ConstantBufferLayout
	{
		Matrix4x4       world;
		Color           color;
		Vector2         minUV;
		Vector2         maxUV;
		unsigned int    flipEnable[4];
	};

	void SetSprite(SpriteRenderer* spriteRenderer, Sprite* sprite);
}