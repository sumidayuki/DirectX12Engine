#pragma once

/// <summary>
/// �摜�t�@�C����`�悷��V�X�e���ł��B
/// Sprite�R���|�[�l���g���������Ă���S�ẴG���e�B�e�B��Sprite������ɕ`�悵�܂��B
/// </summary>
class SpriteRendererSystem : public System
{
public:
	void Draw(ComponentManager& cm, World& world) override;
};