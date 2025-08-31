#pragma once

/// <summary>
/// �N���X���FSystem
/// �T�v�F�X�V�����E�`�揈�����s�����N���X�ł��B
/// �p�r�F���̃N���X���p�����邱�Ƃ�ECS�̃V�X�e���Ƃ��ē��삵�܂��B
/// ���l�F�g�p����ɂ́Aworld��System��ǉ�����K�v������܂��B
/// </summary>
class System
{
public:
	virtual void Start(ComponentManager& cm, World& world) {};

	/// <summary>
	/// System�̍X�V�������s���܂��B�����͔C�ӂł��B
	/// </summary>
	/// <param name="cm">�R���|�[�l���g�}�l�[�W���[�̎Q�ƁBView���g�p���鎞�ȂǂɎg���܂��B</param>
	/// <param name="world">���[���h�̎Q�ƁBEntity�𐶐�������ȂǂɎg���܂�</param>
	virtual void Update(ComponentManager& cm, World& world) {}

	virtual void InternalRender(ComponentManager& cm, World& world) {}

	/// <summary>
	/// System�̕`�揈�����s���܂��B�����͔C�ӂł��B
	/// </summary>
	/// <param name="cm"></param>
	/// <param name="world"></param>
	virtual void Draw(ComponentManager& cm, World& world) {}

	virtual ~System() = default;
};