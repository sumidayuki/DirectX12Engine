#pragma once

/// <summary>
/// Animator�R���|�[�l���g���������A�{�[���̌������X�V����V�X�e���B
/// </summary>
class AnimationSystem : public System
{
public:
	void Update(ComponentManager& cm, World& world) override;
};