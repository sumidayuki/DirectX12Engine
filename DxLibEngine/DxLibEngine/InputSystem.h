#pragma once

/// <summary>
/// ���͂��󂯎��V�X�e���ł��B
/// </summary>
class InputSystem : public System
{
public:
	// System�̍X�V�����̃I�[�o�[���C�h�ł��B
	void Update(ComponentManager& cm, World& world) override;
};