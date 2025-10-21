#pragma once
#include <stdexcept>

template <typename T>
class List
{
private:
	T* m_data;
	size_t m_size;
	size_t m_capacity;

public:
	List();
	~List();

	/// <summary>
	/// 要素を追加します。
	/// </summary>
	/// <param name="value"></param>
	void Add(const T& value);

	/// <summary>
	/// 指定した位置の要素を削除して、後ろを前詰めします。
	/// </summary>
	/// <param name="index"></param>
	void RemoveAt(size_t index);

	/// <summary>
	/// 要素を全て削除します。
	/// </summary>
	void Clear();

	T& operator[](size_t index);

	size_t Count() const;
};

template<typename T>
inline List<T>::List()
	: m_data(nullptr)
	, m_size(0)
	, m_capacity(0)
{
}

template<typename T>
inline List<T>::~List()
{
	delete[] m_data;
}

template<typename T>
inline void List<T>::Add(const T& value)
{
	if (m_size >= m_capacity)
	{
		// 容量拡張
		size_t newCapacity = (m_capacity == 0) ? 4 : m_capacity * 2;
		T* newData = new T[newCapacity];

		// 既存データをコピー
		for (size_t i = 0; i < m_size; ++i)
		{
			newData[i] = m_data[i];
		}

		delete[] m_data;
		m_data = newData;
		m_capacity = newCapacity;
	}

	// 新しい要素を追加
	m_data[m_size] = value;
	++m_size;
}

template<typename T>
inline void List<T>::RemoveAt(size_t index)
{
	if (index >= m_size)
	{
		throw std::out_of_range("index out of range");
	}

	for (size_t i = index; i < m_size - 1; ++i)
	{
		m_data[i] = m_data[i + 1];
	}

	--m_size;
}

template<typename T>
inline void List<T>::Clear()
{
	m_size = 0;
}

template<typename T>
inline T& List<T>::operator[](size_t index)
{
	if (index >= m_size)
	{
		throw std::out_of_range("Index out of range");
	}

	return m_data[index];
}

template<typename T>
inline size_t List<T>::Count() const
{
	return m_size;
}
