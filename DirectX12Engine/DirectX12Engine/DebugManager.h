#pragma once

#include "DebugLineRenderer.h"
#include "DebugTextRenderer.h"
#include "DebugStats.h"

#ifdef DrawText
#undef DrawText
#endif

class DebugManager : public Singleton<DebugManager>
{
	friend class Singleton<DebugManager>;
	
public:
	struct TextItem
	{
		Vector3 position;
		std::wstring text;
		Color color;
	};


private:
	DebugLineRenderer* m_lineRenderer;
	bool m_isActive;

public: 
	/// <summary>
	/// Debug用のラインを描画します。
	/// </summary>
	/// <param name="start">始点</param>
	/// <param name="end">終点</param>
	/// <param name="color"></param>
	void DrawLine(const Vector3& start, const Vector3& end, const Color& color = Color::white);
	
	/// <summary>
	/// Debug用のAABBを描画します。
	/// </summary>
	/// <param name="min"></param>
	/// <param name="max"></param>
	/// <param name="color"></param>
	void DrawAABB(const Vector3& min, const Vector3& max, const Color& color = Color::white);
	
	/// <summary>
	/// Debug用の球体を描画します。
	/// </summary>
	/// <param name="center"></param>
	/// <param name="radius"></param>
	/// <param name="color"></param>
	void DrawSphere(const Vector3& center, float radius, const Color& color = Color::white);
	
	/// <summary>
	/// Debug用のテキストを描画します。
	/// </summary>
	/// <param name="position"></param>
	/// <param name="text"></param>
	/// <param name="color"></param>
	void DrawText(const Vector2& position, const std::wstring& text, const Color& color = Color::white);


public:
	void Initialize();
	void Render();
	void Shutdown();
};