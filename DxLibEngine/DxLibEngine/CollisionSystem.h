#pragma once

/// <summary>
/// �����蔻����s���V�X�e���ł��B
/// Collider�R���|�[�l���g���������Ă���S�ẴG���e�B�e�B���m�̓����蔻����s���܂��B
/// </summary>
class CollisionSystem : public System
{
private:
	// �~�R���C�_�[���m���������Ă��邩���m�F���܂��B
	void CheckCollision(CircleCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB);

	// �~�R���C�_�[����`�R���C�_�[�ɓ������Ă��邩���m�F���܂��B
	void CheckCollision(CircleCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB);

	// ��`�R���C�_�[���m���������Ă��邩���m�F���܂��B
	void CheckCollision(BoxCollider2D& a, Transform& transformA, BoxCollider2D& b, Transform& transformB, Entity* entityB);

	// ��`�R���C�_�[���~�R���C�_�[�ɓ������Ă��邩���m�F���܂��B
	void CheckCollision(BoxCollider2D& a, Transform& transformA, CircleCollider2D& b, Transform& transformB, Entity* entityB);

public:
	void Update(ComponentManager& cm, World& world) override;
};