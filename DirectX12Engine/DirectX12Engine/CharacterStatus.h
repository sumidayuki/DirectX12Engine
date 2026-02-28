#pragma once

struct CharacterStatus : IComponentData
{
	float maxHealth = 100.0f;
    float walkSpeed = 100.0f;
    float runSpeed = 250.0f;
};