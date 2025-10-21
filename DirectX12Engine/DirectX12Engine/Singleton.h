#pragma once

template <typename T>
class Singleton
{
private:
	static inline T* m_singleton = nullptr;

protected:
	Singleton() = default;
	virtual ~Singleton() = default;

public:
	/// <summary>
	/// シングルトンオブジェクトを作成します。
	/// </summary>
	static void CreateSingleton() { if (!m_singleton) m_singleton = new T(); }

	static T* GetInstance() { return m_singleton; }

	/// <summary>
	/// シングルトンオブジェクトを破壊します。
	/// </summary>
	static void DestroySingleton()
	{
		if (m_singleton)
		{
			delete m_singleton;
			m_singleton = nullptr;
		}
	}
};