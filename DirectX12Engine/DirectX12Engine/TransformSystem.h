#pragma once

/// <summary>
/// Transform�̊Ǘ����s���V�X�e���ł��B
/// Transform�R���|�[�l���g�̕ύX��Matrix���ɔ��f���܂��B
/// </summary>
class TransformSystem : public System
{
private:
	// �e���L�[�Ƃ��āA���̒��ڂ̎q�G���e�B�e�B�̃��X�g��ێ�����}�b�v
	std::unordered_map<Entity, std::vector<Entity>> m_hierarchy;

	// ���[�g�ƂȂ�G���e�B�e�B�i�e�����Ȃ����́j�̃��X�g
	std::vector<Entity> m_roots;

public:
	static void SetLocalRotation(Transform& transform, const Quaternion& localRotation);

	static void SetLocalPosition(Transform& transform, const Vector3& localPosition);

	static void SetLocalPosition(Transform& transform, float x, float y, float z);


	static const Matrix4x4& GetLocalToWorldMatrix(Transform& transform);

	static const Matrix4x4& GetWorldToLocalMatrix(Transform& transform);

	/// <summary>
	/// ���݂̈ʒu����w�肵���������ړ����܂��B
	/// </summary>
	/// <param name="transform">�ړ���������transform</param>
	/// <param name="translation">�ړ���</param>
	static void Translate(Transform& transform, const Vector3& translation);

	/// <summary>
	/// �w�肵����]���Ɏw��̊p�x������]�����܂��B
	/// </summary>
	/// <param name="transform">��]��������transform</param>
	/// <param name="axis">��]��</param>
	/// <param name="angle">�p�x</param>
	static void Rotate(Transform& transform, const Vector3 axis, float angle);

private:
	static void RecalculateMatricesIfNeeded(Transform& transform);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};