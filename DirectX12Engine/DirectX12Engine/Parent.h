#pragma once

struct Parent : public IComponentData
{
	Entity parent = INVALID_ENTITY;
};