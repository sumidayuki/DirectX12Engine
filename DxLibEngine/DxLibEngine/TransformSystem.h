#pragma once

/// <summary>
/// Transform�̊Ǘ����s���V�X�e���ł��B
/// Transform�R���|�[�l���g�̕ύX��Matrix���ɔ��f���܂��B
/// </summary>
class TransformSystem : public System
{
private:
	std::unique_ptr<View<Transform>> m_view;

	std::unordered_map<Entity, std::vector<Entity>> m_hierarhy;

public:
	/// <summary>
	/// ���݂̈ʒu����w�肵���������ړ����܂��B
	/// </summary>
	/// <param name="transform">�ړ���������transform</param>
	/// <param name="translation">�ړ���</param>
	void Translate(Transform& transform, const Vector3& translation);

	/// <summary>
	/// �w�肵����]���Ɏw��̊p�x������]�����܂��B
	/// </summary>
	/// <param name="transform">��]��������transform</param>
	/// <param name="axis">��]��</param>
	/// <param name="angle">�p�x</param>
	void Rotate(Transform& transform, const Vector3 axis, float angle);

public:
	void Start(ComponentManager& cm, World& world) override;

	void Update(ComponentManager& cm, World& world) override;
};