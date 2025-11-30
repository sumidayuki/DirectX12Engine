#pragma once

struct Layer : IComponentData
{
	LayerMask layer = Layers::Default;
};