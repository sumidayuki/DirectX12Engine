#pragma once

struct CharacterHitboxes : IComponentData
{
	std::unordered_map<uint32_t, Entity> entities;
};