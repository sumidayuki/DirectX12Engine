#include "TransformSystem.h"

void TransformSystem::Translate(Transform& transform, const Vector3& translation)
{
	transform.position += translation;
	transform.dirty = true;
}

void TransformSystem::Rotate(Transform& transform, const Vector3 axis, float angle)
{
	const Quaternion q = Quaternion::AngleAxis(angle, axis);

	transform.rotation = q * transform.rotation;
	transform.dirty = true;
}

void TransformSystem::Start(ComponentManager& cm, World& world)
{
	m_view = std::make_unique<View<Transform>>(cm);
}

void TransformSystem::Update(ComponentManager& cm, World& world)
{
	m_view->Update();

	for (auto&& [entity, transform] : *m_view)
	{
		// entity �� enabled �ł͂Ȃ��Ȃ�X�L�b�v���܂��B�i�������s���K�v���Ȃ����߁j
		if (!entity.enabled) continue;

		// dirty �� false �Ȃ�X�L�b�v���܂��B�i�X�V���|����K�v���Ȃ����߁j
		if (!transform.dirty) continue;

		// �X�P�[���E��]�E�ʒu���烍�[�J���ϊ��s����v�Z���܂��B
		transform.localMatrix.SetSRT(transform.scale, transform.rotation, transform.position);

		// �e�G���e�B�e�B���ݒ肳��Ă���Ȃ�
		if (transform.parent != NullEntity)
		{
			// �e�� Transform ���擾���܂��B
			const auto& parentTransform = world.GetComponent<Transform>(transform.parent);

			// �e�̃��[���h�s����|���Ď��g�̃��[���h�s����Z�o���܂��B
			transform.worldMatrix = transform.localMatrix * parentTransform->worldMatrix;
		}
		// ����Ă��Ȃ��Ȃ�
		else
		{
			// ���[�J���s������̂܂܃��[���h�s��ɑ�����܂��B
			transform.worldMatrix = transform.localMatrix;
		}

		// �X�V���I������̂� dirty �t���O�� false �ɂ���B�i���̃t���[������X�V���|�����Ȃ��悤�ɂ��邽�߁j
		transform.dirty = false;
	}
}