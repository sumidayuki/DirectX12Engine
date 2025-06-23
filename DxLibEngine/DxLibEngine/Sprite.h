#pragma once

/// <summary>
/// Spriteを扱うコンポーネントです。
/// 第一成分：handle	（テクスチャのpath）
/// 第二成分：width		（任意）
/// 第三成分：height	（任意）
/// 第四成分：pivot		（任意）
/// 第五成分：r			（任意）
/// 第六成分：g			（任意）
/// 第七成分：b			（任意）
/// 第八成分：layer		（任意）
/// 第九成分：isActive	（任意）
/// </summary>
struct Sprite
{
	int handle;					// textureのpath
	int width = 32;				// 横幅
	int height = 32;			// 縦幅
	float pivot = 0.5f;			// ピボット

	int r = 255;
	int g = 255;
	int b = 255;

	int layer = 0;
	
	bool isActive = true;
};