#pragma once

struct Box
{
	float x, y;
	int r = 255;
	int g = 255;
	int b = 255;

	int layer = 0;

	bool isActive = true;
};