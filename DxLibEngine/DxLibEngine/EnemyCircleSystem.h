#pragma once
#include "TransformSystem.h"

/// <summary>
/// �N���X���FEnemyCircleSystem
/// �T�v�F�~�^�����s������̐����V�X�e���ł��B
/// �p�r�F����𐶐����鎞�A�������E�����Ԋu�Ȃǂ��m�F���Ȃ��琶�����s���܂��B
/// ���l�FSystem���p�����Ă���AUpdate�֐����I�[�o���C�h���Ă��܂��B
/// </summary>
class EnemyCircleSystem : public System
{
private:
	TransformSystem* m_transformSystem;

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};