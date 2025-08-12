#pragma once

/// <summary>
/// ECSにおけるパフォーマンスの鍵となるデータ構造。
/// コンポーネントをメモリ上で隙間なく管理します。
/// </summary>
/// <typeparam name="T">管理するコンポーネントの型</typeparam>
template <typename T>
class PackedArray
{
private:
	// 密な配列
	// 実際のコンポーネントデータを、メモリ上で隙間なく連続して格納します。
	// これを先頭から舐めることで、CPUキャッシュが最大限に活かされます。
	std::vector<T> m_component_array;

	// 疎な配列
	// 「エンティティID」をインデックスとして、「密な配列のインデックス」を格納します。
	// これにより、特定のエンティティのデータがどこにあるかを瞬時に見つけられます。
	std::vector<int> m_entity_to_index_map;

	// 逆引き用の配列
	// 「密な配列のインデックス」をインデックスとして、「エンティティID」を格納します。
	// これは、要素を削除する際の「末尾入れ替え（swap-and-pop）」処理で必要になります。
	std::vector<Entity> m_index_to_entity_map;

public:
	/// <summary>
	/// コンポーネントを追加します。
	/// </summary>
	void AddComponent(Entity entity, const T& component)
	{
		// すでにコンポーネントを持っているかチェック
		assert(!HasComponent(entity) && "Component added to the same entity more than once.");

		// 疎な配列のサイズが足りなければ拡張する
		if (entity >= m_entity_to_index_map.size())
		{
			// 大きめのサイズでリサイズし、無効な値(-1)で初期化
			m_entity_to_index_map.resize(entity.id + 10, -1);
		}

		// 新しいインデックスは、現在の密な配列の末尾
		int newIndex = m_component_array.size();

		// 対応表を更新
		m_entity_to_index_map[entity] = newIndex;
		m_index_to_entity_map.push_back(entity);

		// 実際のコンポーネントデータを密な配列の末尾に追加
		m_component_array.push_back(component);
	}

	/// <summary>
	/// コンポーネントを削除します（swap-and-pop）。
	/// </summary>
	void RemoveComponent(Entity entity)
	{
		assert(HasComponent(entity) && "Removing non-existent component.");

		// 削除対象のインデックスを取得
		int indexOfRemoved = m_entity_to_index_map[entity];

		// 密な配列の最後の要素を取得
		T& lastComponent = m_component_array.back();
		Entity lastEntity = m_index_to_entity_map.back();

		// 最後の要素を、削除対象の場所に上書き（移動）する
		m_component_array[indexOfRemoved] = lastComponent;

		// 対応表を更新する
		//    - 移動してきた要素（lastEntity）の新しい場所を記録
		m_entity_to_index_map[lastEntity] = indexOfRemoved;
		//    - 削除されたエンティティの対応を無効にする
		m_entity_to_index_map[entity] = -1;

		// 逆引き用の配列も同様に更新
		m_index_to_entity_map[indexOfRemoved] = lastEntity;

		// 密な配列と逆引き用配列の末尾の、不要になった要素を削除
		m_component_array.pop_back();
		m_index_to_entity_map.pop_back();
	}

	/// <summary>
	/// コンポーネントへのポインタを取得します。
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
	/// コンポーネントを持っているか確認します。
	/// </summary>
	bool HasComponent(Entity entity)
	{
		// 疎な配列の範囲外か、対応するインデックスが無効(-1)でないかを確認
		return entity < m_entity_to_index_map.size() && m_entity_to_index_map[entity] != -1;
	}
};