#pragma once

/// <summary>
/// �O���b�h��� 1 �Z����\���܂��B
/// ���̃Z���̒��ɂ��� Entity ��z��Ŏ����܂��B
/// </summary>
struct GridCell 
{
	std::vector<Entity> entities;
};

/// <summary>
/// ��ԏ�̍��W���n�b�V���֐��ŃZ���ɕϊ����A�O���b�h�\���ŊǗ�����N���X�ł��B
/// </summary>
class SpatialHashGrid 
{
private:
	float cellSize;
	std::unordered_map<std::pair<int, int>, GridCell, PairHash> grid;

public:
	/// <summary>
	/// �����t���R���X�g���N�^
	/// 1�Z���̃T�C�Y�����߂܂��B
	/// </summary>
	/// <param name="size">1�Z���̃T�C�Y</param>
	SpatialHashGrid(float size) : cellSize(size) {}

	/// <summary>
	/// �ǂ̃Z���ɂ���̂��m�F���邽�߂̊֐��ł��B
	/// ��@Vector2(140, 70) �Ȃ� �O���b�h���W�i2, 1�j�ɕϊ����܂��B
	/// </summary>
	/// <param name="position">�����̍��W</param>
	/// <returns>�Z����</returns>
	std::pair<int, int> GetCellCoords(const Vector2& position) const;

	/// <summary>
	/// �O���b�h�̏��S�Ă��N���A���܂��B
	/// ����͑O�t���[���̏������Z�b�g���čX�V����ׂł��B
	/// </summary>
	void Clear();

	/// <summary>
	/// �w�肵�����W�ɑΉ����Ă���Z���� Entity ��o�^���܂��B
	/// ���̏��͏�ɐV������Ԃɂ���K�v������܂��B�iEntity �̍��W���ς�����肷�邽�߁j
	/// </summary>
	/// <param name="entity">�ǉ�����G���e�B�e�B</param>
	/// <param name="position">�G���e�B�e�B�̍��W</param>
	void AddEntity(const Entity& entity, const Vector2& position);

	/// <summary>
	/// �w�肵�����W�ɑΉ������Z���̎��͂ɂ��� 8�Z�� + �����̃Z���i�v9�Z���j�𒲂ׂāA
	/// �����ɓo�^����Ă��� Entity ��S�ĕς����܂��B
	/// </summary>
	/// <param name="position">�����̍��W</param>
	/// <returns>std::vector<Entity>�^�̃A�h���X��Ԃ��܂��B</returns>
	const std::vector<Entity>& GetNearbyEntities(const Vector2& position) const;
};
