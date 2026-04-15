#pragma once

struct UIObject : IComponentData
{
	bool					isEnabled = true;
	bool					isStarted = false;
};