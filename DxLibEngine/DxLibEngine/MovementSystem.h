#pragma once

class MovementSystem : public System
{
public:
	// System�̍X�V�����̃I�[�o�[���C�h�ł��B
	void Update(ComponentManager& cm, World& world) override;
};