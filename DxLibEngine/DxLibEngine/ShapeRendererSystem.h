#pragma once

/// <summary>
/// �}�`�Ȃǂ̕`����s���V�X�e���ł��B
/// �}�`�R���|�[�l���g���������Ă���S�ẴG���e�B�e�B�̐}�`������ɕ`�悵�܂��B
/// </summary>
class ShapeRendererSystem : public System
{
public:
	void Draw(ComponentManager& cm, World& world) override;
};