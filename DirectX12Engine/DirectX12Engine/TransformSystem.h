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
	/// <summary>
	/// �w�肳�ꂽ transform �ɐe��ݒ肵�܂��B
	/// �e�q�֌W��ݒ肷��ƁA�e�̃g�����X�t�H�[���ɘA�����Ĉړ��E��]�E�X�P�[������܂��B
	/// </summary>
	/// <param name="transform">�q�ɂ���transform</param>
	/// <param name="parent">�e�ɂ���transform</param>
	static void SetParent(Transform& transform, Transform* parent);

	/// <summary>
	/// �e�q�֌W���������܂��B
	/// </summary>
	/// <param name="transform">�e����؂藣������transform</param>
	static void UnsetParent(Transform& transform);

	static Transform* GetRoot(Transform& transform);

	// �qTransform�̌����擾���܂��B
	static int GetChildCount(Transform* transform) { return (int)transform->children.size(); }

	// index�Ԗڂ̎qTransform���擾���܂��B
	static Transform* GetChild(Transform* transform, int index);

	static Transform* FindChild(Transform* transform, const std::string& name);

	static void SetLocalRotation(Transform& transform, const Quaternion& localRotation);

	static void SetLocalPosition(Transform& transform, const Vector3& localPosition);

	static void SetLocalPosition(Transform& transform, float x, float y, float z);

	static void RotateAround(Transform& transform, Vector3 point, Vector3 axis, float angle);

	static Vector3 GetPosition(Transform& transform);

	static const Matrix4x4& GetLocalToWorldMatrix(Transform& transform);

	static const Matrix4x4& GetWorldToLocalMatrix(Transform& transform);

	// ���[�J����Ԃ��烏�[���h��Ԃ� direction ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�u�ʒu�v�̉e�����󂯂܂���B
	// �Ԃ����x�N�g���� direction �Ɠ��������ɂȂ�܂��B
	static Vector3 TransformDirection(Transform& transform, const Vector3& direction);
	 
	// ���[�J����Ԃ��烏�[���h��Ԃ� vector ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�̉e�����󂯂܂����u�ʒu�v�̉e���͎󂯂܂���B
	// �Ԃ����x�N�g���̒����́Avector �Ƃ͈قȂ�ꍇ������܂��B
	static Vector3 TransformVector(Transform& transform, const Vector3& vector);

	// ���[�J����Ԃ��烏�[���h��Ԃ� position ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�u��]�v�u�ʒu�v�̉e�����󂯂܂��B
	static Vector3 TransformPoint(Transform& transform, const Vector3& position);

	// ���[���h��Ԃ��烍�[�J����Ԃ� direction ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�u�ʒu�v�̉e�����󂯂܂���B
	// �Ԃ����x�N�g���� direction �Ɠ��������ɂȂ�܂��B
	static Vector3 InverseTransformDirection(Transform& transform, const Vector3& direction);

	// ���[���h��Ԃ��烍�[�J����Ԃ� vector ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�̉e�����󂯂܂����u�ʒu�v�̉e���͎󂯂܂���B
	// �Ԃ����x�N�g���̒����́Avector �Ƃ͈قȂ�ꍇ������܂��B
	static Vector3 InverseTransformVector(Transform& transform, const Vector3& vector);

	// ���[���h��Ԃ��烍�[�J����Ԃ� position ��ϊ����܂��B
	// ���̕ϊ��́u�X�P�[���v�u��]�v�u�ʒu�v�̉e�����󂯂܂��B
	static Vector3 InverseTransformPoint(Transform& transform, const Vector3& position);

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