#pragma once

struct EventSystem : IComponentData
{
	Entity currentHovered  = { (EntitySize)-1 };
	Entity currentPressed  = { (EntitySize)-1 };
	Entity currentSelected = { (EntitySize)-1 };
};