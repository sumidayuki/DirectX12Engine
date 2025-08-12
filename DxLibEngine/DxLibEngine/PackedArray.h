#pragma once

/// <summary>
/// ECS�ɂ�����p�t�H�[�}���X�̌��ƂȂ�f�[�^�\���B
/// �R���|�[�l���g����������Ō��ԂȂ��Ǘ����܂��B
/// </summary>
/// <typeparam name="T">�Ǘ�����R���|�[�l���g�̌^</typeparam>
template <typename T>
class PackedArray
{
private:
	// ���Ȕz��
	// ���ۂ̃R���|�[�l���g�f�[�^���A��������Ō��ԂȂ��A�����Ċi�[���܂��B
	// �����擪�����r�߂邱�ƂŁACPU�L���b�V�����ő���Ɋ�������܂��B
	std::vector<T> m_component_array;

	// �a�Ȕz��
	// �u�G���e�B�e�BID�v���C���f�b�N�X�Ƃ��āA�u���Ȕz��̃C���f�b�N�X�v���i�[���܂��B
	// ����ɂ��A����̃G���e�B�e�B�̃f�[�^���ǂ��ɂ��邩���u���Ɍ������܂��B
	std::vector<int> m_entity_to_index_map;

	// �t�����p�̔z��
	// �u���Ȕz��̃C���f�b�N�X�v���C���f�b�N�X�Ƃ��āA�u�G���e�B�e�BID�v���i�[���܂��B
	// ����́A�v�f���폜����ۂ́u��������ւ��iswap-and-pop�j�v�����ŕK�v�ɂȂ�܂��B
	std::vector<Entity> m_index_to_entity_map;

public:
	/// <summary>
	/// �R���|�[�l���g��ǉ����܂��B
	/// </summary>
	void AddComponent(Entity entity, const T& component)
	{
		// ���łɃR���|�[�l���g�������Ă��邩�`�F�b�N
		assert(!HasComponent(entity) && "Component added to the same entity more than once.");

		// �a�Ȕz��̃T�C�Y������Ȃ���Ίg������
		if (entity >= m_entity_to_index_map.size())
		{
			// �傫�߂̃T�C�Y�Ń��T�C�Y���A�����Ȓl(-1)�ŏ�����
			m_entity_to_index_map.resize(entity.id + 10, -1);
		}

		// �V�����C���f�b�N�X�́A���݂̖��Ȕz��̖���
		int newIndex = m_component_array.size();

		// �Ή��\���X�V
		m_entity_to_index_map[entity] = newIndex;
		m_index_to_entity_map.push_back(entity);

		// ���ۂ̃R���|�[�l���g�f�[�^�𖧂Ȕz��̖����ɒǉ�
		m_component_array.push_back(component);
	}

	/// <summary>
	/// �R���|�[�l���g���폜���܂��iswap-and-pop�j�B
	/// </summary>
	void RemoveComponent(Entity entity)
	{
		assert(HasComponent(entity) && "Removing non-existent component.");

		// �폜�Ώۂ̃C���f�b�N�X���擾
		int indexOfRemoved = m_entity_to_index_map[entity];

		// ���Ȕz��̍Ō�̗v�f���擾
		T& lastComponent = m_component_array.back();
		Entity lastEntity = m_index_to_entity_map.back();

		// �Ō�̗v�f���A�폜�Ώۂ̏ꏊ�ɏ㏑���i�ړ��j����
		m_component_array[indexOfRemoved] = lastComponent;

		// �Ή��\���X�V����
		//    - �ړ����Ă����v�f�ilastEntity�j�̐V�����ꏊ���L�^
		m_entity_to_index_map[lastEntity] = indexOfRemoved;
		//    - �폜���ꂽ�G���e�B�e�B�̑Ή��𖳌��ɂ���
		m_entity_to_index_map[entity] = -1;

		// �t�����p�̔z������l�ɍX�V
		m_index_to_entity_map[indexOfRemoved] = lastEntity;

		// ���Ȕz��Ƌt�����p�z��̖����́A�s�v�ɂȂ����v�f���폜
		m_component_array.pop_back();
		m_index_to_entity_map.pop_back();
	}

	/// <summary>
	/// �R���|�[�l���g�ւ̃|�C���^���擾���܂��B
	/// </summary>
	T* GetComponent(Entity entity)
	{
		if (!HasComponent(entity))
		{
			return nullptr;
		}
		return &m_component_array[m_entity_to_index_map[entity]];
	}

	/// <summary>
	/// �R���|�[�l���g�������Ă��邩�m�F���܂��B
	/// </summary>
	bool HasComponent(Entity entity)
	{
		// �a�Ȕz��͈̔͊O���A�Ή�����C���f�b�N�X������(-1)�łȂ������m�F
		return entity < m_entity_to_index_map.size() && m_entity_to_index_map[entity] != -1;
	}
};